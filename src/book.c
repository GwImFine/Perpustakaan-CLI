#include "book.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void booklist_init(BookList *bl)
{
    bl->items = NULL;
    bl->count = 0;
    bl->capacity = 0;
}

void booklist_free(BookList *bl)
{
    free(bl->items);
    booklist_init(bl);
}

static int booklist_reserve(BookList *bl, size_t need)
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

Book *booklist_find(const BookList *bl, const char *ocaid)
{
    int i;

    for (i = 0; i < bl->count; i++) {
        if (strcmp(bl->items[i].ocaid, ocaid) == 0) {
            return &bl->items[i];
        }
    }
    return NULL;
}

int booklist_add(BookList *bl, const Book *b)
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

int booklist_remove(BookList *bl, const char *ocaid)
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

static int read_text_field(const char **pp, char *dst, size_t size)
{
    const char *p = *pp;
    size_t n = 0;

    while (*p != '\0' && isspace((unsigned char)*p)) {
        p++;
    }
    if (*p == '\0') {
        return 0;
    }
    if (*p == '"') {
        p++;
        for (;;) {
            if (*p == '\0') {
                return 0; /* tanda petik penutup tidak ada */
            }
            if (*p == '"') {
                if (p[1] == '"') {
                    if (n + 1 < size) {
                        dst[n++] = '"';
                    }
                    p += 2;
                    continue;
                }
                p++;
                break;
            }
            if (n + 1< size) {
                dst[n++] = *p;
            }
            p++;
        }
    } else {
        while (*p != '\0' && !isspace((unsigned char)*p)) {
            if (n + 1 < size) {
                dst[n++] = *p;
            }
            p++;
        }
    }
    dst[n] = '\0';
    *pp = p;
    return 1;
}

int book_parse_master_line(const char *line, Book *out)
{
    const char *p = line;
    Book b;

    memset(&b, 0, sizeof(b));
    if (!read_text_field(&p, b.ocaid, sizeof(b.ocaid)) ||
        !read_text_field(&p, b.title, sizeof(b.title)) ||
        !read_text_field(&p, b.author, sizeof(b.author))) {
        return 0;
    }
    if (sscanf(p, "%d %d %31s %31s %127s",
           &b.year, &b.page_count,
           b.edition, b.work, b.url) != 5) {
        return 0;
    }
    if (b.year <= 0 || b.page_count <= 0 || !util_is_token(b.ocaid)) {
        return 0;
    }
    util_sanitize_field(b.title);
    util_sanitize_field(b.author);
    b.stock = DEFAULT_STOCK;
    *out = b;
    return 1;
}

int book_import_master(BookList *bl, ImportReport *report)
{
    FILE *f = fopen(FILE_BOOKS_MASTER, "r");
    char line[LINE_BUFFER];
    Book b;
    int first = 1;
    int rc;

    memset(report, 0, sizeof(*report));
    if (f == NULL) {
        return -1;
    }
    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);

        while (len > 0 && (line[len -1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }
        if (first) {            /* baris header */
            first = 0;
            continue;
        }
        if (len == 0) {
            continue;
        }
        report->total_lines++;
        if (!book_parse_master_line(line, &b)) {
            report->skipped_invalid++;
            continue;
        }
        rc = booklist_add(bl, &b);
        if (rc == 0) {
            report->imported++;
        } else if (rc == -2) {
            report->skipped_duplicate++;
        } else {
            fclose(f);
            return -1;
        }
    }
    fclose(f);
    return 0;
}

int book_catalog_save(const BookList *bl)
{
    char tmp[256];
    FILE *f;
    size_t i;

    snprintf(tmp, sizeof(tmp), "%s.tmp", FILE_CATALOG);
    f = fopen(tmp, "w");
    if (f == NULL) {
        return -1;
    }
    for (i = 0; i < bl->count; i++) {
        const Book *b = &bl->items[i];
        fprintf(f, "%s|%s|%s|%d|%d|%s|%s|%s|%d\n", b->ocaid, b->title, b->author,
                b->year, b->page_count, b->edition, b->work, b->url, b->stock);
    }
    if (fclose(f) != 0) {
        remove(tmp);
        return -1;
    }
    return util_replace_file(tmp, FILE_CATALOG);
}

static int split_fields(char *line, char **fields, int max)
{
    int n = 0;
    char *p = line;

    fields[n++] = p;
    while (*p != '\0' && n < max) {
        if (*p == '|') {
            *p = '\0';
            fields[n++] = p + 1;
        }
        p++;
    }
    return n;
}

static int parse_catalog_line(char *line, Book *out)
{
    char *f[9];
    Book b;
    int year, pages, stock;

    if (split_fields(line, f, 9) != 9) {
        return 0;
    }
    if (!util_parse_int(f[3], 1, 9999, &year) ||
        !util_parse_int(f[4], 1, 1000000, &pages) ||
        !util_parse_int(f[8], 0, 1000000, &stock) || !util_is_token(f[0])) {
        return 0;
    }
    memset(&b, 0, sizeof(b));
    util_copy(b.ocaid, sizeof(b.ocaid), f[0]);
    util_copy(b.title, sizeof(b.title), f[1]);
    util_copy(b.author, sizeof(b.author), f[2]);
    b.year = year;
    b.page_count = pages;
    util_copy(b.edition, sizeof(b.edition), f[5]);
    util_copy(b.work, sizeof(b.work), f[6]);
    util_copy(b.url, sizeof(b.url), f[7]);
    b.stock = stock;
    *out = b;
    return 1;
}

int book_catalog_load(BookList *bl)
{
    FILE *f = fopen(FILE_CATALOG, "r");
    char line[LINE_BUFFER];
    Book b;

    if (f == NULL) {
        return -1;
    }
    while (fgets(line, sizeof(line), f) != NULL) {
        size_t len = strlen(line);

        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }
        if (len == 0) {
            continue;
        }
        if (parse_catalog_line(line, &b)) {
            if (booklist_add(bl, &b) == -1) {
                fclose(f);
                return -1;
            }
        }
    }
    fclose(f);
    return 0;
}