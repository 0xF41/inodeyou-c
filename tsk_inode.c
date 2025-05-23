#include "inodeyou.h"

// Returns linked list of inodes found by tsk functions
inodenode *get_tsk_inodes(const char vol[], char dir[])
{
    TSK_IMG_INFO *img = tsk_img_open_utf8_sing(vol, TSK_IMG_TYPE_DETECT, 0);
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

    inodenode *tsk_ll = create_inode_ll(-1);
    if (tsk_ll == NULL)
    {
        printf("Error: Failed to create tsk_ll inodenode node\n");
        exit(1);
    }

    // Finds inode number (dir_inode_num) of dir (/home/user1/)
    if (TEST_BY_INODE)
    {
        TSK_INUM_T dir_inode_num;
        tsk_fs_ifind_path(fs, dir, &dir_inode_num);
        tsk_ll = tsk_walk_path(fs, dir_inode_num, tsk_ll);
    }
    else
    {
        tsk_ll = tsk_walk_path_by_pwd(fs, dir, tsk_ll);
    }

    // Cleanup
    tsk_fs_close(fs);
    tsk_img_close(img);

    return tsk_ll;
}

// Returns linked list of inodes found by tsk functions
inodenode *tsk_walk_path(TSK_FS_INFO *fs, TSK_INUM_T dir_ino_num, inodenode *tsk_ll)
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
    for (size_t i = 0, n = tsk_fs_dir_getsize(cur); i < n; i++)
    {
        inode_num = -1;
        tsk_dirent = tsk_fs_dir_get(cur, i); // Get current inode
        if (tsk_dirent == NULL)
        {
            printf("Error: Failed to get directory entry (index %li, %s)\n", i, tsk_dirent->name->name);
            exit(1);
        }

        // May cause memory leak
        if (tsk_dirent->name->flags & TSK_FS_NAME_FLAG_UNALLOC)
        {
            // Check if file is recently deleted, if it is, skip.
            tsk_fs_file_close(tsk_dirent);
            continue;
        }

        if (!strcmp(tsk_dirent->name->name, ".") || !strcmp(tsk_dirent->name->name, ".."))
        {
            // If inode name is . or .. then skip
            tsk_fs_file_close(tsk_dirent);
            continue;
        }
        else if (tsk_dirent->meta->type == TSK_FS_META_TYPE_REG)
        {
            // Regular file
            inode_num = tsk_dirent->meta->addr;
            tsk_ll = insert_inode_ll(tsk_ll, (long)inode_num);
        }
        else if (tsk_dirent->meta->type == TSK_FS_META_TYPE_DIR && strcmp(tsk_dirent->name->name, "$OrphanFiles") != 0)
        {
            // Directory
            inode_num = tsk_dirent->meta->addr;
            tsk_ll = insert_inode_ll(tsk_ll, (long)inode_num);

            // Recursive functionality (might give some memory leaks)
            if (RECURSIVE_TEST)
            {
                tsk_ll = tsk_walk_path(fs, inode_num, tsk_ll);
            }
        }
        // Close tsk_dirent before next iteration
        tsk_fs_file_close(tsk_dirent);
    }

    // Cleanup
    tsk_fs_dir_close(cur);

    return tsk_ll;
}

