#include <stdlib.h>
#include <string.h>
#include "curses.h"
#include "ui.h"
#include "user.h"
#include "product.h"

int menu_centered(const int num_items, const int max_width, const char **items);
int menu_compact(const int num_items, const int max_width, const char **items);

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

void product_edit_screen()
{
    clear();
    Product *current = get_products();
    char product_name[21];
    int quantity;
    int price;
    char location[256];
    // Product *product = NULL;
    int width = COLS / 2;
    int height = 12;
    WINDOW *win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
    wclear(win);
    mvwprintw(win, 2, 4, "Product type: ");
    mvwprintw(win, 3, 4, "Product name: ");
    mvwprintw(win, 4, 4, "Quantity: ");
    mvwprintw(win, 5, 4, "Price: ");
    mvwprintw(win, 6, 4, "Location: ");
    wrefresh(win);
    echo(); // Turn on echo so the user sees what they are typing.
    curs_set(1); // Show the cursor so the user sees where they are typing.
    mvwprintw(win, 2, 18, "%s", current->product_type);
    mvwscanw(win, 3, 18, "%20[^\n]", product_name);
    mvwscanw(win, 4, 18, "%d", &quantity);
    mvwscanw(win, 5, 18, "%d", &price);
    mvwscanw(win, 6, 18, "%20[^\n]", location);
    curs_set(0); // Hide the cursor again.
    wrefresh(win);
    save_products();
    int count = save_products();
    if (count < 0) // Something went wrong.
    {
        mvwprintw(win, 6, 2, "Something went wrong. Press any key to continue.");
        wrefresh(win);
        getch();
    }
    wrefresh(win);
    return;
}

void show_message(char *msg)
{
    const char s[] = " - Press any key.";
    int l = LINES - 1;
    int c = (COLS - strlen(msg) - strlen(s)) / 2;
    move(l, 0);
    clrtoeol();
    mvprintw(l, c, "%s%s", msg, s);
    getch();
    move(l, 0);
    clrtoeol();
}


void printMessageCenter(const int l, const char *message)
{
    const int col = (COLS - strlen(message)) / 2; // COLS is the number of columns of the screen. Calculate column to center message.
    mvprintw(l, col, "%s", message); // mvprintw is like printf() but with the row and column position where to start.
}

void headMessage(const char *message)
{
    clear(); // Clear the screen.
    const char l[] = "같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같�";
    const int col = (COLS - strlen(l)) / 2; // Calculate the column to center the string.
    mvprintw(0, col, "같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같�");
    mvprintw(1, col, "�                                                                         �");
    mvprintw(2, col, "�                     Farmer's Cooperative Sales Platform                 �");
    mvprintw(3, col, "�                                                                         �");
    mvprintw(4, col, "같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같같�");
    mvprintw(6, col, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*");
    printMessageCenter(7, message);
    mvprintw(8, col, "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*");
    refresh();
}
