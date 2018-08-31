/*
  File: fs/ext4/extacl.h

  Michał Piotr Stankiewicz, ms335789@students.mimuw.edu.pl
 */

#include <linux/extacl_xattr.h>
#include <linux/extacl.h>

#ifndef _EXTACL_H
#define _EXTACL_H

int ext4_init_extacl(handle_t *handle, struct inode *inode, struct inode *dir);

#endif /* _EXTACL_H */
