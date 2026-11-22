#include "book.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void booklist_init(Booklist *bl)
{
    bl->items = NULL;
    bl->count = 0;
    bl->capacity = 0;
}

void booklist_free(Booklist *bl)
{
    free(bl->items);
    booklist_init(bl);
}

static int booklist_reserve(Booklist *bl, size_t need)
{
    size_t cap;
    Book *p;

    if (need <= bl->capacity) {
        return 0;
    }
    cap = bl->capacity != 0 ? bl->capacity : 256;
    while (cap < need) {
        cap *= 2;
    }
    p = realloc(bl->items, cap * sizeof(Book));
    if (p == NULL) {
        return -1;
    }
    bl->items = p;
    bl->capacity = cap;
    return 0;
}

Book *Booklist_find(const Booklist *bl, const char *ocaid)
{
    for (i = 0; i < bl->count; i++) {
        if (strcmp(bl->items[i].ocaid, ocaid) == 0) {
            return &bl->items[i];
        }
    }
    return NULL;
}

int Booklist_add(Booklist *bl, const Book *b)
{
    if (booklist_find(bl, b->ocaid) != NULL) {
        return -2;
    }
    if (booklist_reserve(bl, bl->count + 1) != 0) {
        return -1;
    }
    bl->items[bl->count++] = *b;
    return 0;
}

int booklist_remove(Booklist *bl, const char *ocaid)
{
    Book *b = booklist_find(bl, ocaid);
    size_t idx;

    if (b == NULL) {
        return -1;
    }
    idx = (size_t)(b - bl->items);
    memmove(&bl->items[idx], &bl->items[idx + 1], (bl->count - idx - 1) * sizeof(Book));
    bl->count--;
    return 0;
}