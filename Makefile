CC = gcc
PROG_NAME = inodeyou

all:
	$(CC) -Wall -g -ggdb3 -fno-stack-protector inodeyou.c inodelinkedlist.c tsk_inode.c fs_inode.c -ltsk -o $(PROG_NAME)
debug:
	$(CC) -Wall -g -ggdb3 -fno-stack-protector inodeyou.c inodelinkedlist.c tsk_inode.c fs_inode.c -ltsk 
clean:
	rm -rf $(PROG_NAME)
	