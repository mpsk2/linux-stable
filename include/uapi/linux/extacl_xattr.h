/*
  File: uapi/linux/extacl_xattr.h

  Michał Piotr Stankiewicz, ms335789@students.mimuw.edu.pl
 */

#ifndef __UAPI_LINUX_EXTACL_XATTR_H
#define __UAPI_LINUX_EXTACL_XATTR_H

#include <linux/types.h>

#define EXTACL_XATTR_VERSION 0x0001

#define EXTACL_UNDEFINED_ID (-1)

struct extacl_xattr_entry {
    __le16 e_type;
    __le16 e__pad;
    __le32 e_uid_gid;
    __le64 e_range_start;
    __le64 e_range_len;
};

struct extacl_xattr_header {
  __le32   a_version;
};

typedef struct extacl_xattr_entry extacl_xattr_entry_t;
typedef struct extacl_xattr_header extacl_xattr_header_t;

#endif /* __UAPI_LINUX_EXTACL_XATTR_H */
