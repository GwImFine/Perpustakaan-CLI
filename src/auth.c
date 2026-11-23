#include "auth.h"
#include <stdio.h>
#include <string.h>
#include "util.h"

int auth_login_args(const App *app, const char *user,
                    const char *pass, User *out)
{
    return users_authenticate(&app->users, user, pass, out);
}

int auth_login_interactive(const App *app, User *out)
{
    char username[LEN_USERNAME + 64];
    char password[LEN_PASSWORD + 64];

    puts("\n--- LOGIN ---");
    util_read_line("Username : ", username, sizeof(username));
    util_read_line("Password : ", password, sizeof(password));

    if (util_eof()){
        return 0;
    }

    if (users_authenticate(&app->users, username, password, out)){
        return 1;
    }

    puts("  ! Username atau password salah.");
    return 0;
}

int auth_register(App *app)
{
    User u;
    char pass2[LEN_PASSWORD + 64];
    char name[LEN_USERNAME + 64];
    char pass[LEN_PASSWORD + 64];
    int rc;

    puts("\n--- REGISTRASI AKUN BARU (role: user) ---");

    util_read_line("Username (tanpa spasi, maks 31 karakter): ",
                   name, sizeof(name));

    if (util_eof()){
        return 0;
    }

    if (!util_is_token(name) || strlen(name) >= LEN_USERNAME){
        puts("  ! Username tidak valid (tanpa spasi/karakter khusus, maks 31 karakter).");
        return 0;
    }

    if (users_find(&app->users, name) != NULL){
        puts("  ! Username sudah dipakai.");
        return 0;
    }

    util_read_line("Password (tanpa spasi, maks 31 karakter): ",
                   pass, sizeof(pass));

    if (!util_is_token(pass) || strlen(pass) >= LEN_PASSWORD){
        puts("  ! Password tidak valid (tanpa spasi/karakter khusus, maks 31 karakter).");
        return 0;
    }

    util_read_line("Ulangi password : ", pass2, sizeof(pass2));

    if (strcmp(pass, pass2) != 0){
        puts("  ! Password tidak sama.");
        return 0;
    }

    util_copy(u.username, sizeof(u.username), name);
    util_copy(u.password, sizeof(u.password), pass);
    util_copy(u.role, sizeof(u.role), ROLE_USER);

    rc = users_add(&app->users, &u);

    if (rc != 0 || users_save(&app->users) != 0){
        puts("  ! Gagal menyimpan akun.");
        return 0;
    }

    puts("  Registrasi berhasil. Silakan login.");
    return 1;
}