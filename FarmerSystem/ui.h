#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "user.h"
#include "Product.h"
#include "ui_buyer.h"

int menu(const char **items); // This is a big menu with a box.
void show_message(char *msg);
void headMessage(const char *message);
char show_edit_menu(bool has_edit);
char show_menu(char *menu_chars, char **menu_items); // This is a small menu at the bottom.
void show_purchases();

Product *new_product(User *user, char *product_type);

#endif // UI_H_INCLUDED
