#include <linux/syscalls.h>
#include "extacl.h"

#define LOOKUP_FLAGS 0

static ssize_t path_extacl_get_file(const char __user *pathname,
    const struct extacl __user *extacl, size_t size)
{
  return -ENOSYS;
}

static ssize_t path_extacl_set_file(const char __user *pathname,
    const struct extacl __user *extacl, size_t size)
{
  return -ENOSYS;
}

SYSCALL_DEFINE3(extacl_get_file, const char __user *, pathname,
    const struct extacl __user *, extacl, size_t, size)
{
  return path_extacl_get_file(pathname, extacl, size);
}

SYSCALL_DEFINE3(extacl_set_file, const char __user *, pathname,
    const struct extacl __user *, extacl, size_t, size)
{
  return path_extacl_set_file(pathname, extacl, size);
}
