#include "ui.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "config.h"
#include "util.h"

/* ---------- pencetakan sel tabel (aman untuk UTF-8) ---------- */

static int utf8_cols(const char *s)
{
    int n = 0;

    for (; *s != '\0'; s++) {
        if (((unsigned char)*s & 0xC0) != 0x80) {
            n++;
        }
    }
    return n;
}

static void put_cell(const char *s, int width)
{
    int total = utf8_cols(s);
    int keep = total <= width ? total : width - 1;
    int cols = 0;
    int printed;

    for (; *s != '\0'; s++) {
        if (((unsigned char)*s & 0xC0) != 0x80) {
            if (cols == keep) {
                break;
            }
            cols++;
        }
        putchar(*s);
    }
    printed = keep;
    if (total > width) {
        putchar('~');
        printed++;
    }
    for (; printed < width; printed++) {
        putchar(' ');
    }
}

static void put_line(int width)
{
    int i;

    for (i = 0; i < width; i++) {
        putchar('-');
    }
    putchar('\n');
}

void ui_title(const char *text)
{
    printf("\n============================================================\n");
    printf(" %s\n", text);
    printf("============================================================\n");
}

static int ask_page(size_t *page, size_t pages)
{
    char buf[32];
    int v;

    util_read_line("[N] Berikutnya  [P] Sebelumnya  [nomor] Lompat  [Q] Selesai : ", buf, sizeof(buf));
    if (util_eof() || util_icmp(buf, "q") == 0 || buf[0] == '\0') {
        return 0;
    }
    if (util_icmp(buf, "n") == 0) {
        if (*page + 1 < pages) {
            (*page)++;
        } else {
            puts("  (sudah di halaman terakhir)");
        }
    } else if (util_icmp(buf, "p") == 0) {
        if (*page > 0) {
            (*page)--;
        } else {
            puts("  (sudah di halaman pertama)");
        }
    } else if (util_parse_int(buf, 1, (int)pages, &v)) {
        *page = (size_t)(v - 1);
    } else {
        puts("  ! Perintah tidak dikenal.");
    }
    return 1;
}

void ui_show_books(Book **list, size_t n, const char *title)
{
    size_t pages, page = 0, i, from, to;

    if (n == 0) {
        puts("\n(tidak ada data untuk ditampilkan)");
        return;
    }
    pages = (n + PAGE_SIZE - 1) / PAGE_SIZE;
    do {
        from = page * PAGE_SIZE;
        to = from + PAGE_SIZE < n ? from + PAGE_SIZE : n;
        printf("\n%s\n", title);
        put_line(121);
        put_cell("No", 5);  printf(" | ");
        put_cell("ID Buku (ocaid)", 26); printf(" | ");
        put_cell("Judul", 36); printf(" | ");
        put_cell("Pengarang", 22); printf(" | ");
        put_cell("Tahun", 5); printf(" | ");
        put_cell("Hal.", 5); printf(" | ");
        put_cell("Stok", 4); putchar('\n');
        put_line(121);
        for (i = from; i < to; i++) {
            char num[32], year[16], pg[16], stk[16];

            snprintf(num, sizeof(num), "%zu", i + 1);
            snprintf(year, sizeof(year), "%d", list[i]->year);
            snprintf(pg, sizeof(pg), "%d", list[i]->page_count);
            snprintf(stk, sizeof(stk), "%d", list[i]->stock);
            put_cell(num, 5);  printf(" | ");
            put_cell(list[i]->ocaid, 26); printf(" | ");
            put_cell(list[i]->title, 36); printf(" | ");
            put_cell(list[i]->author, 22); printf(" | ");
            put_cell(year, 5); printf(" | ");
            put_cell(pg, 5); printf(" | ");
            put_cell(stk, 4); putchar('\n');
        }
        put_line(121);
        printf("Halaman %zu/%zu  |  Menampilkan %zu-%zu dari %zu buku\n",
               page + 1, pages, from + 1, to, n);
        if (pages == 1) {
            return;
        }
    } while (ask_page(&page, pages));
}

void ui_show_book_detail(const Book *b)
{
    printf("  ID (ocaid)  : %s\n", b->ocaid);
    printf("  Judul       : %s\n", b->title);
    printf("  Pengarang   : %s\n", b->author);
    printf("  Tahun       : %d\n", b->year);
    printf("  Halaman     : %d\n", b->page_count);
    printf("  Stok        : %d\n", b->stock);
}

