#ifndef PRODUCT_H_INCLUDED
#define PRODUCT_H_INCLUDED

typedef struct Product
{
    unsigned int id;
    char product_name[32];
    int quantity;
    int price;
    char location[256];
    bool is_fruit;
    struct Product *next;
} Product;

int load_products();
int save_products();

Product *add_product(const char *product_name, const int quantity, const int price, const char *location);

#endif // USER_H_INCLUDED
