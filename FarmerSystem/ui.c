#include "curses.h"
#include "ui.h"

/* Show a menu with a null-pointer terminated list of items and return the user's selection. */
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
    mvwprintw(win, num_items + 2, 0, "%s", "Enter your choice.");
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
