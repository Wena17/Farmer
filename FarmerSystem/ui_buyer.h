#ifndef UI_BUYER_H_INCLUDED
#define UI_BUYER_H_INCLUDED

#include "product.h"
#include "user.h"

typedef struct Buyer
{
    unsigned int id;
    char buyer_name[32];
    int quantity;
    char location[256];
    int contact;
    struct User *seller;
    struct Buyer *next;
} Buyer;

void buy_product_screen(Product *product);
Buyer *add_buyer(User *seller, const char *buyer_name, const int quantity, const char *location, int contact);
Buyer *append_buyer(Buyer *delivery_info);
void show_buyer_screen();
void delivered_product(const User *seller, Product *product, int mode);

#endif // UI_BUYER_H_INCLUDED
