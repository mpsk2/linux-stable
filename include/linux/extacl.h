/*
   File: linux/extacl.h

   Michał Piotr Stankiewicz, <ms335789@students.mimuw.edu.pl>
 */

#ifndef __LINUX_EXTACL_H
#define __LINUX_EXTACL_H

#include <linux/bug.h>
#include <linux/slab.h>
#include <linux/rcupdate.h>
#include <uapi/linux/extacl.h>

struct extacl_entry {
  uint16_t e_type;
  uint16_t e__pad;
  uint32_t e_uid_gid;
  uint64_t e_range_start;
  uint64_t e_range_len;
};

struct extacl {
  atomic_t            a_refcount;
  struct rcu_head     a_rcu;
  uint32_t            a_count;
  struct extacl_entry a_entries[0];
};

typedef struct extacl_entry extacl_entry_t;
typedef struct extacl extacl_t;

static inline extacl_t *
extacl_dup(extacl_t *extacl)
{
  if (extacl) {
    atomic_inc(&extacl->a_refcount);
  }
  return extacl;
}

static inline void
extacl_release(extacl_t *extacl)
{
  if (extacl && atomic_dec_and_test(&extacl->a_refcount)) {
    kfree_rcu(extacl, a_rcu);
  }
}

#endif  /* __LINUX_EXTACL_H */
