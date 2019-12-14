#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "curses.h"
#include "ui.h"
#include "user.h"
#include "product.h"
#include "ui_buyer.h"

Product *product;
User *user;

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

void printMessageCenter(const int l, const char *message)
{
    const int col = (COLS - strlen(message)) / 2; // COLS is the number of columns of the screen. Calculate column to center message.
    mvprintw(l, col, "%s", message); // mvprintw is like printf() but with the row and column position where to start.
}

void headMessage(const char *message)
{
    clear(); // Clear the screen.
    const char l[] = "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°";
    const int col = (COLS - strlen(l)) / 2; // Calculate the column to center the string.
    mvprintw(0, col, "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");
    mvprintw(1, col, "°                                                                         °");
    mvprintw(2, col, "°                     Farmer's Cooperative Sales Platform                 °");
    mvprintw(3, col, "°                                                                         °");
    mvprintw(4, col, "°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");
    mvprintw(6, col, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*");
    printMessageCenter(7, message);
    mvprintw(8, col, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*");
    refresh();
}
