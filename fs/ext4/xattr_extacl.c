#include <linux/capability.h>
#include <linux/cred.h>
#include <linux/namei.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/syscalls.h>
#include <linux/vmalloc.h>
#include <linux/kernel.h>
#include <asm/current.h>

#include "ext4.h"
#include "extacl.h"

#define LOOKUP_FLAGS 0

static ssize_t is_allowed(struct inode *inode)
{
  // TODO check CAP_FOWNER
  if ((inode->i_uid.val != get_current_user()->uid.val)) {
    return -EACCES;
  }
  return 0;
}

static ssize_t path_extacl_set_file(const char __user *pathname,
    struct extacl_entry __user *extacl, size_t extacl_len)
{
  unsigned int lookup_flags = LOOKUP_FLAGS;
  extacl_t *pass;
  struct path path;
  struct inode *inode;
  ssize_t error;
  size_t i;

// TODO retry?
  error = user_path_at(AT_FDCWD, pathname, lookup_flags, &path);

  if (error) {
    return error;
  }

  inode = d_inode(path.dentry);

  pass = extacl_alloc(extacl_len, GFP_NOFS);

  if (IS_ERR(pass)) {
    error = (ssize_t) PTR_ERR(pass);
    goto set_file_out;
  }

  pass->a_count = extacl_len;

  for (i = 0; i < extacl_len; ++i) {
    pass->a_entries[i] = extacl[i];

  }

  error = ext4_set_extacl(inode, pass);

set_file_out:
  path_put(&path);

  // TODO retry?

  return error;
}

SYSCALL_DEFINE3(extacl_get_file, const char __user *, pathname,
    struct extacl_entry __user *, extacl, size_t, extacl_len)
{
  return extacl_get(pathname, extacl, extacl_len);
}

static ssize_t
sub_extacl_set2(struct dentry *d, const char *kname,
  struct extacl_entry *extacl,
  const size_t size, int flags)
{
    return -ENOSYS;
}

static ssize_t
ext4_extacl_set2(const char __user *pathname,
  struct extacl_entry __user *extacl,
  size_t extacl_len)
{
  unsigned int lookup_flags = LOOKUP_FLAGS;
  struct path path;
  ssize_t error;
  const size_t size = extacl_len * sizeof(struct extacl_entry);
  void *kvalue = NULL;
  const char *kname = XATTR_NAME_EXTACL;
  const int flags = XATTR_CREATE|XATTR_REPLACE;

  error = user_path_at(AT_FDCWD, pathname, lookup_flags, &path);

  if (error) {
    return error;
  }

  if (extacl_len > 0) {
    // it is actually 64k!
    if (extacl_xattr_size(extacl_len) > XATTR_SIZE_MAX) {
      return -E2BIG;
    }
    kvalue = kmalloc(size, GFP_KERNEL | __GFP_NOWARN);
    if (!kvalue) {
      kvalue = vmalloc(size);
      if (!kvalue) {
        goto path_put_l;
        error = -ENOMEM;
      }
    }
    if (copy_from_user(kvalue, extacl, size)) {
      error = -EFAULT;
      goto out2;
    }
  }

  error = sub_extacl_set2(path.dentry, kname, extacl, size, flags);

out2:
  kfree(kvalue);
path_put_l:
  path_put(&path);
  return -ENOSYS;
}

SYSCALL_DEFINE3(extacl_set_file, const char __user *, pathname,
    struct extacl_entry __user *, extacl, size_t, extacl_len)
{
  return ext4_extacl_set2(pathname, extacl, extacl_len);
}
