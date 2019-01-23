#ifndef __LIST_H__
#define __LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list_head {
    struct list_head *next, *prev;
} ;


#define LIST_HEAD_INIT(name)            { &(name), &(name) }
#define LIST_HEAD(name)                 struct list_head name = LIST_HEAD_INIT(name)
#define list_entry(ptr, type, member)   container_of(ptr, type, member)

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t)& ((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member)                                         \
    (type*)((char *)ptr - offsetof(type,member))

#define list_for_each(pos, head)                                                \
    for (pos = (head)->next; pos != (head);                                     \
        pos = pos->next)

#define list_for_each_safe(pos, n, head)                                        \
    for (pos = (head)->next, n = pos->next; pos != (head);                      \
        pos = n, n = pos->next)



static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

static inline void __list_add(struct list_head *l_new,
                  struct list_head *prev,
                  struct list_head *next)
{
    next->prev = l_new;
    l_new->next = next;
    l_new->prev = prev;
    prev->next = l_new;
}

static inline void list_add(struct list_head *l_new, struct list_head *head)
{
    __list_add(l_new, head, head->next);
}

static inline void list_add_tail(struct list_head *l_new, struct list_head *head)
{
    __list_add(l_new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

static inline void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

static inline int list_is_last(const struct list_head *list,
                const struct list_head *head)
{
    return list->next == head;
}

static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}

#endif /* __LIST_H__ */
