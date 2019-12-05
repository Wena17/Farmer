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

#define filename "buyers.csv"
#define BUF_SIZE 256

Product *products;
Buyer *buyers;
User *user;
int buyer_max_id = 0;


int delivery_info(int mode)
{
    clear();
    headMessage("DELIVERY INFORMATION");
    char buyer_name[50];
    int quantity;
    char location[256];
    int contact;
    int col = COLS / 3;
    mvprintw(15, col, "Full name: ");
    mvprintw(16, col, "Quantity: ");
    mvprintw(17, col, mode == SALE_DELIVERY ? "Complete Address: " : "For pickup");
    mvprintw(18, col, "Contact No: ");
    echo(); // Turn on echo so the user sees what they are typing.
    curs_set(1); // Show the cursor so the user sees where they are typing.
    mvscanw(15, col + 15, "%[^\n]", buyer_name);
    mvscanw(16, col + 15, "%d", &quantity);
    if (mode == SALE_DELIVERY)
    {
        mvscanw(17, col + 18, "%[^\n]", location);
    }
    mvscanw(18, col + 18, "%d", &contact);
    curs_set(0); // Hide the cursor again.
    Buyer *b = add_buyer(user, buyer_name, quantity, mode == SALE_DELIVERY ? location : "Pickup", contact);
    if (b == NULL) // Something went wrong.
    {
        mvprintw(LINES - 3, col, "Something went wrong. Press any key to continue.");
        getch();
        return 0;
    }
    refresh();
    return quantity;
}

void pickup_product(const User *seller, Product *product, int mode)
{
    int desired = delivery_info(SALE_PICKUP);
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

void delivered_product(const User *seller, Product *product, int mode)
{
    int desired = delivery_info(SALE_DELIVERY);
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
            pickup_product(get_logged_in_user(), product, SALE_PICKUP);
            return;
        case 1:
            delivered_product(get_logged_in_user(), product, SALE_DELIVERY);
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

Buyer *add_buyer(User *seller, const char *buyer_name, const int quantity, const char *location, int contact)
{
    /* First, we find the last user in the list. */
    Buyer *last = buyers;
    while (last != NULL && last->next != NULL)
        last = last->next;
    Buyer *delivery_info = malloc(sizeof(Product));
    delivery_info->id = ++buyer_max_id; // Assign the next id, increment *before* assignment.
    delivery_info->seller = seller;
    strcpy(delivery_info->buyer_name, buyer_name); // Same for the product name
    delivery_info->quantity = quantity; // This is a simple int, we can just assign it.
    strcpy(delivery_info->location, location); // Again a string that we need to copy.
    delivery_info->contact = contact;
    delivery_info->next = NULL; // Make sure the list is properly terminated.
    if (last != NULL) // Make sure there is a previous product at all.
        last->next = delivery_info; // Append to the list.
    else
        buyers = delivery_info; // If there was no product before, now this one will be the beginning of our list.
    return append_buyer(delivery_info);
}

/* Write a new product to the end of the file. This more robust than writing the complete file every time but of course doesn't work for updates of exisitng products. */
Buyer *append_buyer(Buyer *delivery_info)
{
    if (delivery_info == NULL || delivery_info->seller == NULL)
    {
        fprintf(stderr, "Trying to add NULL product or product with NULL seller.\n");
        return NULL;
    }
    FILE *f = fopen(filename, "a+"); // Here we simply append a line at the end of the file. Note that this doesn't work for changes/updates.
    if (f == NULL)
    {
        fprintf(stderr, "%s:%d Could not open file.\n", __FUNCTION__, __LINE__); // Print a nice error message with function name and line number.
        return NULL;
    }
    int written = fprintf(f, "%d,%d,%s,%d,%s,%d \n", delivery_info->id, delivery_info->seller->id, delivery_info->buyer_name, delivery_info->quantity, delivery_info->location, delivery_info->contact);
    if (written < 0 || written >= BUF_SIZE)
    {
        fclose(f); // We don't want dangling open files in case of an error.
        fprintf(stderr, "%s:%d Could not write file.\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    fclose(f); // We're done here.
    return delivery_info;
}

