#include "main.h"

int delete_last(DList **head, DList **tail)
{
    if(!(*head) && !(*tail))
    {
	return FAILURE;
    }

    if((*head)->next == NULL)
    {
	free(*head);
	*head = NULL;
	*tail = NULL;

	return SUCCESS;
    }

    DList *temp = *head;


    while(temp->next->next)
    {
	temp = temp->next;
    }
    
    *tail = temp;
    free(temp->next);
    temp->next = NULL;

    return SUCCESS;
}
