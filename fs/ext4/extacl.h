/*
  File: fs/ext4/extacl.h

  Michał Piotr Stankiewicz, ms335789@students.mimuw.edu.pl
 */

#include <linux/extacl_xattr.h>
#include <linux/extacl.h>

#ifndef _EXTACL_H
#define _EXTACL_H

extacl_t *extacl_alloc(int count, gfp_t flags);
int ext4_init_extacl(handle_t *handle, struct inode *inode, struct inode *dir);
extacl_t *ext4_get_extacl(struct inode *inode);
int ext4_set_extacl(struct inode *inode, extacl_t *extacl);

#endif /* _EXTACL_H */
