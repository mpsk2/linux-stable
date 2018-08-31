#include "ext4_jbd2.h"
#include "ext4.h"
#include "xattr.h"
#include "extacl.h"

static int
__ext4_set_extacl(handle_t *handle, struct inode *inode, extacl_t *extacl)
{
  return -ENOSYS;
}

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

extacl_t *
ext4_get_extacl(struct inode *inode)
{
  return ERR_PTR(-ENOSYS);
}

int
ext4_set_extacl(struct inode *inode, extacl_t *extacl)
{
  handle_t *handle;
  int error, retries = 0;

retry:
  handle = ext4_journal_start(inode, EXT4_HT_XATTR,
            ext4_jbd2_credits_xattr(inode));
  if (IS_ERR(handle)) {
    return PTR_ERR(handle);
  }

  error = __ext4_set_extacl(handle, inode, extacl);
  ext4_journal_stop(handle);

  // TODO retry?
  if (error == -ENOSPC && ext4_should_retry_alloc(inode->i_sb, &retries))
    goto retry;

  return -ENOSYS;
}
