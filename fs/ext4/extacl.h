/*
  File: fs/ext4/extacl.h

  Michał Piotr Stankiewicz, ms335789@students.mimuw.edu.pl
 */

#include <linux/extacl_xattr.h>
#include <linux/extacl.h>

#ifndef _EXTACL_H
#define _EXTACL_H

#define LOOKUP_FLAGS 0

// extacl_get.c
ssize_t extacl_get(const char __user *pathname,
  struct extacl_entry __user *entries, const size_t count);

ssize_t extacl_set(const char __user *pathname,
  struct extacl_entry __user *entries, const size_t count);

#endif /* _EXTACL_H */
