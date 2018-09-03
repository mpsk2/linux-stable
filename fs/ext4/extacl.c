/*
  File: fs/ext4/extacl.c

  Michał Piotr Stankiewicz, <ms335789@students.mimuw.edu.pl>
 */

#include <linux/syscalls.h>
#include <linux/extacl_xattr.h>
#include "extacl.h"

SYSCALL_DEFINE3(extacl_get,
  const char __user *, pathname,
  const struct extacl_entry __user *, entries,
  size_t, count)
{
  return -ENOSYS;
}

SYSCALL_DEFINE3(extacl_set,
  const char __user *, pathname,
  const struct extacl_entry __user *, entries,
  size_t, count)
{
  return -ENOSYS;
}
