#include "sorting.h"

#include "util.h"

static int compare_key(const Book *a, const Book *b, SortKey key)
{
    switch (key) {
    case KEY_ID:    return util_icmp(a->ocaid, b->ocaid);
    case KEY_TITLE: return util_icmp(a->title, b->title);
    case KEY_YEAR:  return (a->year > b->year) - (a->year < b->year);
    case KEY_PAGES: return (a->page_count > b->page_count) - (a->page_count < b->page_count);
    case KEY_STOCK: return (a->stock > b->stock) - (a->stock < b->stock);
    }
    return 0;
}

static int out_of_order(const Book *a, const Book *b, SortKey key, SortOrder order)
{
    int c = compare_key(a, b, key);

    return order == ORDER_ASC ? c > 0 : c < 0;
}

static void swap_ptr(Book **a, Book **b)
{
    Book *t = *a;

    *a = *b;
    *b = t;
}

static void bubble_sort(Book **arr, size_t n, SortKey key, SortOrder order)
{
    size_t i, j;

    for (i = 0; i + 1 < n; i++) {
        int swapped = 0;

        for (j = 0; j + 1 < n - i; j++) {
            if (out_of_order(arr[j], arr[j + 1], key, order)) {
                swap_ptr(&arr[j], &arr[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

static void insertion_sort(Book **arr, size_t n, SortKey key, SortOrder order)
{
    size_t i, j;

    for (i = 1; i < n; i++) {
        Book *cur = arr[i];

        j = i;
        while (j > 0 && out_of_order(arr[j - 1], cur, key, order)) {
            arr[j] = arr[j - 1];
            j--;
        }
        arr[j] = cur;
    }
}

static void selection_sort(Book **arr, size_t n, SortKey key, SortOrder order)
{
    size_t i, j, best;

    for (i = 0; i + 1 < n; i++) {
        best = i;
        for (j = i + 1; j < n; j++) {
            if (out_of_order(arr[best], arr[j], key, order)) {
                best = j;
            }
        }
        if (best != i) {
            swap_ptr(&arr[i], &arr[best]);
        }
    }
}

void sort_books(Book **arr, size_t n, SortAlgo algo, SortKey key, SortOrder order)
{
    switch (algo) {
    case SORT_BUBBLE:    bubble_sort(arr, n, key, order);    break;
    case SORT_INSERTION: insertion_sort(arr, n, key, order); break;
    case SORT_SELECTION: selection_sort(arr, n, key, order); break;
    }
}

const char *sort_algo_name(SortAlgo algo)
{
    switch (algo) {
    case SORT_BUBBLE:    return "Bubble Sort";
    case SORT_INSERTION: return "Insertion Sort";
    case SORT_SELECTION: return "Selection Sort";
    }
    return "?";
}

const char *sort_key_name(SortKey key)
{
    switch (key) {
    case KEY_ID:    return "ID Buku (ocaid)";
    case KEY_TITLE: return "Judul";
    case KEY_YEAR:  return "Tahun Terbit";
    case KEY_PAGES: return "Jumlah Halaman";
    case KEY_STOCK: return "Stok";
    }
    return "?";
}

const char *sort_order_label(SortKey key, SortOrder order)
{
    int asc = (order == ORDER_ASC);

    switch (key) {
    case KEY_ID:    return asc ? "Ascending" : "Descending";
    case KEY_TITLE: return asc ? "A-Z" : "Z-A";
    case KEY_YEAR:  return asc ? "Terlama ke Terbaru" : "Terbaru ke Terlama";
    case KEY_PAGES: return asc ? "Tipis ke Tebal" : "Tebal ke Tipis";
    case KEY_STOCK: return asc ? "Sedikit ke Banyak" : "Banyak ke Sedikit";
    }
    return "?";
}