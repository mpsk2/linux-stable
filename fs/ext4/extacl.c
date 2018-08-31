#include "ext4_jbd2.h"
#include "ext4.h"
#include "xattr.h"
#include "extacl.h"

/*
 * Initialize the ACLs of a new inode. Called from ext4_new_inode.
 *
 * dir->i_mutex: down
 * inode->i_mutex: up (access to inode is still exclusive)
 */
int
ext4_init_extacl(handle_t *handle, struct inode *inode, struct inode *dir)
{
  // TODO returns 0, -ENOSYS would break system (at least I think so)
  return 0;
}
