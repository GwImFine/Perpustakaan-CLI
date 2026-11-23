#ifndef LOAN_H
#define LOAN_H

#include <stddef.h>
#include "book.h"
#include "config.h"

typedef struct{
    unsigned int id;
    char username[LEN_USERNAME];
    char ocaid[LEN_OCAID];
    char tanggal[LEN_DATE];
    char status[LEN_STATUS];
} Loan;

typedef struct{
    Loan *items;
    size_t count;
    size_t capacity;
} LoanList;

typedef enum{
    LOAN_OK = 0,
    LOAN_NO_BOOK,
    LOAN_NO_STOCK,
    LOAN_ALREADY,
    LOAN_NOT_FOUND,
    LOAN_ERR_MEM,
    LOAN_ERR_IO
} LoanResult;

void loanlist_init(LoanList *ll);
void loanlist_free(LoanList *ll);
int loans_load(LoanList *ll);
int loans_save(const LoanList *ll);

Loan *loans_find_active(const LoanList *ll,
                        const char *username,
                        const char *ocaid);

size_t loans_count_active_for_book(const LoanList *ll,
                                   const char *ocaid);

LoanResult loan_borrow(LoanList *ll, BookList *bl,
                       const char *username, const char *ocaid);

LoanResult loan_return(LoanList *ll, BookList *bl,
                       const char *username, const char *ocaid);

#endif