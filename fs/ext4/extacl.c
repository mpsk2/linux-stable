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

SYSCALL_DEFINE2(extacl_set_file, const char __user *, pathname,
    struct extacl_entry __user *, extacl)
{
  return extacl_set(pathname, extacl);
}

SYSCALL_DEFINE2(extacl_get_file, const char __user *, pathname,
    struct extacl_entry __user *, extacl)
{
  return extacl_get(pathname, extacl);
}
