#include "app.h"

#include <stdio.h>

void app_free(App *app)
{
    booklist_free(&app->books);
    userlist_free(&app->users);
    loanlist_free(&app->loans);
}