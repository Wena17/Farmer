#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curses.h"
#include "ui.h"
#include "user.h"

/* Declare functions before use. */
void welcomeMessage();
void login_or_signup(const bool is_seller);
void homePage();
void newproduct_menu();
void seller_menu();
void show_products();
void edit_product_menu();
void product_edit_screen();
void show_users();

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
    if (user) mvprintw(0, 0, "%s", user->user_name);
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

void login_or_signup(const bool is_seller)
{
    clear();
    headMessage("WHAT'S NEXT?");
    const char *solm[] = { "Login", "Sign up", "Exit", NULL };
    while (user == NULL)
    {
        switch (menu(0, solm))
        {
        case 0:
            user = login();
            break;
        case 1:
            user = signup(is_seller);//If user name already exist should be error
            break;
        case 2:
            exit(0);
        }
    }
    return;
}

void homePage()
{
    clear();
    headMessage("HOMEPAGE");
    const char *m[] = { "Admin", "Seller", "Buyer", "Exit", NULL }; // The NULL pointer marks the end of the list.
    bool is_running = true;
    while (is_running)
    {
        switch (menu(0, m))
        {
        case 0:
            if (user == NULL) user = login();
            if(user->is_admin)
            {
                show_users();
            }
            else
            {
                user = NULL;
                show_message("No admin rights");
            }
            break;
        case 1:
            if (user == NULL) login_or_signup(true);
            if (user != NULL)
                seller_menu();
            break;
        case 2:
            if (user == NULL) login_or_signup(false);
            // TODO
            break;
        case 3:
            exit(0);
        }
    }
}

void seller_menu()
{
    const char *sellm[] = { "Add new product", "See product updates", "Logout", NULL};
    while (true)
    {
        clear();
        headMessage("SELLER MENU");
        switch(menu(0, sellm))
        {
        case 0:
            newproduct_menu();
            //TODO
            break;
        case 1:
            show_products();
            break;
        case 2:
            user = NULL;
            homePage();
        }
    }
}
void newproduct_menu()
{
    clear();
    headMessage("PRODUCT TYPE");
    const char *sellm[] = { "Fruits", "Vegetables", "Others", "Back", NULL};
    switch(menu(0, sellm))
    {
    case 0:
        headMessage("ADDING NEW PRODUCT FOR FRUITS");
        new_product(user, "fruit");
        //TODO
        break;
    case 1:
        headMessage("ADDING NEW PRODUCT FOR VEGETABLES");
        new_product(user, "vegetable");
        break;
    case 2:
        headMessage("ADDING OTHER TYPE OF PRODUCT");
        // TODO
        new_product(user, "other");
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
    int line = 11;
    int col = COLS / 4;
    int i = 0;
    mvprintw(10, col, "Products \t\t\t Quantity \t Price \t\t Location");
    while (current != NULL)
    {
        if (current->seller == user)
        {
            mvprintw(line, col - 3, "%d", ++i);
            mvprintw(line, col, "%s \t\t %d \t\t %d \t\t %s", current->product_name, current->quantity, current->price, current->location);
            line++;
        }

        current = current->next;
    }

    edit_product_menu();

}

void edit_product_menu()
{
    char produpdate;
    int height = LINES / 2;
    int col = COLS / 2;
    mvprintw(height, col - 20, "e) Edit");
    mvprintw(height, col - 10, "d) Delete");
    mvprintw(height, col, "0) Back");
    mvprintw(height + 3, col - 15, "Enter choice => ");
    echo();
    curs_set(1);
    mvscanw(height + 3, col, "%c", &produpdate);

    if(produpdate == 'e' || produpdate == 'E')
    {
        headMessage("UPDATING PRODUCTS");
        product_edit_screen();
    }
    else if(produpdate == 'd' || produpdate == 'D')
    {
        //TODO
    }
    else if(produpdate == 'o')
    {
        return;
    }
    /*else
    {
        mvprintw(height + 6, col, "Invalid selection Press to try again");
        getch();
        refresh();
        return edit_product_menu();
    }*/

}

void show_users()
{
    clear();
    headMessage("SELLER");
    User *u = get_users();
    int line = 11;
    int col = COLS / 4;
    // int i = 0;
    mvprintw(10, col, "Seller ID");
    mvprintw(10, col + 11, "Name");
    mvprintw(10, col + 22, "Email");
    mvprintw(10, col + 33, "Products");
    while (u != NULL)
    {
        if (!u->is_admin)
        {
            int count = get_product_count(u);
            mvprintw(line, col, "%d", u->id);
            mvprintw(line, col + 11, "%s", u->user_name);
            clrtoeol();
            mvprintw(line, col + 22, "%s", u->email);
            clrtoeol();
            mvprintw(line, col + 33, "%d", count);
            clrtoeol();
            if (u->is_seller)
            {
                mvprintw(line, col + 44, "(Seller)");
            }
            else
            {
                mvprintw(line, col + 44, "(Buyer)");
            }
            line++;
        }
        u = u->next;
    }
    refresh();
    getch();
}


