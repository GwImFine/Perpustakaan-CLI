
#include "user.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void userlist_init(UserList *ul)
{
    ul->items = NULL;
    ul->count = 0;
    ul->capacity = 0;
}

void userlist_free(UserList *ul)
{
    free(ul->items);
    userlist_init(ul);
}

User *users_find(const UserList *ul, const char *username)
{
    size_t i;

    for (i = 0; i < ul->count; i++) {
        if (strcmp(ul->items[i].username, username) == 0) {
            return &ul->items[i];
        }
    }
    return NULL;
}

int users_add(UserList *ul, const User *u)
{
    if (users_find(ul, u->username) != NULL) {
        return -2;
    }
    if (ul->count == ul->capacity) {
        size_t cap = ul->capacity != 0 ? ul->capacity * 2 : 8;
        User *p = realloc(ul->items, cap * sizeof(User));

        if (p == NULL) {
            return -1;
        }
        ul->items = p;
        ul->capacity = cap;
    }
    ul->items[ul->count++] = *u;
    return 0;
}