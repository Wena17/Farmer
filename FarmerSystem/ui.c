#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "curses.h"
#include "ui.h"
#include "user.h"
#include "product.h"
#include "ui_buyer.h"
#include "login.h"
#include "sales.h"

Product *product;
User *user;

void show_purchases()
{
    const User *current_user = get_logged_in_user();
    const bool is_seller = current_user->is_seller;
    clear();
    headMessage(is_seller ? "ALL YOUR SALES" : "ALL YOUR PURCHASES");
    int page = 0;
    mvprintw(10, 15, "Product");
    mvprintw(10, 35, is_seller ? "Buyer Name" : "Seller Name");
    mvprintw(10, 55, "Quantity");
    mvprintw(10, 70, "Price");
    mvprintw(10, 80, "Total Price");
    mvprintw(10, 98, "<1>Pick-up");
    mvprintw(11, 98, "<2>Delivery");
    while(true)
    {
        Sale *current = get_sales();
        int line = 13;
        int i = 0;
        move(line, 0);
        clrtobot();
        while (current != NULL)
        {
            const User *role = is_seller ? current->product->seller : current->buyer;
            if (role == current_user)
            {
                if (i >= 9 * page && i < 9 * (page + 1))
                {
                    mvprintw(line, 16, "%s", current->product->product_name);
                    clrtoeol();
                    mvprintw(line, 36, "%s", is_seller ? current->buyer->user_name : current->product->seller->user_name);
                    clrtoeol();
                    mvprintw(line, 56, "%6d", current->quantity);
                    clrtoeol();
                    mvprintw(line, 70, "%5d", current->price);
                    clrtoeol();
                    mvprintw(line, 82, "%8d", current->quantity * current->price);
                    clrtoeol();
                    mvprintw(line, 100, "%d", current->mode);
                    clrtoeol();
                    line++;
                }
                i++;
            }
            current = current->next;
        }
        bool is_last_page = current == NULL;
        mvprintw(LINES - 2, 30, "(p) Previous");
        mvprintw(LINES - 2, 50, "(n) Next");
        mvprintw(LINES - 2, 70, "(0) Back");
        refresh();
        char c = getch();
        switch (tolower(c))
        {
        case '0':
            move(10,0);
            clrtobot();
            refresh();
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
        default:
            show_message("Invalid selection");
            break;
        }
    }
}

char show_menu(char *menu_chars, char **menu_items)
{
    int total_length = 0;
    char **current_item = menu_items;
    while (*current_item)
    {
        total_length += strlen(*current_item) + 4;
        current_item++;
        if (current_item) total_length += 2;
    }
    int col = (COLS - total_length) / 2;
    col = col < 0 ? 0 : col;
    char *current_char = menu_chars;
    current_item = menu_items;
    while (*current_item)
    {
        mvprintw(LINES - 2, col, "(%c) %s", *current_char, *current_item);
        col += strlen(*current_item) + 6;
        current_char++;
        current_item++;
    }
    refresh();
    while (true)
    {
        char c = tolower(getch());
        const char *s = menu_chars;
        while (*s)
        {
            if (*s == c) return c;
            s++;
        }
    }

}

/* Show a menu with common options for a paged list and wait for the user to select one. */
char show_edit_menu(bool has_edit)
{
    const int col = COLS / 4;
    const char *allowed = "0123456789depn";
    if (has_edit) mvprintw(LINES - 2, col - 3, "(e) Edit");
    mvprintw(LINES - 2, col + 10, "(d) Delete");
    mvprintw(LINES - 2, col + 25, "(p) Previous");
    mvprintw(LINES - 2, col + 38, "(n) Next");
    mvprintw(LINES - 2, col + 55, "(0) Back");
    while (true)
    {
        char c = tolower(getch());
        const char *s = allowed;
        while (*s)
        {
            if (*s == c && (has_edit || c != 'e')) return c;
            s++;
        }
    }
}

