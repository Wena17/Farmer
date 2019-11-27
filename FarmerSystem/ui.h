#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "user.h"
#include "Product.h"

int menu(const char **items);

User *login();
User *signup();

Product *new_product(const User * user);

#endif // UI_H_INCLUDED
