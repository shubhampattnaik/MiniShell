#include "main.h"

int i;
int print_list(DList *head)
{
    i = 0;
    
    if(head == NULL)
    {
	return FAILURE;
    }
    DList *temp = head;

    while (temp)
    {
	if(!temp->next)
	    printf("[%d]+ Stopped		%d\n", ++i, temp->pid);
	else if(!temp->next->next)
	    printf("[%d]- Stopped		%d\n", ++i, temp->pid);
	else
	    printf("[%d]  Stopped		%d\n", ++i, temp->pid);
	
	temp = temp->next;
    }
    
    return SUCCESS;
}
