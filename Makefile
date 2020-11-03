all:
	clang -Wall inodeyou.c inodelinkedlist.c tsk_inode.c fs_inode.c -ltsk -o inodeyou
clean:
	rm inodeyou
	rm fs_inodes.txt
	rm tsk_inodes.txt