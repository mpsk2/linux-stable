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

ssize_t
extacl_get(const char __user *pathname, struct extacl_entry __user *entries, size_t count)
{
  unsigned int lookup_flags = LOOKUP_FLAGS;
  struct path path;
  ssize_t error;
  struct dentry *dentry;
  struct inode *inode;
  size_t buffer_size;
  void *buffer;
  int number;
  extacl_xattr_entry_t *entry;
  struct extacl_entry sub_entry;
  int i;

  printk(KERN_ERR "DEBUG: extacl_get(%s, %p, %ld) for %s\n",
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

      // no records
      if ((error == 0) || (error == sizeof(extacl_xattr_header_t))) {
        error = 0;
        goto out2;
      }

      // there are actually records
      if ((error - sizeof(extacl_xattr_header_t) % sizeof(extacl_xattr_entry_t))) {
        // should not happen, but may
        error = -ENOSYS;
        goto out2;
      }

      number = (error - sizeof(extacl_xattr_header_t)) / sizeof(extacl_xattr_entry_t);
      if (count < number) {
        number = count;
      }
      entry = (extacl_xattr_entry_t *) (buffer + sizeof(extacl_xattr_header_t));
      for (i = 0; i < number; ++i) {
        sub_entry.e_type = le16_to_cpu(entry->e_type);
        sub_entry.e__pad = le16_to_cpu(entry->e__pad);
        sub_entry.e_uid_gid = le32_to_cpu(entry->e_uid_gid);
        sub_entry.e_range_start = le64_to_cpu(entry->e_range_start);
        sub_entry.e_range_len = le64_to_cpu(entry->e_range_len);
        error = copy_to_user(entries, &sub_entry, sizeof(struct extacl_entry));

        if (error) {
          goto out2;
        }

        entry += sizeof(extacl_xattr_entry_t);
        entries += sizeof(struct extacl_entry);
      }
      error = number;
    }
  }
out2:
  kfree(buffer);
out1:
  path_put(&path);
out0:
  return error;
}
