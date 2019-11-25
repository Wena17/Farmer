#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "user.h"

int menu(const char **items);
int login_or_register();

User *login();
User *signup();

#endif // UI_H_INCLUDED
