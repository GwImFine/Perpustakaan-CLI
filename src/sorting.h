#ifndef SORTING_H
#define SORTING_H

#include <stddef.h>
#include "book.h"

typedef enum { SORT_BUBBLE = 1, SORT_INSERTION, SORT_SELECTION } SortAlgo;
typedef enum { KEY_ID = 1, KEY_TITLE, KEY_YEAR, KEY_PAGES, KEY_STOCK } SortKey;
typedef enum { ORDER_ASC = 1, ORDER_DESC } SortOrder;

/* Mengurutkan array POINTER ke Book (struct asli di katalog tidak dipindah). */
void        sort_books(Book **arr, size_t n, SortAlgo algo, SortKey key, SortOrder order);
const char *sort_algo_name(SortAlgo algo);
const char *sort_key_name(SortKey key);
const char *sort_order_label(SortKey key, SortOrder order);

#endif