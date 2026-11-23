#include <stdio.h>
#include <stdlib.h>

#include "app.h"
#include "auth.h"
#include "menu_user.h"
#include "util.h"

int main(void)
{
    App app;
    User session;
    char input[16];
    int pilihan;

    if (app_init(&app) != 0) {
        fprintf(stderr, "Gagal menyiapkan aplikasi.\n");
        app_free(&app);
        return 1;
    }

    while (1) {
        puts("\n================================");
        puts(" SISTEM MANAJEMEN PERPUSTAKAAN");
        puts("================================");
        puts("1. Login");
        puts("2. Registrasi");
        puts("0. Keluar");

        util_read_line("Pilihan: ", input, sizeof(input));

        if (util_eof()) {
            break;
        }

        pilihan = atoi(input);

        switch (pilihan) {
            case 1:
                if (auth_login_interactive(&app, &session)) {
                    if (user_is_admin(&session)) {
                        puts("\nLogin admin berhasil.");
                        puts("Menu admin belum tersedia.");
                    } else {
                        menu_user_run(&app, &session);
                    }
                }
                break;

            case 2:
                auth_register(&app);
                break;

            case 0:
                puts("\nTerima kasih telah menggunakan aplikasi.");
                goto selesai;

            default:
                puts("\nPilihan tidak valid.");
        }
    }

selesai:
    app_free(&app);
    return 0;
}