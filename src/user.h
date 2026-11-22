#ifndef USER_H
#define USER_H

#include <stddef.h>
#include "config.h"

typedef struct {
    char username[LEN_USERNAME];
    char password[LEN_PASSWORD];
    char role[LEN_ROLE];
} User;

typedef struct {
    User *items;
    size_t count;
    size_t capacity;
} UserList;

void userlist_init(UserList *ul);
void userlist_free(UserList *ul);
int users_load(UserList *ul);
int users_save(const UserList *ul);
User *users_find(const UserList *ul, const char *username);
int users_add(UserList *ul, const User *u);
int users_authenticate(const UserList *ul, const char *username,
                      const char *password, User *out);
int user_is_admin(const User *u);

#endif