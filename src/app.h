#ifndef APP_H
#define APP_H

#include "book.h"
#include "loan.h"
#include "user.h"

typedef struct {
    BookList books;
    UserList users;
    LoanList loans;
} App;

int  app_init(App *app);   /* muat/ bootstrap semua berkas data; 0 = sukses */
void app_free(App *app);   /* bebaskan seluruh memori dinamis               */

#endif