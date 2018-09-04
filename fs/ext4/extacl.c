/*
  File: fs/ext4/extacl.c

  Michał Piotr Stankiewicz, <ms335789@students.mimuw.edu.pl>
 */

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <linux/xattr.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/security.h>
#include <linux/evm.h>
#include <linux/syscalls.h>
#include <linux/export.h>
#include <linux/fsnotify.h>
#include <linux/audit.h>
#include <linux/vmalloc.h>
#include <linux/posix_acl_xattr.h>

#include <asm/uaccess.h>

#include <linux/syscalls.h>
#include <linux/extacl_xattr.h>
#include "extacl.h"

ssize_t vfs_getxattr(struct dentry *dentry, const char *name, void *value,
  size_t size);

int vfs_setxattr(struct dentry *dentry, const char *name, const void *value,
		size_t size, int flags);

static int
fix_back_extacl(struct extacl_entry *values, size_t size)
{
  int i = 0;
  for (; i < (size / sizeof(struct extacl_entry)); i++) {
    struct extacl_entry *value = values + i;
    value->e_type = le16_to_cpu(value->e_type);
    value->e__pad = le16_to_cpu(value->e__pad);
    value->e_uid_gid = le32_to_cpu(value->e_uid_gid);
    value->e_range_start = le64_to_cpu(value->e_range_start);
    value->e_range_len = le64_to_cpu(value->e_range_len);
  }

  return 0;
}

static int
fix_and_validate_extacl(struct extacl_entry *values, size_t size)
{
  int i = 0;
  for (; i < (size / sizeof(struct extacl_entry)); i++) {
    struct extacl_entry *value = values + i;
    switch (value->e_type) {
      case EXTACL_USER_READ:
      case EXTACL_USER_WRITE:
      case EXTACL_GROUP_READ:
      case EXTACL_GROUP_WRITE:
        break;
      default:
        return -EINVAL; // TODO check error code
    }

    value->e_type = cpu_to_le16(value->e_type);
    value->e__pad = cpu_to_le16(value->e__pad);
    value->e_uid_gid = cpu_to_le32(value->e_uid_gid);
    value->e_range_start = cpu_to_le64(value->e_range_start);
    value->e_range_len = cpu_to_le64(value->e_range_len);
  }

  return 0;
}

/*
 * Extended attribute GET operations
 */
static ssize_t
getxattr(struct dentry *d, struct extacl_entry __user *value,
	 size_t size)
{
	ssize_t error;
	struct extacl_entry *kvalue = NULL;
  const char *name = XATTR_NAME_EXTACL;

	if (size) {
		if (size > XATTR_SIZE_MAX)
			size = XATTR_SIZE_MAX;
		kvalue = kzalloc(size, GFP_KERNEL | __GFP_NOWARN);
		if (!kvalue) {
			kvalue = vmalloc(size);
			if (!kvalue)
				return -ENOMEM;
		}
	}

	error = vfs_getxattr(d, name, kvalue, size);


	if (error > 0) {
		if (size && copy_to_user(value, kvalue, error))
			error = -EFAULT;
	} else if (error == -ERANGE && size >= XATTR_SIZE_MAX) {
		/* The file system tried to returned a value bigger
		   than XATTR_SIZE_MAX bytes. Not possible. */
		error = -E2BIG;
	}

  fix_back_extacl(kvalue, size);

	kvfree(kvalue);

	return error;
}

static ssize_t path_getxattr(const char __user *pathname,
  struct extacl_entry __user *value,
	size_t size, unsigned int lookup_flags)
{
	struct path path;
	ssize_t error;
retry:
	error = user_path_at(AT_FDCWD, pathname, lookup_flags, &path);
	if (error)
		return error;
	error = getxattr(path.dentry, value, size);
	path_put(&path);
	if (retry_estale(error, lookup_flags)) {
		lookup_flags |= LOOKUP_REVAL;
		goto retry;
	}
	return error;
}

/*
 * Extended attribute SET operations
 */
static long
setxattr(struct dentry *d, const struct extacl_entry __user *value,
	 size_t size, int flags)
{
	int error;
	struct extacl_entry *kvalue = NULL;
	const char *name = XATTR_NAME_EXTACL;

	if (flags & ~(XATTR_CREATE|XATTR_REPLACE))
		return -EINVAL;

	if (size) {
		if (size > XATTR_SIZE_MAX) {
      printk(KERN_ERR "size=%ld, error=%d\n", size, -E2BIG);
    	return -E2BIG;
    }
		kvalue = kmalloc(size, GFP_KERNEL | __GFP_NOWARN);
		if (!kvalue) {
			kvalue = vmalloc(size);
			if (!kvalue)
				return -ENOMEM;
		}
		if (copy_from_user(kvalue, value, size)) {
			error = -EFAULT;
			goto out;
		}
	}

  error = fix_and_validate_extacl(kvalue, size);
  if (error) {
    goto out;
  }

	error = vfs_setxattr(d, name, kvalue, size, flags);
out:
	kvfree(kvalue);

	return error;
}

static int path_setxattr(const char __user *pathname,
  const struct extacl_entry __user *value,
	size_t size, int flags, unsigned int lookup_flags)
{
	struct path path;
	int error;
retry:
	error = user_path_at(AT_FDCWD, pathname, lookup_flags, &path);
	if (error)
		return error;
	error = mnt_want_write(path.mnt);
	if (!error) {
		error = setxattr(path.dentry, value, size, flags);
		mnt_drop_write(path.mnt);
	}
	path_put(&path);
	if (retry_estale(error, lookup_flags)) {
		lookup_flags |= LOOKUP_REVAL;
		goto retry;
	}
	return error;
}

SYSCALL_DEFINE3(extacl_get,
  const char __user *, pathname,
  struct extacl_entry __user *, entries,
  size_t, count)
{
  ssize_t err;
  size_t size;
  size = sizeof(struct extacl_entry) * count;
  printk(KERN_ERR "size=%ld, count=%ld, sizeof=%ld\n, casted count %ld\n",
    size, count, sizeof(struct extacl_entry), (ssize_t) count);
  err = path_getxattr(pathname, entries, size, 0);
  if (err > 0) {
    err /= sizeof(struct extacl_entry);
  }
  return err;
}

SYSCALL_DEFINE3(extacl_set,
  const char __user *, pathname,
  const struct extacl_entry __user *, entries,
  size_t, count)
{
  size_t size;
  size = sizeof(struct extacl_entry) * count;
  printk(KERN_ERR "size=%ld, count=%ld, sizeof=%ld\n, casted count %ld\n",
    size, count, sizeof(struct extacl_entry), (ssize_t) count);
  return path_setxattr(pathname, entries, size, 0, 0);
}
