#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curses.h"
#include "ui.h"
#include "user.h"

/* Declare functions before use. */
void welcomeMessage();
void login_or_signup();
void homePage();
void newproduct_menu();
void seller_menu();
void show_products();


User *user = NULL; // This will be the logged in user.

int main()
{
    load_users();
    load_products();
    initscr(); // Initialize the curses library.
    int cursor_setting = curs_set(0);
    noecho(); // Typed characters are not automatically printed on the screen.
    welcomeMessage();
    homePage();
    curs_set(cursor_setting); // Back to normal.
    echo(); // Back to normal.
    endwin(); // Close the curses library.
    save_users();
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

void welcomeMessage()
{
    headMessage("Welcome");
    /* This can be optimized to use screen coordinates. */
    printw("\n\n\n\t\t\t\t\t\t°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");
    printw("\n\t\t\t\t\t      °°                              °°");
    printw("\n\t\t\t\t\t   °°                                    °°");
    printw("\n\t\t\t\t\t°°                                          °°");
    printw("\n\t\t\t\t     °°  ||                 WELCOME                ||  °°");
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

void login_or_signup()
{
    clear();
    headMessage("WHO ARE YOU?");
    const char *solm[] = { "Login", "Sign up", "Quit", NULL };
    while (user == NULL)
    {
        switch (menu(solm))
        {
        case 0:
            user = login();
            break;
        case 1:
            user = signup();//If user name already exist should be error
            break;
        case 2:
            return;
        }
    }
    return;
}

void homePage()
{
    clear();
    headMessage("HOMEPAGE");
    const char *m[] = { "Admin", "Seller", "Buyer", "Quit", NULL }; // The NULL pointer marks the end of the list.
    bool is_running = true;
    while (is_running)
    {
        switch (menu(m))
        {
        case 0:
            login();
                break;
            // TODO
            break;
        case 1:
            login_or_signup();
            if (user != NULL) seller_menu();
            break;
        case 2:
            login_or_signup();
            // TODO
            break;
        case 3:
            is_running = false;
            break;
        }
    }
}

void seller_menu()
{
    clear();
    headMessage("SELLER MENU");
    const char *sellm[] = { "Add new product", "See product updates", "Exit", NULL};
    while (true)
    {
        switch(menu(sellm))
        {
        case 0:
            newproduct_menu();
            //TODO
            break;
        case 1:
            show_products();
            break;
        case 2:
            return;
        }
    }
}
void newproduct_menu()
{
    clear();
    headMessage("PRODUCT TYPE");
    const char *sellm[] = { "Fruits", "Vegetables", "Others", "Back", NULL};
    switch(menu(sellm))
    {
    case 0:
        new_product(user);
        //TODO
        break;
    case 1:
        new_product(user);
        //TODO
        break;
    case 2:
        //TODO
        break;
    case 3:
        return;
        break;
    }
}

void show_products()
{
    clear();
    headMessage("PRODUCTS YOU ARE SELLING");
    Product *current = get_products();
    int line = 12;
    while (current != NULL)
    {
        if (current->seller == user)
        {
            mvprintw(line++, 2, "%s %d %d %s", current->product_name, current->quantity, current->price, current->location);
        }
        current = current->next;
    }
    mvprintw(LINES - 1, 2, "Press any key");
    refresh();
    getch();
    clear();
}
