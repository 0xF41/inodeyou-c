CC = gcc
PROG_NAME = inodeyou

all:
	$(CC) -Wall -fno-stack-protector inodeyou.c inodelinkedlist.c tsk_inode.c fs_inode.c -ltsk -o $(PROG_NAME)
clean:
	rm -rf $(PROG_NAME)
	