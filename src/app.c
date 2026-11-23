#include "app.h"

#include <stdio.h>

void app_free(App *app)
{
    booklist_free(&app->books);
    userlist_free(&app->users);
    loanlist_free(&app->loans);
}

int app_init(App *app)
{
    ImportReport rep;

    booklist_init(&app->books);
    userlist_init(&app->users);
    loanlist_init(&app->loans);

    if (book_catalog_load(&app->books) != 0) {
        /* Pertama kali dijalankan: impor dataset asli (read-only) + stok awal */
        printf("Menyiapkan inventaris dari %s ...\n", FILE_BOOKS_MASTER);
        if (book_import_master(&app->books, &rep) != 0) {
            fprintf(stderr, "Gagal membuka dataset '%s'.\n", FILE_BOOKS_MASTER);
            return -1;
        }
        printf("  Baris data      : %d\n", rep.total_lines);
        printf("  Berhasil diimpor: %d buku (stok awal %d/judul)\n", rep.imported, DEFAULT_STOCK);
        if (rep.skipped_duplicate > 0) {
            printf("  Dilewati (ID ganda): %d\n", rep.skipped_duplicate);
        }
        if (rep.skipped_invalid > 0) {
            printf("  Dilewati (format tidak valid): %d\n", rep.skipped_invalid);
        }
        if (book_catalog_save(&app->books) != 0) {
            fprintf(stderr, "Gagal menulis '%s'.\n", FILE_CATALOG);
            return -1;
        }
    }
    if (users_load(&app->users) != 0) {
        fprintf(stderr, "Gagal memuat/membuat '%s'.\n", FILE_USERS);
        return -1;
    }
    if (loans_load(&app->loans) != 0) {
        fprintf(stderr, "Gagal memuat/membuat '%s'.\n", FILE_LOANS);
        return -1;
    }
    return 0;
}