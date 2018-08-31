#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/atomic.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/export.h>
#include <linux/user_namespace.h>

#include "ext4_jbd2.h"
#include "ext4.h"
#include "xattr.h"
#include "extacl.h"

extacl_t *
extacl_alloc(int count, gfp_t flags)
{
  const size_t size = sizeof(extacl_t) + count * sizeof(extacl_entry_t);
  printk(KERN_ERR "extacl_alloc(%d %d)\n", count, flags);
  extacl_t *extacl = kmalloc(size, flags);
  if (extacl) {
    // ext4_init_extacl(extacl, count);
  }
  return extacl;
}

static void *
ext4_extacl_to_disk(const extacl_t *extacl, size_t *size)
{
  extacl_xattr_header_t *result;
  char *e;
  size_t n;

  printk(KERN_ERR "ext4_extacl_to_disk(%p, %p)\n", extacl, size);
  *size = extacl_xattr_size(extacl->a_count);
  result = kmalloc(sizeof(extacl_xattr_header_t) + extacl->a_count *
            sizeof(extacl_xattr_entry_t), GFP_NOFS);
  if (!result) {
    return ERR_PTR(-ENOMEM);
  }

  result->a_version = cpu_to_le32(EXTACL_XATTR_VERSION);

  e = (char *) result + sizeof(extacl_xattr_header_t);

  for (n = 0; n < extacl->a_count; ++n) {
    const extacl_entry_t *extacl_e = &extacl->a_entries[n];
    extacl_xattr_entry_t *entry = (extacl_xattr_entry_t *) e;
    entry->e__pad = cpu_to_le16(extacl_e->e__pad);
    entry->e_uid_gid = cpu_to_le32(extacl_e->e_uid_gid);
    entry->e_range_start = cpu_to_le64(extacl_e->e_range_start);
    entry->e_range_len = cpu_to_le64(extacl_e->e_range_len);
    switch (extacl_e->e_type) {
      case EXTACL_USER_READ:
      case EXTACL_USER_WRITE:
      case EXTACL_GROUP_READ:
      case EXTACL_GROUP_WRITE:
        entry->e_type = cpu_to_le16(extacl_e->e_type);
        e += sizeof(extacl_xattr_entry_t);
        break;
      default:
        goto fail;
    }
  }

  return (void *) result;
fail:
  kfree(result);
  return ERR_PTR(-EINVAL);
}

static int
__ext4_set_extacl(struct inode *inode, extacl_t *extacl)
{
  const int name_index = EXT4_XATTR_INDEX_EXTACL;
  void *value = NULL;
  size_t size = 0;
  int error;
  printk(KERN_ERR "__ext4_set_extacl(%p %p)\n", inode, extacl);

  if (extacl) {
    value = ext4_extacl_to_disk(extacl, &size);
    if (IS_ERR(value)) {
      return (int) PTR_ERR(value);
    }
  }

  error = ext4_xattr_set(inode, name_index, XATTR_NAME_EXTACL,
    value, size, 0);

  kfree(value);

  if (!error) {
    // TODO set cached extacl !
  }

  return error;
}

/*
 * Initialize the ACLs of a new inode. Called from ext4_new_inode.
 *
 * dir->i_mutex: down
 * inode->i_mutex: up (access to inode is still exclusive)
 */
int
ext4_init_extacl(handle_t *handle, struct inode *inode, struct inode *dir)
{
  // TODO returns 0, -ENOSYS would break system (at least I think so)
  return 0;
}

extacl_t *
ext4_get_extacl(struct inode *inode)
{
  return ERR_PTR(-ENOSYS);
}

int
ext4_set_extacl(struct inode *inode, extacl_t *extacl)
{
    int error, retries = 0;
    printk(KERN_ERR "ext4_set_extacl(%p, %p)\n", inode, extacl);
retry:

  error = __ext4_set_extacl(inode, extacl);
  // TODO retry?
  if (error == -ENOSPC && ext4_should_retry_alloc(inode->i_sb, &retries)) {

    goto retry;
  }

  return error;
}
