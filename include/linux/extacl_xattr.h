/*
  File: linux/extacl_xatter.h

  Michał Piotr Stankiewicz, <ms335789@students.mimuw.edu.pl>
 */

#ifndef _LINUX_EXTACL_XATTR_H
#define _LINUX_EXTACL_XATTR_H

#include <uapi/linux/xattr.h>
#include <uapi/linux/extacl_xattr.h>
#include <linux/extacl.h>

static inline size_t
extacl_xattr_size(int count)
{
  return (sizeof(struct extacl_xattr_header) +
    count * sizeof(struct extacl_xattr_entry));
}

static inline int
extacl_xattr_count(size_t size)
{
  if (size < sizeof(struct extacl_xattr_header)) {
    return -1;
  }

  size -= sizeof(struct extacl_xattr_header);
  if (size % sizeof(struct extacl_xattr_header)) {
    return -1;
  }
  return size / sizeof(struct extacl_xattr_entry);
}

void extacl_fix_xattr_from_user(void *value, size_t size);
void extacl_fix_xattr_to_user(void *value, size_t size);

#endif /* _LINUX_EXTACL_XATTR_H */
