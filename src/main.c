#include <stdio.h>

#include "app.h"
#include "auth.h"
#include "menu_admin.h"
#include "menu_user.h"
#include "util.h"

static void run_session(App *app, const User *session)
{
    printf("\nSelamat datang, %s (%s)!\n", session->username, session->role);
    if (user_is_admin(session)) {
        menu_admin_run(app, session);
    } else {
        menu_user_run(app, session);
    }
    printf("\nAnda telah logout. Sampai jumpa, %s.\n", session->username);
}

int main(int argc, char *argv[])
{
    App app;
    User session;
    int choice;

    if (argc != 1 && argc != 3) {
        fprintf(stderr, "Penggunaan: %s [username password]\n", argv[0]);
        return 1;
    }
    if (app_init(&app) != 0) {
        app_free(&app);
        return 1;
    }

    puts("\n###########################################");
    puts("#  SISTEM MANAJEMEN PERPUSTAKAAN (CLI)     #");
    puts("###########################################");

    /* Login lewat Command Line Argument (opsional) */
    if (argc == 3) {
        if (auth_login_args(&app, argv[1], argv[2], &session)) {
            run_session(&app, &session);
        } else {
            puts("Login via argumen gagal: username atau password salah.");
        }
    }

    do {
        puts("\n=== MENU UTAMA ===");
        puts("1. Login");
        puts("2. Registrasi akun anggota");
        puts("0. Keluar");
        choice = util_prompt_int("Pilihan: ", 0, 2);
        if (util_eof()) {
            break;
        }
        if (choice == 1) {
            if (auth_login_interactive(&app, &session)) {
                run_session(&app, &session);
            }
        } else if (choice == 2) {
            auth_register(&app);
        }
    } while (choice != 0);

    app_free(&app);
    puts("\nProgram selesai. Data tersimpan dengan aman.");
    return 0;
}