void ui_show_loans(const BookList *bl, Loan **list, size_t n, int show_user, const char *title)
{
    size_t pages, page = 0, i, from, to;

    if (n == 0) {
        puts("\n(belum ada data peminjaman)");
        return;
    }
    pages = (n + PAGE_SIZE - 1) / PAGE_SIZE;
    do {
        from = page * PAGE_SIZE;
        to = from + PAGE_SIZE < n ? from + PAGE_SIZE : n;
        printf("\n%s\n", title);
        put_line(112);
        put_cell("ID", 6); printf(" | ");
        put_cell("Peminjam", 16); printf(" | ");
        put_cell("ID Buku", 26); printf(" | ");
        put_cell("Judul", 32); printf(" | ");
        put_cell("Tanggal", 10); printf(" | ");
        put_cell("Status", 8); putchar('\n');
        put_line(112);
        for (i = from; i < to; i++) {
            const Book *b = booklist_find(bl, list[i]->ocaid);
            char id[16];

            snprintf(id, sizeof(id), "%u", list[i]->id);
            put_cell(id, 6); printf(" | ");
            put_cell(show_user ? list[i]->username : "-", 16); printf(" | ");
            put_cell(list[i]->ocaid, 26); printf(" | ");
            put_cell(b != NULL ? b->title : "(buku sudah dihapus)", 32); printf(" | ");
            put_cell(list[i]->tanggal, 10); printf(" | ");
            put_cell(list[i]->status, 8); putchar('\n');
        }
        put_line(112);
        printf("Halaman %zu/%zu  |  Menampilkan %zu-%zu dari %zu transaksi\n",
               page + 1, pages, from + 1, to, n);
        if (pages == 1) {
            return;
        }
    } while (ask_page(&page, pages));
}

int ui_choose_sort(SortAlgo *algo, SortKey *key, SortOrder *order)
{
    int choice;
    int k;

    puts("\nUrutkan katalog?");
    puts("  1. Ya, pilih algoritma & atribut pengurutan");
    puts("  2. Tidak (urutan asli)");
    puts("  0. Batal");
    choice = util_prompt_int("Pilihan: ", 0, 2);
    if (choice == 0) {
        return -1;
    }
    if (choice == 2) {
        return 0;
    }

    puts("\nAlgoritma pengurutan:");
    puts("  1. Bubble Sort");
    puts("  2. Insertion Sort");
    puts("  3. Selection Sort");
    *algo = (SortAlgo)util_prompt_int("Pilihan: ", 1, 3);

    puts("\nUrutkan berdasarkan:");
    puts("  1. ID Buku (ocaid)");
    puts("  2. Judul Buku");
    puts("  3. Tahun Publikasi");
    puts("  4. Ketebalan Halaman (page_count)");
    puts("  5. Stok Ketersediaan");
    k = util_prompt_int("Pilihan: ", 1, 5);
    *key = (SortKey)k;

    puts("\nArah pengurutan:");
    printf("  1. %s\n", sort_order_label(*key, ORDER_ASC));
    printf("  2. %s\n", sort_order_label(*key, ORDER_DESC));
    *order = (SortOrder)util_prompt_int("Pilihan: ", 1, 2);
    return 1;
}

void ui_browse_catalog(const BookList *bl, int only_available)
{
    SortAlgo algo = SORT_BUBBLE;
    SortKey key = KEY_ID;
    SortOrder order = ORDER_ASC;
    Book **view;
    size_t n = 0, i;
    int use_sort;
    char title[200];

    use_sort = ui_choose_sort(&algo, &key, &order);
    if (use_sort < 0 || util_eof()) {
        return;
    }
    view = malloc((bl->count + 1) * sizeof(Book *));
    if (view == NULL) {
        puts("Memori tidak cukup.");
        return;
    }
    for (i = 0; i < bl->count; i++) {
        if (!only_available || bl->items[i].stock > 0) {
            view[n++] = &bl->items[i];
        }
    }
    snprintf(title, sizeof(title), "%s", only_available ? "KATALOG BUKU TERSEDIA (stok > 0)"
                                                         : "KATALOG SELURUH BUKU");
    if (use_sort && n > 1) {
        clock_t t0 = clock();
        double secs;

        printf("\nMengurutkan %zu buku dengan %s ...\n", n, sort_algo_name(algo));
        sort_books(view, n, algo, key, order);
        secs = (double)(clock() - t0) / CLOCKS_PER_SEC;
        printf("Selesai dalam %.3f detik. Urutan: %s (%s)\n", secs,
               sort_key_name(key), sort_order_label(key, order));
    }
    ui_show_books(view, n, title);
    free(view);
}