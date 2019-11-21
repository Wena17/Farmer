#include <stdio.h>
#include <string.h>
#include "curses.h"
#include "ui.h"

/* Decalre functions before use. */
void welcomeMessage();
void homePage();

int main()
{
    initscr(); // Initialize the curses library.
    int cursor_setting = curs_set(0);
    noecho(); // Typed characters are not automatically printed on the screen.
    welcomeMessage();
    homePage();
    curs_set(cursor_setting); // Back to normal.
    echo(); // Back to normal.
    endwin(); // Close the curses library.
    return 0;
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

void welcomeMessage()
{
    headMessage("Welcome");
    /* This can be optimized to use screen coordinates. */
    printw("\n\n\n\t\t\t\t\t\t같같같같같같같같같같같같같같같");
    printw("\n\t\t\t\t\t      같                              같");
    printw("\n\t\t\t\t\t   같                                    같");
    printw("\n\t\t\t\t\t같                                          같");
    printw("\n\t\t\t\t     같  ||                 WELCOME                ||  같");
    printw("\n\t\t\t\t\t  ||                  TO                  ||  ");
    printw("\n\t\t\t\t\t   ||               FARMER's             ||  ");
    printw("\n\t\t\t\t\t   ||             COOPERATIVE            ||  ");
    printw("\n\t\t\t\t\t   ||               SYSTEM               ||  ");
    printw("\n\t\t\t\t\t   ||                                    ||  ");
    printw("\n\t\t\t\t\t   ||                                    ||  ");
    printw("\n\t\t\t\t\t   ||                                    ||  ");
    printw("\n\t\t\t\t\t   ||*_**_**_**_**_**_**_**_**_**_**_**_*||  ");
    printw("\n\n\n\n\t\t\t\t\t     Press any key to continue.....");
    refresh();
    getch();
}

void homePage()
{
    clear();
    headMessage("HOMEPAGE");
    const char *m[] = { "Admin", "Seller", "Buyer", NULL }; // The NULL pointer marks the end of the list.
    menu(m);
}

