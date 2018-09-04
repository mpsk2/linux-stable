/*
  File: fs/ext4/xatter_extacl.h

  Michał Piotr Stankiewicz, <ms335789@students.mimuw.edu.pl>
 */

#include <linux/string.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/extacl.h>

#include <linux/extacl_xattr.h>

#include "ext4.h"
#include "ext4_jbd2.h"
#include "xattr.h"
#include "extacl.h"

static int
ext4_xattr_extacl_get(const struct xattr_handler *handler,
		    struct dentry *unused, struct inode *inode,
		    const char *name, void *buffer, size_t size)
{
  return ext4_xattr_get(inode, EXT4_XATTR_INDEX_EXTACL, name, buffer, size);
}

static int
ext4_xattr_extacl_set(const struct xattr_handler *handler,
		    struct dentry *unused, struct inode *inode,
		    const char *name, const void *value,
		    size_t size, int flags)
{
  return ext4_xattr_set(inode, EXT4_XATTR_INDEX_EXTACL, name, value, size, flags);
}

const struct xattr_handler ext4_xattr_extacl_handler = {
  .name = XATTR_NAME_EXTACL,
  .get = ext4_xattr_extacl_get,
  .set = ext4_xattr_extacl_set,
};
