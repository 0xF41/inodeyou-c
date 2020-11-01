#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <tsk/libtsk.h>

#include <tsk/base/tsk_os.h>
#include <tsk/fs/tsk_fs.h>
#include <tsk/img/tsk_img.h>

#include "inodeyou.h"

int get_tsk_inodes(const char vol[], char root[]);
int tsk_walk_path(TSK_FS_INFO *fs, TSK_INUM_T dir_ino_num, FILE *file);
int inode_to_pwd(TSK_FS_INFO *fs, TSK_INUM_T dir_ino_num);
int get_fs_inodes(const char path[]);

int countlines(FILE *file);

int main(int argc, char *argv[])
{
    // Check proper arguments
    if (argc < 2)
    {
        usage();
        return 1;
    }
    // Check GID = 0
    if (getgid() != 0)
    {
        printf(CHECK_ROOT_ERR_MSG);
        return 1;
    }

    char volume[BUF_LEN];      // '/dev/sda1'
    char mount_point[BUF_LEN]; // '/'
    char root[BUF_LEN];        // '/'
    if (argc > 1)
    {
        strncpy(volume, argv[1], strlen(argv[1]) + 1);
    }
    if (argc > 2)
    {
        strncpy(mount_point, argv[2], strlen(argv[2]) + 1); // /dev/sdb
    }
    if (argc > 3)
    {
        strncpy(root, argv[3], strlen(argv[3]) + 1); // /home/fai
    }
    else
    {
        strncpy(root, "/", 2); // Default is "/"
    }

    system("/bin/sync");
    system("/bin/echo 3 > /proc/sys/vm/drop_caches");

    printf("Volume: %s      Mount Point: %s     Starting Directiory: %s\n", volume, mount_point, root);

    // http://www.sleuthkit.org/sleuthkit/docs/api-docs/4.3/fspage.html
    get_tsk_inodes(volume, root);
    // get_fs_inodes(root);

    // FILE *tsk_file = fopen(TSK_INODES_FILE, "r");
    // if (tsk_file == NULL)
    // {
    //     // printf("Error: %s does not exist\n", TSK_INODES_FILE);
    //     file_err_msg(TSK_INODES_FILE);
    // }
    // int tsk_inode_count = countlines(tsk_file);
    // int tsk_inode_array[tsk_inode_count];

    // // TODO: Fix comparison of fs_inodes.txt and tsk_inodes.txt
    // FILE *fs_file = fopen(FS_INODES_FILE, "r");
    // if (fs_file == NULL)
    // {
    //     file_err_msg(FS_INODES_FILE);
    // }
    // int fs_inode_count = countlines(fs_file);
    // int fs_inode_array[fs_inode_count];

    // char buffer[BUF_LEN];
    // for (int i = 0; i < tsk_inode_count; i++)
    // {
    //     fgets(buffer, sizeof(buffer), tsk_file);
    //     tsk_inode_array[i] = atoi(buffer);
    // }

    // for (int i = 0; i < tsk_inode_count; i++)
    // {
    //     printf("%i\n", tsk_inode_array[i]);
    // }

    // // Cleanup
    // fclose(tsk_file);
    // fclose(fs_file);
    return 0;
}

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

// Return 1 if inode is deleted
int inode_to_pwd(TSK_FS_INFO *fs, TSK_INUM_T dir_ino_num)
{
    // For ffind stuff
    TSK_FS_ATTR_TYPE_ENUM type;
    uint8_t id_used = 0, type_used = 0;
    uint16_t id = 0;
    uint8_t ffind_flags = 0;
    int dir_walk_flags = TSK_FS_DIR_WALK_FLAG_RECURSE;
    dir_walk_flags |= TSK_FS_DIR_WALK_FLAG_ALLOC; // For undeleted entries

    if (tsk_fs_ffind(fs, (TSK_FS_FFIND_FLAG_ENUM)ffind_flags, dir_ino_num, type, type_used, id, id_used, (TSK_FS_DIR_WALK_FLAG_ENUM)dir_walk_flags))
    {
        // ffind returns no error
        return 0;
    }
    // ffind finds error (i.e. deleted entries)
    return 1;
}

int get_fs_inodes(const char path[])
{
    struct stat stats;
    // https://codeforwin.org/2018/03/c-program-find-file-properties-using-stat-function.html

    if (stat(path, &stats) != 0)
    {
        printf("Unable to get file properties.\n");
        printf("Please check whether '%s' file exists.\n", path);
        exit(1);
    }

    DIR *folder = opendir(path);
    if (folder == NULL)
    {
        printf("Error: Unable to read directory %s", path);
        exit(1);
    }
    struct dirent *entry;
    struct stat filestat;
    FILE *datafilefs = fopen(FS_INODES_FILE, "w");
    if (datafilefs == NULL)
    {
        printf("Error: Unable to open %s", "fs_inodes.txt");
        exit(1);
    }

    int inode_number = -1;
    while ((entry = readdir(folder)) != NULL)
    {
        // Directory
        if (entry->d_type == 4)
        {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            {
                // "." and ".."
                continue;
            }
            inode_number = (int)entry->d_ino;
            printf("%s (Dir)\n", entry->d_name);
        }
        // File
        else if (entry->d_type == 8)
        {
            // Regular file
            inode_number = (int)entry->d_ino;
        }
        fprintf(datafilefs, "%i\n", inode_number);
    }

    // Cleanup
    closedir(folder);
    fclose(datafilefs);

    return 0;
}

int countlines(FILE *file)
{
    int lines = 0;
    char ch;
    while (!feof(file))
    {
        ch = fgetc(file);
        if (ch == '\n')
        {
            lines++;
        }
    }
    return lines;
}

// clang inodeyou.c -ltsk -o inodeyou
// Usage: "sudo ./inodeyou /dev/sdb / /"
