
#include <ctype.h>

#include "curses.h"
#include "ui_buyer.h"
#include "ui.h"
#include "product.h"
#include "login.h"
#include "sales.h"

Product *products;

void pickup_product(const User *seller, Product *product)
{
    int remaining = product_reduce_quantity(product, 1); //Minus the quantity of the product
    if (remaining < 0)
    {
        show_message("You cannot buy more than there is.");
    }
    else
    {
        add_sale(product, get_logged_in_user(), 1, product->price);
        show_message("Sold. It's all yours now.");
    }
}

Product *buy_product_screen(Product *product)
{
    const char *buyerm[] = {"Pick-up", "Delivered", "Back", NULL};

    while(true)
    {
        switch(menu(buyerm))
        {
        case 0:
            pickup_product(get_logged_in_user(), product);
            return product;
        case 1:
            //TODO
            break;
        case 2:
            return NULL;
        default:
            show_message("Invalid selection");
            break;
        }
    }

}

void show_buyer_screen()
{
    clear();
    headMessage("AVAILABLE PRODUCTS");
    int col = COLS / 4;
    int page = 0;
    Product *selected_product = NULL; // -1 means: Nothing is selected
    Product *displayed_products[9];
    mvprintw(10, col - 10, "Seller");
    mvprintw(10, col, "Product type");
    mvprintw(10, col + 18, "Products");
    mvprintw(10, col + 36, "Quantity");
    mvprintw(10, col + 48, "Price");
    mvprintw(10, col + 58, "Location");
    while(true)
    {
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

        mvprintw(LINES - 2, col - 3, "(b) Buy");
        mvprintw(LINES - 2, col + 15, "(p) Previous");
        mvprintw(LINES - 2, col + 35, "(n) Next");
        mvprintw(LINES - 2, col + 50, "(0) Logout");
        refresh();
        char c = getch();
        switch (tolower(c))
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
        default:
            show_message("Invalid selection");
            break;
        }
    }
}

