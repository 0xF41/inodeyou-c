#ifndef FS_INODE_H_INCLUDED
#define FS_INODE_H_INCLUDED

#include "inodelinkedlist.h"

inodenode *get_fs_inodes(const char path[]);
inodenode *fs_walk_path(const char path[], inodenode *fs_ll);

#endif