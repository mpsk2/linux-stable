#include <linux/syscalls.h>
#include "extacl.h"

SYSCALL_DEFINE3(extacl_get_file, const char __user *, pathname,
    const struct extacl __user *, extacl, size_t, size)
{
  struct path path;
  return -ENOSYS;
}

SYSCALL_DEFINE3(extacl_set_file, const char __user *, pathname,
    const struct extacl __user *, extacl, size_t, size)
{
  struct path path;
  return -ENOSYS;
}
