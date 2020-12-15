#include "fs_inode.h"
#include "inodeyou.h"

inodenode *get_fs_inodes(const char path[])
{
    struct stat stats;
    // https://codeforwin.org/2018/03/c-program-find-file-properties-using-stat-function.html

    if (stat(path, &stats) != 0)
    {
        printf("Unable to get file properties.\n");
        printf("Please check whether '%s' file exists.\n", path);
        exit(1);
    }

    inodenode *fs_ll = create_inode_ll(-1);
    if (fs_ll == NULL)
    {
        printf("Error: Failed to create fs_ll inodenode node\n");
        exit(1);
    }

    fs_ll = fs_walk_path(path, fs_ll);

    return fs_ll;
}

inodenode *fs_walk_path(const char path[], inodenode *fs_ll)
{
    DIR *folder = opendir(path);
    if (folder == NULL)
    {
        printf("Error: Unable to read directory (%s)", path);
        return fs_ll;
    }

    struct dirent *entry = NULL;
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
            // printf("%s (Dir)\n", entry->d_name);
            fs_ll = insert_inode_ll(fs_ll, (long)inode_number);

            // Recursive functionality
            if (RECURSIVE_TEST)
            {
                char buffer[BUF_LEN_LARGE];
                sprintf(buffer, "%s/%s", path, entry->d_name);
                fs_ll = fs_walk_path(buffer, fs_ll);
            }
        }
        // File
        else if (entry->d_type == 8)
        {
            // Regular file
            inode_number = (int)entry->d_ino;
            // printf("%s (File)\n", entry->d_name);
            fs_ll = insert_inode_ll(fs_ll, (long)inode_number);
        }
    }

    // Cleanup
    closedir(folder);

    return fs_ll;
}