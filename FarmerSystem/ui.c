
#include <stdlib.h>
#include <string.h>
#include "curses.h"
#include "ui.h"
#include "user.h"

/* Show a menu with a null-pointer terminated list of items and return the index of the user's selection. */
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
    max_width += 3; // Add to allow numbering.
    if (max_width < 18)
        max_width = 18; // Make sure the input prompt can be displayed.

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

User *login()
{
    char user_name[21];
    char pass[11];
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

User *signup()
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
    user = add_user(user_name, email, pw_hash); // Now add and save the user, return the new id.
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
