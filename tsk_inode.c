#include "inodeyou.h"

// Get list of inodes from tsk functions
int get_tsk_inodes(const char vol[], char dir[])
{
    const TSK_TCHAR *const images[1] = {vol};
    TSK_IMG_INFO *img = tsk_img_open(1, images, TSK_IMG_TYPE_DETECT, 0);
    if (img == NULL)
    {
        printf("Error: %s is an invalid volume\n", vol);
        exit(1);
    }
    TSK_FS_INFO *fs = tsk_fs_open_img(img, 0, TSK_FS_TYPE_DETECT);
    if (fs == NULL)
    {
        printf("Error: Only type ext2/3/4 supported\n");
        exit(1);
    }
    // tsk_walk_path
    TSK_FS_DIR *cur = tsk_fs_dir_open((TSK_FS_INFO *)fs, (const char *)dir);
    if (cur == NULL)
    {
        printf("Error: Falied to open directory \'%s\'\n", dir);
        exit(1);
    }

    FILE *datafile = fopen(TSK_INODES_FILE, "w");
    // inodenode *tsk_ll = create_inode_ll(-1);
    // if (tsk_ll == NULL)
    // {
    //     exit(1);
    // }

    // Finds inode number (dir_inode_num) of dir (/home/user1/)
    TSK_INUM_T dir_inode_num;
    tsk_fs_ifind_path(fs, dir, &dir_inode_num);
    tsk_walk_path(fs, dir_inode_num, datafile);

    fclose(datafile);

    // printf("Reached here");

    // Cleanup
    tsk_img_close(img);
    tsk_fs_close(fs);

    return 0;
}

// Write inode numbers of inodes in current dir into tsk_inodes.txt
int tsk_walk_path(TSK_FS_INFO *fs, TSK_INUM_T dir_ino_num, FILE *file)
{
    TSK_FS_DIR *cur = tsk_fs_dir_open_meta(fs, dir_ino_num);
    if (cur == NULL)
    {
        printf("Error: Falied to open inode number %lu", dir_ino_num);
        exit(1);
    }

    TSK_FS_FILE *tsk_dirent = NULL;
    TSK_INUM_T inode_num = -1;

    // tsk_fs_dir_getsize() returns number inodes in current directory
    for (int i = 0, n = tsk_fs_dir_getsize(cur); i < n; i++)
    {
        tsk_dirent = tsk_fs_dir_get(cur, i); // Get current inode
        inode_num = -1;
        if (tsk_dirent == NULL)
        {
            printf("Error: Failed to get directory entry (index %i)\n", i);
            exit(1);
        }
        if (!strcmp(tsk_dirent->name->name, ".") || !strcmp(tsk_dirent->name->name, ".."))
        {
            // . or ..
            continue;
        }
        else if (tsk_dirent->name->type == TSK_FS_NAME_TYPE_REG && tsk_dirent->meta->type == TSK_FS_META_TYPE_REG)
        {
            // Regular file
            // TODO: Check if file is recently deleted
            inode_num = tsk_dirent->meta->addr;
            printf("%d (file)\n", (int)inode_num);
            fprintf(file, "%i\n", (int)inode_num); // print current inode number of regular file to FILE* file
        }
        else if (tsk_dirent->name->type == TSK_FS_NAME_TYPE_DIR && tsk_dirent->meta->type == TSK_FS_META_TYPE_DIR)
        {
            // Directory
            inode_num = tsk_dirent->meta->addr;
            printf("%d (dir)\n", (int)inode_num); // print current inode number of dir to FILE* file
            fprintf(file, "%i\n", (int)inode_num);
            // Uncomment for recursive funciton to serach through directories recursively
            // tsk_walk_path(fs, inode_num, file);
        }
    }
    // Cleanup
    tsk_fs_dir_close(cur);
    return 0;
}

// Prints out pwd of given inode number
void inode_to_pwd(TSK_FS_INFO *fs, TSK_INUM_T dir_ino_num)
{
    // Copied from ffind.cpp lel
    TSK_FS_ATTR_TYPE_ENUM type = 0;
    uint8_t id_used = 0, type_used = 0;
    uint16_t id = 0;
    uint8_t ffind_flags = 0;
    int dir_walk_flags = TSK_FS_DIR_WALK_FLAG_RECURSE;
    dir_walk_flags |= TSK_FS_DIR_WALK_FLAG_ALLOC; // Filter for undeleted entries only

    tsk_fs_ffind(fs, (TSK_FS_FFIND_FLAG_ENUM)ffind_flags, dir_ino_num, type, type_used, id, id_used, (TSK_FS_DIR_WALK_FLAG_ENUM)dir_walk_flags);
}