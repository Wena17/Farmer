#ifndef LOGIN_H_INCLUDED
#define LOGIN_H_INCLUDED

#include <stdbool.h>
#include "user.h"

void login_init();
bool login_or_signup(const bool is_seller);
bool login();
bool signup(const bool is_seller);
User *get_logged_in_user();
bool have_admin_rights();
bool have_seller_rights();
void logout();

#endif // LOGIN_H_INCLUDED
