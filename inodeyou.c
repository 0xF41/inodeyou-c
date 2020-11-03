#include "inodeyou.h"

// Usage: "sudo ./inodeyou /dev/sda1 / /home/user1"

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

    inodenode *tsk_ll_head = NULL;
    inodenode *fs_ll_head = NULL;
    int tsk_ll_length, fs_ll_length;

    // http://www.sleuthkit.org/sleuthkit/docs/api-docs/4.3/fspage.html
    tsk_ll_head = get_tsk_inodes(volume, root);
    if (tsk_ll_head == NULL)
    {
        printf("Error: tsk_ll_head is NULL\n");
        exit(1);
    }
    fs_ll_head = get_fs_inodes(root);
    if (fs_ll_head == NULL)
    {
        printf("Error: fs_ll_head is NULL\n");
        exit(1);
    }

    tsk_ll_length = count_inode_ll(tsk_ll_head);
    fs_ll_length = count_inode_ll(fs_ll_head);
    // printf("%d | %d\n", tsk_ll_length, fs_ll_length);

    // Check for disreptencies between tsk linked list and fs linked list
    if (tsk_ll_length >= fs_ll_length)
    {
        for (inodenode *tmp = tsk_ll_head; tmp != NULL; tmp = tmp->next)
        {
            // printf("%ld | %d", tmp->num, find_inode_ll(fs_ll_head, tmp->num));
            if (find_inode_ll(fs_ll_head, tmp->num) == 0)
            {
                printf("Warning: Missing inode %ld\n", tmp->num);
            }
        }
    }
    if (fs_ll_length >= tsk_ll_length)
    {
        for (inodenode *tmp = fs_ll_head; tmp != NULL; tmp = tmp->next)
        {
            if (find_inode_ll(tsk_ll_head, tmp->num) == 0)
            {
                printf("Warning: Missing inode %ld\n", tmp->num);
            }
        }
    }

    // TODO: Compare tsk_ll_head and fs_ll_head linked list

    // Cleanup code
    destroy_inode_ll(tsk_ll_head);
    destroy_inode_ll(fs_ll_head);

    return 0;
}
