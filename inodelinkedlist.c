#include "inodeyou.h"

// Creates the first node of the inodenode linked list
inodenode *create_inode_ll(long value)
{
    inodenode *first = malloc(sizeof(inodenode));
    if (first == NULL)
    {
        exit(1);
    }
    first->num = value;
    first->next = NULL;
    return first;
}

// Inserts node in the front of the inodenode linked list, returns pointer to head (i.e. the new inserted node).
inodenode *insert_inode_ll(inodenode *head, long value)
{
    inodenode *new = malloc(sizeof(inodenode));
    if (new == NULL)
    {
        exit(1);
    }
    new->num = value;
    new->next = head;
    return new;
}

// Search through linked list to find an element
int find_inode_ll(inodenode *head, long val)
{
    for (inodenode *cur = head; cur != NULL; cur = cur->next)
    {
        if (cur->num == val)
        {
            return 1;
        }
    }
    return 0;
}

// Delete an entire linked list
void destroy_inode_ll(inodenode *head)
{
    while (head != NULL)
    {
        inodenode *tmp = head->next;
        free(head);
        head = tmp;
    }
}

// Print inodenode linked list
void print_inode_ll(inodenode *head)
{
    for (inodenode *tmp = head; tmp != NULL; tmp = tmp->next)
    {
        printf("%li\n", tmp->num);
    }
}