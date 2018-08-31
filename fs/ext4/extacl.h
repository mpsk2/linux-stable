/*
  File: fs/ext4/extacl.h

  Michał Piotr Stankiewicz, ms335789@students.mimuw.edu.pl
 */

#include <linux/extacl_xattr.h>

#ifndef _EXTACL_H
#define _EXTACL_H

#define EXTACL_USER_READ 0x1
#define EXTACL_USER_WRITE 0x2
#define EXTACL_GROUP_READ 0x3
#define EXTACL_GROUP_WRITE 0x4

struct extacl {
    uint16_t type;
    uint16_t _pad;
    uint32_t uid_gid;
    uint64_t range_start;
    uint64_t range_len;
};

int extacl_set_file(const char *path, struct extacl *extacl, size_t extacl_len);
int extacl_get_file(const char *path, struct extacl *extacl, size_t extacl_len);

#endif /* _EXTACL_H */
