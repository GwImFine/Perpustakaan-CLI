#include "menu_admin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"
#include "util.h"

static void admin_add_book(App *app)
{
    Book b;
    int rc;

    ui_title("TAMBAH BUKU BARU");
    memset(&b, 0, sizeof(b));
    for (;;) {
        util_prompt_text("ID buku (ocaid, tanpa spasi) : ", b.ocaid, sizeof(b.ocaid));
        if (util_eof()) {
            return;
        }
        if (!util_is_token(b.ocaid)) {
            puts("  ! ID tidak boleh mengandung spasi atau karakter khusus.");
        } else if (booklist_find(&app->books, b.ocaid) != NULL) {
            puts("  ! ID sudah terdaftar di katalog.");
        } else {
            break;
        }
    }
    util_prompt_text("Judul                         : ", b.title, sizeof(b.title));
    util_prompt_text("Pengarang                     : ", b.author, sizeof(b.author));
    b.year = util_prompt_int("Tahun terbit (1-2100)        : ", 1, 2100);
    b.page_count = util_prompt_int("Jumlah halaman                : ", 1, 100000);
    b.stock = util_prompt_int("Stok awal                     : ", 0, 100000);
    if (util_eof()) {
        return;
    }
    util_copy(b.edition, sizeof(b.edition), "-");
    util_copy(b.work, sizeof(b.work), "-");
    util_copy(b.url, sizeof(b.url), "-");

    rc = booklist_add(&app->books, &b);
    if (rc != 0) {
        puts("  ! Gagal menambahkan buku.");
        return;
    }
    if (book_catalog_save(&app->books) != 0) {
        booklist_remove(&app->books, b.ocaid);
        puts("  ! Gagal menyimpan ke berkas. Perubahan dibatalkan.");
        return;
    }
    puts("\nBuku berhasil ditambahkan dan disimpan.");
}