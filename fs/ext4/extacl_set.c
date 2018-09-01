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
extacl_set(const char __user *pathname, struct extacl_entry __user *entries,
  const size_t count)
{
  return -ENOSYS;
}
