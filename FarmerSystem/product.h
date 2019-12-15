#ifndef PRODUCT_H_INCLUDED
#define PRODUCT_H_INCLUDED

#include "user.h"

typedef struct Product
{
    unsigned int id;
    char product_name[256];
    char product_type[256];
    int quantity;
    int price;
    char date[265];
    char location[256];
    struct User *seller;
    struct Product *next;
} Product;

int load_products();
int save_products();

Product *add_product(User *seller, const char *product_type, const char *product_name, const int quantity, const int price, const char *date, const char *location);
void delete_product(Product *p);
Product *get_products();
int get_product_count(const User *u);
int product_reduce_quantity(Product *p, int q);
Product *get_product_by_id(int id);

#endif // USER_H_INCLUDED
