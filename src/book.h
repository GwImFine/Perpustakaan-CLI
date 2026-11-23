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
} BookList;

typedef struct {
    int total_lines;
    int imported;
    int skipped_invalid;
    int skipped_duplicate;
} ImportReport;

void booklist_init(BookList *bl);
void booklist_free(BookList *bl);
Book *booklist_find(const BookList *bl, const char *ocaid);
int booklist_add(BookList *bl, const Book *b);      /* 0 ok, -1 memori, -2 duplikat */
int booklist_remove(BookList *bl, const char *ocaid); /* 0 ok, -1 tidak ditemukan */

/* Dataset asli (read-only) -> memori */
int book_import_master(BookList *bl, ImportReport *report);
int book_parse_master_line(const char *line, Book *out);

/* Inventaris operasional (dataset + stok) */
int book_catalog_load(BookList *bl);                /* -1 jika berkas belum ada */
int book_catalog_save(const BookList *bl);

#endif