// Returns linked list of inodes found by tsk functions
inodenode *tsk_walk_path_by_pwd(TSK_FS_INFO *fs, char path[], inodenode *tsk_ll)
{
    TSK_FS_DIR *cur = tsk_fs_dir_open(fs, path);
    if (cur == NULL)
    {
        exit(1);
    }

    TSK_FS_FILE *tsk_dirent = NULL;
    TSK_INUM_T inode_num = -1;

    // tsk_fs_dir_getsize() returns number inodes in current directory
    for (size_t i = 0, n = tsk_fs_dir_getsize(cur); i < n; i++)
    {
        inode_num = -1;
        tsk_dirent = tsk_fs_dir_get(cur, i); // Get current inode
        if (tsk_dirent == NULL)
        {
            printf("Error: Failed to get directory entry (index %li, %s)\n", i, tsk_dirent->name->name);
            exit(1);
        }
        if (tsk_dirent->name->flags & TSK_FS_NAME_FLAG_UNALLOC)
        {
            // Check if file is recently deleted, if it is, skip.
            tsk_fs_file_close(tsk_dirent);
            continue;
        }
        if (!strcmp(tsk_dirent->name->name, ".") || !strcmp(tsk_dirent->name->name, ".."))
        {
            // . or ..
            tsk_fs_file_close(tsk_dirent);
            continue;
        }
        else if (tsk_dirent->meta->type == TSK_FS_META_TYPE_REG)
        {
            // Regular file
            inode_num = tsk_dirent->meta->addr;
            tsk_ll = insert_inode_ll(tsk_ll, (long)inode_num);
        }
        else if (tsk_dirent->meta->type == TSK_FS_META_TYPE_DIR)
        {
            // Directory
            inode_num = tsk_dirent->meta->addr;
            tsk_ll = insert_inode_ll(tsk_ll, (long)inode_num);
            // Recursive functionality (Might cause some memory leaks)
            if (RECURSIVE_TEST)
            {
                char new_path[BUF_LEN_LARGE];
                sprintf(new_path, "%s/%s", path, tsk_dirent->name->name);
                tsk_ll = tsk_walk_path_by_pwd(fs, new_path, tsk_ll);
            }
        }
        // Close tsk_dirent before next iteration
        tsk_fs_file_close(tsk_dirent);
    }

    // Cleanup
    tsk_fs_dir_close(cur);

    return tsk_ll;
}

// Prints out pwd of given inode number
void inode_to_pwd(const char vol[], TSK_INUM_T dir_ino_num)
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
    // Copied from ffind.cpp lel
    TSK_FS_ATTR_TYPE_ENUM type = 0;
    uint8_t id_used = 0, type_used = 0;
    uint16_t id = 0;
    uint8_t ffind_flags = 0;
    int dir_walk_flags = TSK_FS_DIR_WALK_FLAG_RECURSE;
    // Filter for undeleted entries only
    dir_walk_flags |= TSK_FS_DIR_WALK_FLAG_ALLOC;

    // Prints out pwd of inode number
    my_tsk_fs_ffind(fs, (TSK_FS_FFIND_FLAG_ENUM)ffind_flags, dir_ino_num, type, type_used, id, id_used, (TSK_FS_DIR_WALK_FLAG_ENUM)dir_walk_flags);
    // Cleanup
    tsk_fs_close(fs);
    tsk_img_close(img);
}

// ffind function
uint8_t my_tsk_fs_ffind(TSK_FS_INFO *fs, TSK_FS_FFIND_FLAG_ENUM lclflags,
                        TSK_INUM_T a_inode,
                        TSK_FS_ATTR_TYPE_ENUM type, uint8_t type_used,
                        uint16_t id, uint8_t id_used, TSK_FS_DIR_WALK_FLAG_ENUM flags)
{
    FFIND_DATA data;

    data.found = 0;
    data.flags = lclflags;
    data.inode = a_inode;

    /* Since we start the walk on the root inode, then this will not show
     ** up in the above functions, so do it now
     */
    if (data.inode == fs->root_inum)
    {
        if (flags & TSK_FS_DIR_WALK_FLAG_ALLOC)
        {
            printf("\n");
            data.found = 1;
            if (!(lclflags & TSK_FS_FFIND_ALL))
                return 0;
        }
    }
    if (tsk_fs_dir_walk(fs, fs->root_inum, flags, find_file_act, &data))
    {
        return 1;
    }

    if (data.found == 0)
    {
        tsk_printf("File name not found for inode\n");
    }
    return 0;
}

// Callback function for tsk_fs_dir_walk
TSK_WALK_RET_ENUM find_file_act(TSK_FS_FILE *fs_file, const char *a_path, void *ptr)
{
    FFIND_DATA *data = (FFIND_DATA *)ptr;
    /* We found it! */
    if (fs_file->name->meta_addr == data->inode)
    {
        data->found = 1;
        if (fs_file->name->flags & TSK_FS_NAME_FLAG_UNALLOC)
            printf("* ");

        printf("%s%s", a_path, fs_file->name->name);

        if (!(data->flags & TSK_FS_FFIND_ALL))
        {
            return TSK_WALK_STOP;
        }
    }
    return TSK_WALK_CONT;
}