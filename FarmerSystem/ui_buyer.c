#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "curses.h"
#include "ui_buyer.h"
#include "ui.h"
#include "product.h"
#include "login.h"
#include "sales.h"

Product *products;
User *user;
int buyer_max_id = 0;

int delivery_info(int mode, const Product *product)
{
    User *buyer = get_logged_in_user();
    clear();
    headMessage("DELIVERY INFORMATION");
    int quantity;
    int col = COLS / 3;
    mvprintw(15, col, "Full name:    %s", buyer->full_name);
    mvprintw(16, col, "Quantity:");
    if (mode == SALE_DELIVERY)
    {
        mvprintw(17, col, "Full address: %s", buyer->address);
    }
    else
    {
        mvprintw(17, col, "Pick up at:   %s", product->location);
    }
    mvprintw(18, col, "Contact No:   %s", buyer->phone);
    refresh();
    echo(); // Turn on echo so the user sees what they are typing.
    curs_set(1); // Show the cursor so the user sees where they are typing.
    mvscanw(15, col + 14, "%255[^\n]", buyer->full_name);
    mvprintw(15, col + 14, "%s", buyer->full_name);
    clrtoeol();
    refresh();
    do
    {
        mvscanw(16, col + 14, "%d", &quantity);
        mvprintw(16, col + 14, "%d", quantity);
        clrtoeol();
        refresh();
    }
    while (quantity < 0 || quantity > product->quantity);
    if (quantity == 0)
        return 0;
    if (mode == SALE_DELIVERY)
    {
        mvscanw(17, col + 14, "%255[^\n]", buyer->address);
        mvprintw(17, col + 14, "%s", buyer->address);
        clrtoeol();
        refresh();

    }
    mvscanw(18, col + 14, "%255[^\n]", buyer->phone);
    mvprintw(18, col + 14, "%s", buyer->phone);
    clrtoeol();
    refresh();
    curs_set(0); // Hide the cursor again.
    noecho();
    return quantity;
}

void sale_details_screen(Product *product, int mode)
{
    int desired = delivery_info(mode, product);
    if (desired <= 0)
    {
        show_message("You entered 0. I'm assuming you got cold feet.");
        return;
    }
    int remaining = product_reduce_quantity(product, desired); //Minus the quantity of the product
    if (remaining < 0)
    {
        show_message("You cannot buy more than there is.");
    }
    else
    {
        add_sale(product, get_logged_in_user(), desired, product->price, mode);
        show_message("Sold. It's all yours now.");
    }
}

void buy_product_screen(Product *product)
{
    const char *buyerm[] = {"Pick-up", "Delivered", "Back", NULL};
    while(true)
    {
        switch(menu(buyerm))
        {
        case 0:
            sale_details_screen(product, SALE_PICKUP);
            return;
        case 1:
            sale_details_screen(product, SALE_DELIVERY);
            return;
        case 2:
            return;
        default:
            show_message("Invalid selection");
            break;
        }
    }

}

void show_buyer_screen()
{
    clear();
    int col = COLS / 4;
    int page = 0;
    Product *selected_product = NULL; // -1 means: Nothing is selected
    Product *displayed_products[9];
    while(true)
    {
        headMessage("AVAILABLE PRODUCTS");
        mvprintw(10, col - 10, "Seller");
        mvprintw(10, col, "Product type");
        mvprintw(10, col + 18, "Products");
        mvprintw(10, col + 36, "Quantity");
        mvprintw(10, col + 48, "Price");
        mvprintw(10, col + 58, "Location");
        for (int i = 0; i < 9; i++)
            displayed_products[i] = NULL;
        Product *current = get_products();
        int line = 12;
        int i = 0;
        move(line, 0);
        clrtobot();
        while (current != NULL)
        {
            if (current->quantity > 0)
            {
                if (i >= 9 * page && i < 9 * (page + 1))
                {
                    displayed_products[i % 9] = current;
                    mvprintw(line, col - 20, "%d   =>", i % 9 + 1);
                    mvprintw(line, col - 10, "%s", current->seller->user_name);
                    clrtoeol();
                    mvprintw(line, col, "%s", current->product_name);
                    clrtoeol();
                    mvprintw(line, col + 18, "%s", current->product_type);
                    clrtoeol();
                    mvprintw(line, col + 36, "%d", current->quantity);
                    clrtoeol();
                    mvprintw(line, col + 48, "%d", current->price);
                    clrtoeol();
                    mvprintw(line, col + 58, "%s", current->location);
                    clrtoeol();
                    if (current == selected_product)
                    {
                        mvprintw(line, col - 22, "(Selected)");
                    }
                    line++;
                }
                i++;
            }
            current = current->next;
        }
        bool is_last_page = current == NULL;
        char *menu_chars = "bpnu0123456789";
        char *menu_items[] = { "Buy", "Previous", "Next", "pUrchases", "Logout", NULL };
        char c = show_menu(menu_chars, menu_items);
        switch (c)
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            selected_product = displayed_products[c - '1'];
            break;
        case '0':
            move(10,0);
            clrtobot();
            refresh();
            logout();
            return;
        case 'p':
            if (page == 0)
                show_message("You're already on the first page.");
            else
                page--;
            break;
        case 'n':
            if(is_last_page)
            {
                show_message("No Records Found");
            }
            else
                page++;
            break;
        case 'b':
            if (selected_product)
            {
                buy_product_screen(selected_product);
                selected_product = NULL;
            }
            break;
        case 'u':
            show_purchases();
            break;
        default:
            show_message("Invalid selection");
            break;
        }
    }
}
