#include "loan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void loanlist_init(LoanList *ll)
{
    ll->items = NULL;
    ll->count = 0;
    ll->capacity = 0;
}

void loanlist_free(LoanList *ll)
{
    free(ll->items);
    loanlist_init(ll);
}

static int loanlist_add(LoanList *ll, const Loan *l)
{
    if (ll->count == ll->capacity){
        size_t cap = ll->capacity != 0 ? ll->capacity * 2 : 64;
        Loan *p = realloc(ll->items, cap * sizeof(Loan));

        if (p == NULL){
            return -1;
        }

        ll->items = p;
        ll->capacity = cap;
    }

    ll->items[ll->count++] = *l;
    return 0;
}

static unsigned int loans_next_id(const LoanList *ll)
{
    unsigned int max = 0;
    size_t i;

    for (i = 0; i < ll->count; i++){
        if (ll->items[i].id > max){
            max = ll->items[i].id;
        }
    }

    return max + 1;
}

int loans_save(const LoanList *ll)
{
    char tmp[256];
    FILE *f;
    size_t i;

    snprintf(tmp, sizeof(tmp), "%s.tmp", FILE_LOANS);
    f = fopen(tmp, "w");

    if (f == NULL){
        return -1;
    }

    for (i = 0; i < ll->count; i++){
        const Loan *l = &ll->items[i];

        fprintf(f, "%u %s %s %s %s\n",
                l->id, l->username, l->ocaid,
                l->tanggal, l->status);
    }

    if (fclose(f) != 0){
        remove(tmp);
        return -1;
    }

    return util_replace_file(tmp, FILE_LOANS);
}

int loans_load(LoanList *ll)
{
    FILE *f = fopen(FILE_LOANS, "r");
    char line[512];
    Loan l;

    if (f == NULL){
        return loans_save(ll);
    }

    while (fgets(line, sizeof(line), f) != NULL){
        if (sscanf(line, "%u %31s %63s %19s %19s",
                    &l.id, l.username, l.ocaid,
                    l.tanggal, l.status) == 5){

            if (loanlist_add(ll, &l) != 0){
                fclose(f);
                return -1;
            }
        }
    }

    fclose(f);
    return 0;
}

Loan *loans_find_active(const LoanList *ll, const char *username, const char *ocaid)
{
    size_t i;

    for (i = 0; i < ll->count; i++){
        Loan *l = &ll->items[i];

        if (strcmp(l->status, STATUS_BORROWED) == 0 &&
            strcmp(l->username, username) == 0 &&
            strcmp(l->ocaid, ocaid) == 0) {
            return l;
        }
    }

    return NULL;
}

size_t loans_count_active_for_book(const LoanList *ll, const char *ocaid)
{
    size_t i, n = 0;

    for (i = 0; i < ll->count; i++){
        if (strcmp(ll->items[i].status, STATUS_BORROWED) == 0 &&
            strcmp(ll->items[i].ocaid, ocaid) == 0){
            n++;
        }
    }

    return n;
}

LoanResult loan_borrow(LoanList *ll, BookList *bl,
                        const char *username, const char *ocaid)
{
    Book *b = booklist_find(bl, ocaid);
    Loan l;

    if (b == NULL){
        return LOAN_NO_BOOK;
    }

    if (b->stock <= 0){
        return LOAN_NO_STOCK;
    }

    if (loans_find_active(ll, username, ocaid) != NULL){
        return LOAN_ALREADY;
    }

    l.id = loans_next_id(ll);
    util_copy(l.username, sizeof(l.username), username);
    util_copy(l.ocaid, sizeof(l.ocaid), ocaid);
    util_today(l.tanggal, sizeof(l.tanggal));
    util_copy(l.status, sizeof(l.status), STATUS_BORROWED);

    if (loanlist_add(ll, &l) != 0){
        return LOAN_ERR_MEM;
    }

    b->stock--;

    if (book_catalog_save(bl) != 0 || loans_save(ll) != 0){
        b->stock++;
        ll->count--;
        return LOAN_ERR_IO;
    }

    return LOAN_OK;
}

LoanResult loan_return(LoanList *ll, BookList *bl,
                       const char *username, const char *ocaid)
{
    Loan *l = loans_find_active(ll, username, ocaid);
    Book *b = booklist_find(bl, ocaid);

    if (l == NULL || b == NULL){
        return LOAN_NOT_FOUND;
    }

    util_copy(l->status, sizeof(l->status), STATUS_RETURNED);
    b->stock++;

    if (book_catalog_save(bl) != 0 || loans_save(ll) != 0){
        b->stock--;
        util_copy(l->status, sizeof(l->status), STATUS_BORROWED);
        return LOAN_ERR_IO;
    }

    return LOAN_OK;
}