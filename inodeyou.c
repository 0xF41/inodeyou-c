#include "inodeyou.h"

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
    get_fs_inodes(root);

    // TODO: Find sorting algorihm and other algorihm to compare sum shit

    // get_tsk_inodes in array A
    // get_fs_inodes in array B
    // for element in B:
    //     if element in A
    //         return True
    //     else
    //         return False

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

// int countlines(FILE *file)
// {
//     int lines = 0;
//     char ch;
//     while (!feof(file))
//     {
//         ch = fgetc(file);
//         if (ch == '\n')
//         {
//             lines++;
//         }
//     }
//     return lines;
// }

// clang inodeyou.c -ltsk -o inodeyou
// Usage: "sudo ./inodeyou /dev/sdb / /"
