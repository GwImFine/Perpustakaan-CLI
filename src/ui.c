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