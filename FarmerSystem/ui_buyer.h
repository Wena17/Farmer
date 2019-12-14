#ifndef UI_BUYER_H_INCLUDED
#define UI_BUYER_H_INCLUDED

#include "product.h"
#include "user.h"

void buy_product_screen(Product *product);
void show_buyer_screen();
void delivered_product(const User *seller, Product *product, int mode);

#endif // UI_BUYER_H_INCLUDED
