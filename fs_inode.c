#include "inodeyou.h"

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
    // struct stat filestat;
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
