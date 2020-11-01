#define BUF_LEN 100
#define USAGE "Usage: ./inodeyou-c filesystem mountpoint [directory] (./inodeyou-c /dev/sda1 / /)\n"
#define CHECK_ROOT_ERR_MSG "You must be root to perform this function!\n"
#define TSK_INODES_FILE "tsk_inodes.txt"
#define FS_INODES_FILE "fs_inodes.txt"

#define usage() \
    printf(USAGE);

#define file_err_msg(msg) \
    printf("Error: File %s does not exist.\n", msg);