#ifndef PRODUCT_H_INCLUDED
#define PRODUCT_H_INCLUDED

#include "user.h"

typedef struct Product
{
    unsigned int id;
    char product_name[32];
    int quantity;
    int price;
    char location[256];
    bool is_fruit;
    struct User *seller;
    struct Product *next;
} Product;

int load_products();
int save_products();

Product *add_product(const User *seller, const char *product_name, const int quantity, const int price, const char *location);
Product *get_products();

#endif // USER_H_INCLUDED
