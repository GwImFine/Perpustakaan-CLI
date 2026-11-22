#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

int  util_eof(void);                                   /* stdin sudah habis?      */
void util_trim(char *s);                               /* buang spasi kiri/kanan  */
void util_read_line(const char *prompt, char *buf, size_t size);
int  util_parse_int(const char *s, int min, int max, int *out);
int  util_prompt_int(const char *prompt, int min, int max);
void util_prompt_text(const char *prompt, char *buf, size_t size);
int  util_prompt_yes_no(const char *prompt);
void util_pause(void);
void util_sanitize_field(char *s);                     /* '|' -> '/', kontrol -> ' ' */
int  util_is_token(const char *s);                     /* tanpa spasi & '|'        */
int  util_icmp(const char *a, const char *b);          /* banding tanpa case       */
int  util_icontains(const char *hay, const char *needle);
void util_copy(char *dst, size_t size, const char *src);
void util_today(char *buf, size_t size);               /* DD-MM-YYYY               */
int  util_replace_file(const char *tmp, const char *dest);

#endif