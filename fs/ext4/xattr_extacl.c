/*
 * linux/fs/ext4/xattr_extacl.c
 * Handler for storing security labels as extended attributes.
 */

#include <linux/string.h>
#include <linux/fs.h>
#include <linux/security.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include "ext4_jbd2.h"
#include "ext4.h"
#include "xattr.h"
#include "extacl.h"

static int
ext4_xattr_extacl_get(const struct xattr_handler *handler,
			struct dentry *unused, struct inode *inode,
			const char *name, void *buffer, size_t size)
{
	return ext4_xattr_get(inode, EXT4_XATTR_INDEX_EXTACL,
			      name, buffer, size);
}

static int
ext4_xattr_extacl_set(const struct xattr_handler *handler,
			struct dentry *unused, struct inode *inode,
			const char *name, const void *value,
			size_t size, int flags)
{
	return ext4_xattr_set(inode, EXT4_XATTR_INDEX_EXTACL,
			      name, value, size, flags);
}

const struct xattr_handler ext4_xattr_extacl_handler = {
	.prefix	= XATTR_PREFIX_EXTACL,
	.get	= ext4_xattr_extacl_get,
	.set	= ext4_xattr_extacl_set,
};
