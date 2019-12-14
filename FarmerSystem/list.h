#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#define LIST_FOREACH(list, value_type) for (ListItem *item = list->head, value_type *each = item ? item->value : NULL; item; item = item->next, each = item ? item->value : NULL)

typedef struct ListItem {
    void *value;
    struct ListItem *next;
} ListItem;

typedef struct List {
        ListItem *head;
        ListItem *tail;
} List;

List *new_list();
void list_free(List *list);

void *list_add(List *list, void *value);
void *list_delete(List *list, void *value);

int list_length(List *);

/* Execute the function f for each item in the list. f must take each value as its single parameter. */
void for_each(List *list, void (*f)(void *));

#endif // LIST_H_INCLUDED
