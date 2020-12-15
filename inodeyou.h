#ifndef INODEYOU_H_INCLUDED
#define INODEYOU_H_INCLUDED

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "fs_inode.h"
#include "inodelinkedlist.h"
#include "tsk_inode.h"

// Buffer sizes
#define BUF_LEN 100
#define BUF_LEN_LARGE 500

// Messages
#define USAGE "Usage: ./inodeyou-c volume mountpoint [directory] (./inodeyou-c /dev/sda1 / /)\n"
#define CHECK_ROOT_ERR_MSG "You must be root to perform this function!\n"
#define SAFE_RESULT "\n[OK] There are no files or directiories that are being currently hidden by a rootkit.\n"
#define WARNING_RESULT "\n[WARNING] There are %d files and directories that are currenty hidden by a rootkit.\n"

#define usage() \
    printf(USAGE);

#define file_err_msg(msg) \
    printf("Error: File %s does not exist.\n", msg);

// Flag to enable recursive functionality. Set to 0 for no recrsive functionality.
#define RECURSIVE_TEST 0

// For calcualting time taken
double calculate_time(const struct rusage *b, const struct rusage *a);

#endif