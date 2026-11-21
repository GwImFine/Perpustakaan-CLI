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