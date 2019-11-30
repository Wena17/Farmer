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
int menu(const int format, const char **items)
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
    max_width += 3; // Add to allow numbering.
    if (max_width < 18)
        max_width = 18; // Make sure the input prompt can be displayed.

    switch (format)
    {
    case 0:
        return menu_centered(num_items, max_width, items);
    case 1:
        return menu_compact(num_items, max_width, items);
    }
    return -1;
}

int menu_centered(const int num_items, const int max_width, const char **items)
{
    /* Now calculate the starting position for the window. */
    const int start_col = (COLS - max_width) / 2;
    const int start_line = (LINES - num_items + 2) / 2;

    /* Create a window with the given size and coords and write the contents. */
    WINDOW *win = newwin(num_items + 2, max_width, start_line, start_col);
    for (int i = 0; i < num_items; i++)
    {
        mvwprintw(win, i, 0, "%i) %s", i + 1, items[i]);
    }
    mvwprintw(win, num_items + 1, 0, "%s", "Enter your choice.");
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

int menu_compact(const int num_items, const int max_width, const char **items)
{
    int items_per_line = COLS / (max_width + 2);
    int line = LINES - (num_items / COLS) - 2;
    for (int i = 0; i < num_items; i++)
    {
        mvprintw(line + i / items_per_line, (max_width + 2) * (i % items_per_line), "%d) %s", i + 1, items[i]);
    }
    refresh();
       int choice;
    do
    {
        choice = getch() - '1'; // Neat way to translate the character to the zero-based index of the item.
    }
    while (choice < 0 || choice >= num_items);
    for (int i = line; i < LINES; i++)
    {
        move(i, 0);
        clrtoeol();
    }
    refresh();
    return choice;
}

User *login()
{
    char user_name[30];
    char pass[20];
    User *user = NULL;
    int width = COLS / 2;
    int height = 7;
    WINDOW *win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
    wclear(win);
    mvwprintw(win, 2, 2, "Username: ");
    mvwprintw(win, 3, 2, "Password: ");
    wrefresh(win);
    do
    {
        echo();
        curs_set(1);
        mvwscanw(win, 2, 12, "%20s", &user_name);
        noecho();
        mvwscanw(win, 3, 12, "%10s", &pass);
        curs_set(0);
        user = check_password(user_name, pass);
        if (user == NULL)
        {
            mvwprintw(win, 5, 2, "Wrong user name or password.");
            wmove(win, 2, 12);
            wclrtoeol(win);
            wmove(win, 3, 12);
            wclrtoeol(win);
            wrefresh(win);
        }
    }
    while (user == NULL);
    wclear(win);
    wrefresh(win);
    delwin(win);
    return user;
}

User *signup(const bool is_seller)
{
    char user_name[21];
    char email[256];
    char pw_hash[11];
    User *user = NULL;
    int width = COLS / 2;
    int height = 8;
    WINDOW *win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
    wclear(win);
    mvwprintw(win, 2, 2, "Username: ");
    mvwprintw(win, 3, 2, "Email: ");
    mvwprintw(win, 4, 2, "Password: ");
    wrefresh(win);
    echo(); // Turn on echo so the user sees what they are typing.
    curs_set(1); // Show the cursor so the user sees where they are typing.
    mvwscanw(win, 2, 12, "%20s", user_name);
    mvwscanw(win, 3, 12, "%50s", email);
    mvwscanw(win, 4, 12, "%20s", pw_hash);
    curs_set(0); // Hide the cursor again.
    noecho(); // Don't show what the users types.
    wmove(win, 4, 12);
    wclrtoeol(win); // Let's delete the password for security.
    wrefresh(win);
    user = add_user(user_name, email, pw_hash, is_seller); // Now add and save the user, return the new id.
    if (user == NULL) // Something went wrong.
    {
        mvwprintw(win, 6, 2, "Something went wrong. Press any key to continue.");
        wrefresh(win);
        getch();
    }
    wclear(win);
    wrefresh(win);
    delwin(win);
    return user;
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
    char product_type[30];
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
    mvwscanw(win, 2, 18, "%20[^\n]", product_type);
    mvwscanw(win, 3, 18, "%20[^\n]", product_name);
    mvwscanw(win, 4, 18, "%d", &quantity);
    mvwscanw(win, 5, 18, "%d", &price);
    mvwscanw(win, 6, 18, "%20[^\n]", location);
    curs_set(0); // Hide the cursor again.
    noecho(); // Don't show what the users types.
    wmove(win, 4, 12);
    wclrtoeol(win); // Let's delete the password for security.
    wrefresh(win);
    int count = save_products();
    if (count < 0) // Something went wrong.
    {
        mvwprintw(win, 6, 2, "Something went wrong. Press any key to continue.");
        wrefresh(win);
        getch();
    }
    wclear(win);
    wrefresh(win);
    delwin(win);
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
