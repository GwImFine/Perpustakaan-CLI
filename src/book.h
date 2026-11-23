#ifndef BOOK_H
#define BOOK_H

#include <stddef.h>
#include "config.h"

typedef struct {
    char ocaid[LEN_OCAID];
    char title[LEN_TITLE];
    char author[LEN_AUTHOR];
    int year;
    int page_count;
    char edition[LEN_OLID];     /* openlibrary_edition */
    char work[LEN_OLID];        /* openlibrary_work */
    char url[LEN_URL];          /* full_text_url */
    int stock;
} Book;

typedef struct {
    Book *items;
    size_t count;
    size_t capacity;
} Booklist;

typedef struct {
    int total_lines;
    int imported;
    int skipped_invalid;
    int skipped_duplicates;
} ImportStats;

void booklist_init(Booklist *bl);
void booklist_free(Booklist *bl);
Book *booklist_find(const Booklist *bl, const char *ocaid);
int booklist_add(Booklist *bl, const Book *b);      /* 0 ok, -1 memori, -2 duplikat */
int booklist_remove(Booklist *bl, const char *ocaid); /* 0 ok, -1 tidak ditemukan */

/* Dataset asli (read-only) -> memori */
int book_import_master(Booklist *bl, ImportReport *report);
int book_parse_master_line(const char *line, Book *out);

/* Inventaris operasional (dataset + stok) */
int book_catalog_load(Booklist *bl);                /* -1 jika berkas belum ada */
int book_catalog_save(const Booklist *bl);

#endif