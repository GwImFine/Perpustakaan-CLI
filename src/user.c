
#include "user.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "config.h"

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

int users_save(const UserList *ul)
{
    char tmp[256];
    FILE *f;
    size_t i;

    snprintf(tmp, sizeof(tmp), "%s.tmp", FILE_USERS);
    f = fopen(tmp, "w");

    if (f == NULL) {
        return -1;
    }

    for (i = 0; i < ul->count; i++) {
        fprintf(f, "%s %s %s\n",
                ul->items[i].username,
                ul->items[i].password,
                ul->items[i].role);
    }

    if (fclose(f) != 0) {
        remove(tmp);
        return -1;
    }

    return util_replace_file(tmp, FILE_USERS);
}

static void add_default(UserList *ul,
                       const char *name,
                       const char *pass,
                       const char *role)
{
    User u;

    util_copy(u.username, sizeof(u.username), name);
    util_copy(u.password, sizeof(u.password), pass);
    util_copy(u.role, sizeof(u.role), role);

    users_add(ul, &u);
}

int users_load(UserList *ul)
{
    FILE *f = fopen(FILE_USERS, "r");
    char line[256];
    User u;

    if (f == NULL) {
        add_default(ul, "admin", "admin123", ROLE_ADMIN);
        add_default(ul, "user1", "user123", ROLE_USER);
        add_default(ul, "user2", "user123", ROLE_USER);

        return users_save(ul);
    }

    while (fgets(line, sizeof(line), f) != NULL) {
        if (sscanf(line, "%31s %31s %11s",
                    u.username, u.password, u.role) == 3) {
            if (users_add(ul, &u) == -1) {
                fclose(f);
                return -1;
            }
        }
    }

    fclose(f);
    return 0;
}

int users_authenticate(const UserList *ul, const char *username,
                       const char *password, User *out)
{
    const User *u = users_find(ul, username);

    if (u == NULL || strcmp(u->password, password) != 0) {
        return 0;
    }

    *out = *u;
    return 1;
}

int user_is_admin(const User *u)
{
    return strcmp(u->role, ROLE_ADMIN) == 0;
}