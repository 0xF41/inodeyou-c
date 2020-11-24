#ifndef TSK_INODE_H_INCLUDED
#define TSK_INODE_H_INCLUDED

// The sleuth kit header files
#include <tsk/libtsk.h>

#include <tsk/base/tsk_os.h>
#include <tsk/fs/tsk_fs.h>
#include <tsk/img/tsk_img.h>

#include "inodelinkedlist.h"

#define TEST_BY_INODE 1

// For my ffind functionality in tsk_inode.c
typedef struct
{
    TSK_INUM_T inode;
    uint8_t flags;
    uint8_t found;
} FFIND_DATA;

// For tsk & fs mechanisms (tsk_inode.c & fs_inode.c)
inodenode *get_tsk_inodes(const char vol[], char dir[]);
// inodenode *tsk_walk_path(TSK_FS_INFO *fs, char path[], inodenode *tsk_ll);
inodenode *tsk_walk_path_by_pwd(TSK_FS_INFO *fs, char path[], inodenode *tsk_ll); // testing shit
inodenode *tsk_walk_path(TSK_FS_INFO *fs, TSK_INUM_T dir_ino_num, inodenode *tsk_ll);
void inode_to_pwd(const char vol[], TSK_INUM_T dir_ino_num);

// For my version of ffind (tsk_inode.c)
TSK_WALK_RET_ENUM find_file_act(TSK_FS_FILE *fs_file, const char *a_path, void *ptr);
uint8_t my_tsk_fs_ffind(TSK_FS_INFO *fs, TSK_FS_FFIND_FLAG_ENUM lclflags,
                        TSK_INUM_T a_inode,
                        TSK_FS_ATTR_TYPE_ENUM type, uint8_t type_used,
                        uint16_t id, uint8_t id_used, TSK_FS_DIR_WALK_FLAG_ENUM flags);

#endif