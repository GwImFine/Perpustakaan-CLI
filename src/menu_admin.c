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

static void admin_edit_book(App *app)
{
    char id[LEN_OCAID + 64];
    char buf[LEN_TITLE + 64];
    Book *b;
    Book backup;
    int v;

    ui_title("UBAH DATA BUKU");
    util_read_line("ID buku yang akan diubah : ", id, sizeof(id));
    if (util_eof()) {
        return;
    }
    b = booklist_find(&app->books, id);
    if (b == NULL) {
        puts("  ! Buku dengan ID tersebut tidak ditemukan.");
        return;
    }
    backup = *b;
    puts("\nData saat ini:");
    ui_show_book_detail(b);
    puts("\n(Tekan Enter untuk mempertahankan nilai lama)");

    util_read_line("Judul baru     : ", buf, sizeof(buf));
    if (buf[0] != '\0') {
        util_sanitize_field(buf);
        if (strlen(buf) < sizeof(b->title)) {
            util_copy(b->title, sizeof(b->title), buf);
        } else {
            puts("  ! Judul terlalu panjang, tidak diubah.");
        }
    }
    util_read_line("Pengarang baru : ", buf, sizeof(buf));
    if (buf[0] != '\0') {
        util_sanitize_field(buf);
        if (strlen(buf) < sizeof(b->author)) {
            util_copy(b->author, sizeof(b->author), buf);
        } else {
            puts("  ! Pengarang terlalu panjang, tidak diubah.");
        }
    }
    util_read_line("Tahun baru     : ", buf, sizeof(buf));
    if (buf[0] != '\0') {
        if (util_parse_int(buf, 1, 2100, &v)) {
            b->year = v;
        } else {
            puts("  ! Tahun tidak valid, tidak diubah.");
        }
    }
    util_read_line("Halaman baru   : ", buf, sizeof(buf));
    if (buf[0] != '\0') {
        if (util_parse_int(buf, 1, 100000, &v)) {
            b->page_count = v;
        } else {
            puts("  ! Jumlah halaman tidak valid, tidak diubah.");
        }
    }
    util_read_line("Ubah stok (mis. +2 atau -1) : ", buf, sizeof(buf));
    if (buf[0] != '\0') {
        if (util_parse_int(buf, -100000, 100000, &v) && b->stock + v >= 0) {
            b->stock += v;
        } else {
            puts("  ! Perubahan stok tidak valid (stok tidak boleh negatif), tidak diubah.");
        }
    }
    if (util_eof()) {
        *b = backup;
        return;
    }
    if (book_catalog_save(&app->books) != 0) {
        *b = backup;
        puts("  ! Gagal menyimpan ke berkas. Perubahan dibatalkan.");
        return;
    }
    puts("\nData buku berhasil diperbarui:");
    ui_show_book_detail(b);
}

static void admin_delete_book(App *app)
{
    char id[LEN_OCAID + 64];
    Book *b;
    Book backup;
    size_t active;

    ui_title("HAPUS BUKU");
    util_read_line("ID buku yang akan dihapus : ", id, sizeof(id));
    if (util_eof()) {
        return;
    }
    b = booklist_find(&app->books, id);
    if (b == NULL) {
        puts("  ! Buku dengan ID tersebut tidak ditemukan.");
        return;
    }
    active = loans_count_active_for_book(&app->loans, id);
    if (active > 0) {
        printf("  ! Buku tidak dapat dihapus: masih ada %zu peminjaman aktif.\n", active);
        return;
    }
    ui_show_book_detail(b);
    if (!util_prompt_yes_no("\nYakin menghapus buku ini? (y/n) : ")) {
        puts("Penghapusan dibatalkan.");
        return;
    }
    backup = *b;
    booklist_remove(&app->books, id);
    if (book_catalog_save(&app->books) != 0) {
        booklist_add(&app->books, &backup);
        puts("  ! Gagal menyimpan ke berkas. Penghapusan dibatalkan.");
        return;
    }
    puts("Buku berhasil dihapus.");
}