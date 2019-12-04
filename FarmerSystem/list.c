#include <stdlib.h>
#include <stdio.h>

#include "list.h"

List *new_list()
{
    List *result = malloc(sizeof(List));
    if (result)
    {
        result->head = result->tail = NULL;
    }
    else
    {
        fprintf(stderr, "%s:%d malloc failed\n", __FUNCTION__, __LINE__);
    }
    return result;
}

int list_length(List *list)
{
    ListItem *current = list->head;
    int length = 0;
    while (current)
    {
        current = current->next;
        length++;
    }
    return length;
}

void *list_add(List *list, void *value)
{
    ListItem *item = malloc(sizeof(ListItem));
    if (item == NULL)
    {
        fprintf(stderr, "%s:%d malloc failed\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    item->value = value;
    item->next = NULL;
    if (list->tail) // If there is at least one item in the list ...
    {
        // ... add the list to the tail and move the tail.
        list->tail->next = item;
        list->tail = item;
    }
    else // If the list was empty ...
    {
        list->head = list->tail = item; // ... the new element becomes both head and tail.
    }
    return value; // Return the value for convenience and to indicate there was no error.
}

void *list_delete(List *list, void *value)
{
    ListItem *current = list->head;
    ListItem *previous = NULL;
    while (current) // While there are more items in the list ...
    {
        if (current->value == value) // If we found the item ...
        {
            if (previous) // If we're not at the first item of the list ...
            {
                previous->next = current->next;
            }
            else // We *are* at the first item,so move the list head.
            {
                list->head = current->next;
            }
            if (current == list->tail) // If we've just removed the tail ...
            {
                list->tail = previous; // ... move the tail back.
            }
            free(current); // Free the memory allocated to the removed item.
            return value; // Return the value of the removed item for convenience.
        }
        previous = current; // Remember the previous item.
        current = current->next; // Move on to next item.
    }
    return value; // We didn't find the item, so we return its value as if it was removed.
}

void list_free(List *list)
{
    ListItem *current = list->head;
    while (current)
    {
        current = current->next;
        free(current);
    }
    free(list);
}
