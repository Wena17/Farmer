#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "user.h"
#include "Product.h"

int menu(const int format, const char **items);
void show_message(char *msg);

User *login();
User *signup(const bool is_seller);

Product *new_product(User *user, char *product_type);

#endif // UI_H_INCLUDED
