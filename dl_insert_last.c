#include "main.h"

int insert_at_last(DList **head, data_t pid, data_t pgid, DList **tail)
{
    //Create new node
    DList *new = malloc(sizeof(DList));

    if(!new)
    {
	return FAILURE;
    }
    //printf("inserted last: %d\n", pgid);

    //if new node created, update data & link
    new->prev = NULL;
    new->pid = pid;
    new->pgid = pgid;
    new->next = NULL;

    //if list is empty
    if(*head == NULL && *tail == NULL)
    {
	//puts("head tail = NULL");
	*tail = new;
	*head = new;
	//printf("head->pid %d tail->pid %d\n", (*head)->pid, (*tail)->pid);
	return SUCCESS;
    }

    new->prev = *tail;

    (*tail)->next = new;
    *tail = new;
    
    //printf("head->pid %d tail->pid %d\n", (*head)->pid, (*tail)->pid);
    //printf("head->%x tail->%x\n", *head, *tail);
    
    return SUCCESS;
}
