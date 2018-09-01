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
#include "xattr.h"

#define LOOKUP_FLAGS 0

ssize_t
extacl_get(const char __user *pathname, struct extacl_entry __user *entries,
  const size_t count)
{
  unsigned int lookup_flags = LOOKUP_FLAGS;
  struct path path;
  ssize_t error;
  struct dentry *dentry;
  struct inode *inode;
  size_t buffer_size;
  void *buffer;

  printk(KERN_ERR "extacl_get(%s, %p, %ld) for %s",
    pathname, entries, count, XATTR_NAME_EXTACL);

  error = user_path_at(AT_FDCWD, pathname, lookup_flags, &path);

  if (error) {
    goto out0;
  }

  dentry = path.dentry;
  inode = d_inode(dentry);

  buffer_size = extacl_xattr_size(count);

  if (buffer_size > 0) {
    if (buffer_size > XATTR_SIZE_MAX) {
      error = -E2BIG;
      goto out1;
    }

    buffer = kmalloc(buffer_size, GFP_KERNEL|__GFP_NOWARN);
    if (!buffer) {
      buffer = vmalloc(buffer_size);
      if (!buffer) {
        error = -ENOMEM;
        goto out1;
      }

      error = ext4_xattr_get(inode, EXT4_XATTR_INDEX_EXTACL,
        XATTR_NAME_EXTACL, buffer, buffer_size);
      if (error) {
        goto out2;
      }
    }
  }
out2:
  kfree(buffer);
out1:
  path_put(&path);
out0:
  return error;
}
