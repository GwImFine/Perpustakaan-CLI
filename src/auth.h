#ifndef AUTH_H
#define AUTH_H

#include "app.h"

int auth_login_interactive(const App *app, User *out);
int auth_login_args(const App *app, const char *user,
                    const char *pass, User *out);
int auth_register(App *app);

#endif