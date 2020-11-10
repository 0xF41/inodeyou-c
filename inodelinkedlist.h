#ifndef INODELINKEDLIST_H_INCLUDED
#define INODELINKEDLIST_H_INCLUDED

// Used in linked list data structure functions
typedef struct inode_ll
{
    long num;
    struct inode_ll *next;
} inodenode;

// For linked list data structure (inodelinkedlist.c)
inodenode *create_inode_ll(long value);
inodenode *insert_inode_ll(inodenode *head, long value);
int find_inode_ll(inodenode *head, long val);
void destroy_inode_ll(inodenode *head);
void print_inode_ll(inodenode *head);
int count_inode_ll(inodenode *head);

#endif