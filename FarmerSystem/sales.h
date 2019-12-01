#ifndef SALES_H_INCLUDED
#define SALES_H_INCLUDED

#include "product.h"
#include "user.h"

typedef struct Sale {
   Product *product;
   User *buyer;
   int quantity;
   int price;
   struct Sale *next;
} Sale;

int load_sales();
Sale *get_sales();
// Sale *get_sales_for_seller(const User *seller);
// Sale *get_sales_for_buyer(const User *buyer);
int add_sale(Product *product, User *buyer, int quantity, int price);

#endif // SALES_H_INCLUDED
