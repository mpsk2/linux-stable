/*
   File: linux/extacl_xattr.h

   Michał Piotr Stankiewicz, <ms335789@students.mimuw.edu.pl>
 */

#ifndef __LINUX_EXTACL_XATTR_H
#define __LINUX_EXTACL_XATTR_H

#include <uapi/linux/xattr.h>
#include <uapi/linux/extacl_xattr.h>
#include <linux/extacl.h>

static inline size_t
extacl_xattr_size(int count)
{
  return sizeof(extacl_xattr_header_t) + count * sizeof(extacl_xattr_entry_t);
}

static inline int
extacl_xattr_count(size_t size)
{
  if (size < sizeof(extacl_xattr_header_t)) {
    return -1;
  }
  size -= sizeof(extacl_xattr_header_t);
  if (size % sizeof(extacl_xattr_entry_t)) {
    return -1;
  }
  return size / sizeof(extacl_xattr_entry_t);
}

#endif  /* __LINUX_EXTACL_XATTR_H */