/* Show a menu with a null-pointer terminated list of items and return the index of the user's selection.
 *
 * The format parameter can be:
 *   0 - Centered on the screen
 *   1 - Compact menu at the bootom of the screen.
 */
int menu(const char **items)
{
    /* Determine the length of the longest menu item. */
    int num_items = 0;
    int max_width = 0;
    while (items[num_items] != NULL)
    {
        int l = strlen(items[num_items]);
        if (l > max_width)
            max_width = l;
        num_items++;
    }
    if (max_width == 0 || num_items == 0)
        return -1; // This should not happen.
    max_width += 4; // Add to allow numbering.
    if (max_width < 18)
        max_width = 18; // Make sure the input prompt can be displayed.

    /* Now calculate the starting position for the window. */
    const int start_col = (COLS - max_width - 4) / 2;

    /* Create a window with the given size and coords and write the contents. */
    WINDOW *win = newwin(num_items + 4, max_width + 4, 12, start_col);
    wclear(win);
    wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
    mvwprintw(win, 1, 2, "%s", "Enter your choice:");
    for (int i = 0; i < num_items; i++)
    {
        mvwprintw(win, i + 3, 2, "(%i) %s", i + 1, items[i]);
    }
    wrefresh(win);
    int choice;
    do
    {
        choice = getch() - '1'; // Neat way to translate the character to the zero-based index of the item.
    }
    while (choice < 0 || choice >= num_items);
    wclear(win); // Clear the window.
    wrefresh(win); // Display the changes.
    delwin(win); // Free up the window memory.
    return choice;
}


Product *new_product(User *user, char *product_type)
{
    char product_name[256];
    int quantity;
    int price;
    char location[256];
    Product *newproduct = NULL;
    int width = COLS / 2;
    int height = 8;
    WINDOW *win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
    wclear(win);
    mvwprintw(win, 2, 2, "Product name: ");
    mvwprintw(win, 3, 2, "Quantity: ");
    mvwprintw(win, 4, 2, "Price: ");
    mvwprintw(win, 5, 2, "Location: ");
    wrefresh(win);
    echo();
    curs_set(1);
    mvwscanw(win, 2, 15, "%50[^\n]", product_name);
    mvwscanw(win, 3, 15, "%d", &quantity);
    mvwscanw(win, 4, 15, "%d", &price);
    mvwscanw(win, 5, 15, "%100[^\n]", location);
    curs_set(0);
    noecho();
    wmove(win, 4, 12);
    wclrtoeol(win); // Let's delete the password for security.
    wrefresh(win);
    newproduct = add_product(user, product_type, product_name, quantity, price, location); // Now add and save the product, return the new id.
    if (newproduct == NULL) // Something went wrong.
    {
        mvwprintw(win, 6, 2, "Something went wrong. Press any key to continue.");
        wrefresh(win);
        getch();
    }
    wclear(win);
    wrefresh(win);
    delwin(win);
    return newproduct;
}

void show_message(char *msg)
{
    const char s[] = " - Press any key.";
    int l = LINES - 6;
    int c = (COLS - strlen(msg) - strlen(s)) / 2;
    move(l, 0);
    clrtoeol();
    mvprintw(l, c, "%s%s", msg, s);
    getch();
    move(l, 0);
    clrtoeol();
    refresh();
}

void headMessage(const char *message)
{
    refresh();
    const int len = strlen("°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");
    const int col = (COLS - len) / 2; // Calculate the column to center the string.
    WINDOW *win = newwin(9, len, 0, col);
    wclear(win);
    mvwprintw(win, 0, 0, "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");
    mvwprintw(win, 1, 0, "°                                                                         °");
    mvwprintw(win, 2, 0, "°                     Farmer's Cooperative Sales Platform                 °");
    mvwprintw(win, 3, 0, "°                                                                         °");
    mvwprintw(win, 4, 0, "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");
    mvwprintw(win, 6, 0, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*");
    mvwprintw(win, 7, (len - strlen(message)) / 2, "%s", message);
    mvwprintw(win, 8, 0, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*");
    wrefresh(win);
    delwin(win);
}
