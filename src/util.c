#include "util.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int g_eof = 0;

int util_eof(void)
{
    return g_eof;
}

void util_trim(char *s)
{
    size_t len = strlen(s);
    size_t start = 0;

    while (s[start] != '\0' && isspace((unsigned char)s[start])) {
        start++;
    }
    while (len > start && isspace((unsigned char)s[len - 1])) {
        len--;
    }
    memmove(s, s + start, len - start);
    s[len - start] = '\0';
}

void util_copy(char *dst, size_t size, const char *src)
{
    size_t n = strlen(src);

    if (n >= size) {
        n = size - 1;
    }
    memcpy(dst, src, n);
    dst[n] = '\0';
}

oid util_read_line(const char *prompt, char *buf, size_t size)
{
    size_t len;

    if (prompt != NULL) {
        fputs(prompt, stdout);
        fflush(stdout);
    }
    if (fgets(buf, (int)size, stdin) == NULL) {
        buf[0] = '\0';
        g_eof = 1;
        return;
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        int c; /* baris terlalu panjang: buang sisanya */
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }
    util_trim(buf);
}