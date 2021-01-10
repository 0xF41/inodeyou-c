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

    // Put arguments into variables
    char volume[BUF_LEN];      // '/dev/sda1'
    char mount_point[BUF_LEN]; // '/'
    char root[BUF_LEN];        // '/'
    if (argc > 1)
    {
        strncpy(volume, argv[1], strlen(argv[1]) + 1);
    }
    if (argc > 2)
    {
        strncpy(mount_point, argv[2], strlen(argv[2]) + 1); // "/dev/sdb"
    }
    if (argc > 3)
    {
        strncpy(root, argv[3], strlen(argv[3]) + 1); // "/"
    }
    else
    {
        strncpy(root, "/", 2); // Default is "/". "/home/user1"
    }

    // Timing test
    // - time_tsk is the time taken to collect inodes using method 1
    // - time_fs is the time taken to collect inodes using method 2
    // - time_compare is the time taken to compare outputs of method 1 and method 2
    // - time_taken is the sum of all above times
    struct rusage before, after;
    double time_tsk, time_fs, time_compare;
    time_tsk = time_fs = time_compare = 0.0f;

    // Free pagecache and kernel memory
    system("/bin/sync");
    system("/bin/echo 3 > /proc/sys/vm/drop_caches");

    // Print out arguments used in current scan
    printf("======== INODEYOU-C =========\n");
    printf("Volume: %s\nMount Point: %s\nStarting Directiory: %s\n", volume, mount_point, root);

    // Output of method 1 and 2 stored in separate inodenode linked lists
    inodenode *tsk_ll_head = NULL;
    inodenode *fs_ll_head = NULL;
    int tsk_ll_length, fs_ll_length;

    // Record number of hidden files and directories found by Step 3
    int evil_hit = 0;

    // Step 1: Method 1: TSK
    getrusage(RUSAGE_SELF, &before);

    tsk_ll_head = get_tsk_inodes(volume, root);
    if (tsk_ll_head == NULL)
    {
        printf("Error: tsk_ll_head is NULL\n");
        exit(1);
    }
    if (tsk_ll_head->num == -1)
    {
        printf("Error: Method 1 linked list not populated.\n");
        exit(1);
    }

    getrusage(RUSAGE_SELF, &after);
    // Calculate time taken for method 1
    time_tsk = calculate_time(&before, &after);

    // Step 2: Method 2: FS
    getrusage(RUSAGE_SELF, &before);

    fs_ll_head = get_fs_inodes(root);
    if (fs_ll_head == NULL)
    {
        printf("Error: fs_ll_head is NULL\n");
        exit(1);
    }
    if (fs_ll_head->num == -1)
    {
        printf("Error: Method 2 linked list not populated.\n");
        exit(1);
    }

    getrusage(RUSAGE_SELF, &after);
    // Calculate time taken for method 2
    time_fs = calculate_time(&before, &after);

    // Step 3: Find anomalies between inodes collected in first 2 steps
    getrusage(RUSAGE_SELF, &before);

    // Get length of linked lists
    tsk_ll_length = count_inode_ll(tsk_ll_head);
    fs_ll_length = count_inode_ll(fs_ll_head);

    printf("\nNumber of inodes found in method 1 (get_tsk_inodes): %d\n", tsk_ll_length);
    printf("Number of inodes found in method 2 (get_fs_inodes): %d\n\n", fs_ll_length);

    // Check that all that is seen by TSK (get_tsk_inodes) is seen by ls commands (get_fs_inodes)
    for (inodenode *tmp = tsk_ll_head; tmp != NULL; tmp = tmp->next)
    {
        if (find_inode_ll(fs_ll_head, tmp->num) == 0)
        {
            printf("[WARNING] Missing inode %ld (", tmp->num);
            inode_to_pwd(volume, tmp->num);
            printf(")\n");
            evil_hit++;
        }
    }

    // Calculate time taken to find anomalies
    getrusage(RUSAGE_SELF, &after);
    time_compare = calculate_time(&before, &after);

    // Report results
    if (evil_hit > 0)
    {
        printf(WARNING_RESULT, evil_hit);
    }
    else
    {
        printf(SAFE_RESULT);
    }

    // Report time taken & benchmark
    printf("\nTIME IN method 1 (get_tsk_inodes):       %.2f s", time_tsk);
    printf("\nTIME IN method 2 (get_fs_inodes):        %.2f s", time_fs);
    printf("\nTIME IN finding anomalies:               %.2f s", time_compare);
    printf("\nTIME IN TOTAL:                           %.2f s", time_tsk + time_fs + time_compare);
    printf("\n\n");

    // Cleanup code
    destroy_inode_ll(tsk_ll_head);
    destroy_inode_ll(fs_ll_head);

    return 0;
}

// Returns number of seconds between b and a
double calculate_time(const struct rusage *b, const struct rusage *a)
{
    if (b == NULL || a == NULL)
    {
        return 0.0;
    }
    else
    {
        return ((((a->ru_utime.tv_sec * 1000000 + a->ru_utime.tv_usec) -
                  (b->ru_utime.tv_sec * 1000000 + b->ru_utime.tv_usec)) +
                 ((a->ru_stime.tv_sec * 1000000 + a->ru_stime.tv_usec) -
                  (b->ru_stime.tv_sec * 1000000 + b->ru_stime.tv_usec))) /
                1000000.0);
    }
}
