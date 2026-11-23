#include "menu_user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "util.h"

static size_t user_show_loans(const App *app, const User *session,
                              int active_only, const char *title)
{
    Loan **view;
    size_t i, n = 0;

    view = malloc((app->loans.count + 1) * sizeof(Loan *));
    if (view == NULL) {
        puts("Memori tidak cukup.");
        return 0;
    }

    for (i = app->loans.count; i > 0; i--){
        Loan *l = &app->loans.items[i - 1];

        if (strcmp(l->username, session->username) != 0){
            continue;
        }

        if (active_only && strcmp(l->status, STATUS_BORROWED) != 0){
            continue;
        }

        view[n++] = l;
    }

    if (n == 0){
        puts(active_only
             ? "\nAnda tidak sedang meminjam buku apa pun."
             : "\nAnda belum pernah meminjam buku.");
    } else{
        ui_show_loans(&app->books, view, n, 0, title);
    }

    free(view);
    return n;
}

static void user_borrow(App *app, const User *session)
{
    char id[LEN_OCAID + 64];
    LoanResult r;
    const Book *b;

    ui_title("PINJAM BUKU");
    util_read_line("ID buku yang akan dipinjam : ",
                   id, sizeof(id));

    if (util_eof()){
        return;
    }

    r = loan_borrow(&app->loans, &app->books,
                    session->username, id);

    switch (r){
    case LOAN_OK:
        b = booklist_find(&app->books, id);
        printf("\nPeminjaman berhasil: \"%s\"\nSisa stok: %d\n",
               b->title, b->stock);
        break;

    case LOAN_NO_BOOK:
        puts("  ! Buku dengan ID tersebut tidak ditemukan.");
        break;

    case LOAN_NO_STOCK:
        puts("  ! Stok buku habis, tidak dapat dipinjam.");
        break;

    case LOAN_ALREADY:
        puts("  ! Anda masih meminjam buku ini. Kembalikan dahulu.");
        break;

    case LOAN_ERR_MEM:
        puts("  ! Memori tidak cukup.");
        break;

    default:
        puts("  ! Gagal menyimpan data. Peminjaman dibatalkan.");
        break;
    }
}

static void user_return(App *app, const User *session)
{
    char id[LEN_OCAID + 64];
    LoanResult r;

    ui_title("KEMBALIKAN BUKU");

    if (user_show_loans(app, session, 1,
                        "BUKU YANG SEDANG ANDA PINJAM") == 0) {
        return;
    }

    util_read_line("\nID buku yang akan dikembalikan : ",
                   id, sizeof(id));

    if (util_eof()){
        return;
    }

    r = loan_return(&app->loans, &app->books,
                    session->username, id);

    switch (r){
    case LOAN_OK:
        puts("\nPengembalian berhasil. Terima kasih!");
        break;

    case LOAN_NOT_FOUND:
        puts("  ! Tidak ada peminjaman aktif untuk ID buku tersebut.");
        break;

    default:
        puts("  ! Gagal menyimpan data. Pengembalian dibatalkan.");
        break;
    }
}

void menu_user_run(App *app, const User *session)
{
    int choice;

    do {
        printf("\n=== MENU ANGGOTA | login: %s ===\n",
               session->username);

        puts("1. Lihat katalog buku tersedia (dengan pilihan sorting)");
        puts("2. Cari buku (ID / judul)");
        puts("3. Pinjam buku");
        puts("4. Kembalikan buku");
        puts("5. Daftar buku yang sedang saya pinjam");
        puts("6. Riwayat peminjaman saya");
        puts("0. Logout");

        choice = util_prompt_int("Pilihan: ", 0, 6);

        if (util_eof()){
            break;
        }

        switch (choice){
        case 1:
            ui_browse_catalog(&app->books, 1);
            break;

        case 2:
            ui_search_books(&app->books);
            break;

        case 3:
            user_borrow(app, session);
            break;

        case 4:
            user_return(app, session);
            break;

        case 5:
            user_show_loans(app, session, 1,
                            "BUKU YANG SEDANG SAYA PINJAM");
            break;

        case 6:
            user_show_loans(app, session, 0,
                            "RIWAYAT PEMINJAMAN SAYA");
            break;

        default:
            break;
        }

    } while (choice != 0);
}