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

int path_setxattr(const char __user *pathname,
			 const char __user *name, const void __user *value,
			 size_t size, int flags, unsigned int lookup_flags);

static extacl_xattr_header_t *
copy_data(const struct extacl_entry __user *entries,
  const size_t count, size_t *size)
{
  int i;
  extacl_xattr_header_t *result;
  char *e;
  struct extacl_entry entry;
  printk(KERN_ERR "copy_data\n");

  *size = extacl_xattr_size(count);
  if (*size == -1) {
    // TODO chekc error
    result = ERR_PTR(-EINVAL);
    goto copy_data_out;
  }
  printk(KERN_ERR "got size %ld\n", *size);

  if (*size > XATTR_SIZE_MAX) {
    result = ERR_PTR(-E2BIG);
    goto copy_data_out;
  }

  // TODO may be bad alloc!
  result = kmalloc(*size, GFP_KERNEL|__GFP_NOWARN);
  if (!result) {
    result = vmalloc(*size);
    if (!result) {
      result = ERR_PTR(-ENOMEM);
      goto copy_data_out;
    }
  }

  printk(KERN_ERR "allocated space\n");

  result->a_version = 1; // TODO fix
  e = (char *) (result + sizeof(extacl_xattr_header_t));

  for (i = 0; i < count; ++i) {
    extacl_xattr_entry_t *rentry = (extacl_xattr_entry_t *) e;
    printk(KERN_ERR "copy data for i=%d\n", i);
    if(copy_from_user(&entry, entries, sizeof(struct extacl_entry))) {
      result = ERR_PTR(-EINVAL);
      goto copy_data_kfree;
    }
    printk(KERN_ERR "copied data from user\n");
    // TODO error ?
    switch (entry.e_type) {
      case EXTACL_USER_READ:
      case EXTACL_USER_WRITE:
      case EXTACL_GROUP_READ:
      case EXTACL_GROUP_WRITE:
        rentry->e_type = cpu_to_le16(entry.e_type);
        break;
      default:
        // check error
        result = ERR_PTR(-EINVAL);
        goto copy_data_kfree;
    }
    printk(KERN_ERR "moved e_type\n");
    rentry->e__pad = cpu_to_le16(entry.e__pad);
    printk(KERN_ERR "moved e__pad\n");
    rentry->e_uid_gid = cpu_to_le32(entry.e_uid_gid);
    printk(KERN_ERR "moved e_uid_gid\n");
    rentry->e_range_start = cpu_to_le64(entry.e_range_start);
    printk(KERN_ERR "moved e_range_start\n");
    rentry->e_range_len = cpu_to_le64(entry.e_range_len);
    printk(KERN_ERR "moved e_range_len\n");
    e += sizeof(extacl_xattr_entry_t);
    printk(KERN_ERR "moved pointer\n");
  }

  printk(KERN_ERR "copied all data\n");
  goto copy_data_out;

copy_data_kfree:
  kfree(result); // TODO FIX
copy_data_out:
  return result;
}

ssize_t
extacl_set(const char __user *pathname, struct extacl_entry __user *entries,
  const size_t count)
{
  const size_t size = (sizeof (struct extacl_entry) * 1);
  const int flags = 0; // XATTR_CREATE|XATTR_REPLACE;
  const unsigned int lookup_flags = LOOKUP_FLAGS;
  return path_setxattr(pathname, XATTR_NAME_EXTACL "jazda", entries, size, flags, lookup_flags);
}
