#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "user.h"
#include "Product.h"

int menu(const char **items);
void show_message(char *msg);
void headMessage(const char *message);

Product *new_product(User *user, char *product_type);
Product *add_product(User *seller, const char *product_type, const char *product_name, const int quantity, const int price, const char *location);

#endif // UI_H_INCLUDED
