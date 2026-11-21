#ifndef UI_H
#define UI_H

#include <stddef.h>
#include "book.h"
#include "loan.h"
#include "sorting.h"

void ui_title(const char *text);
void ui_show_books(Book **list, size_t n, const char *title);
void ui_show_loans(const BookList *bl, Loan **list, size_t n, int show_user, const char *title);
void ui_show_book_detail(const Book *b);

/* 1 = urutkan (hasil di parameter), 0 = tanpa urutan, -1 = batal */
int  ui_choose_sort(SortAlgo *algo, SortKey *key, SortOrder *order);

void ui_browse_catalog(const BookList *bl, int only_available);
void ui_search_books(const BookList *bl);

#endif