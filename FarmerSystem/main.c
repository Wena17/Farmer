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
char show_edit_menu();
char admin_menu();

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
    if (user)
        mvprintw(0, 0, "User is %s", user->user_name);
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
            if (user == NULL)
                user = login();
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
            if (user == NULL)
                login_or_signup(true);
            if (user != NULL)
                seller_menu();
            break;
        case 2:
            if (user == NULL)
                login_or_signup(false);
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
    int col = COLS / 4;
    int page = 0;
    int selection = -1; // -1 means: Nothing is selected
    mvprintw(10, col, "Products");
    mvprintw(10, col + 18, "Product type");
    mvprintw(10, col + 36, "Quantity");
    mvprintw(10, col + 48, "Price");
    mvprintw(10, col + 58, "Location");
    while(true)
    {
        Product *current = get_products();
        int line = 12;
        int i = 0;
        move(line, 0);
        clrtobot();
        while (current != NULL)
        {
            if (current->seller == user)
            {
                if (i >= 9 * page && i < 9 * (page + 1))
                {
                    mvprintw(line, col - 10, "%d   =>", i + 1);
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
                    if (i == selection)
                    {
                        mvprintw(line, col - 15, "(Selected)");
                    }
                    line++;
                }
                i++;
            }
            current = current->next;
        }
        char c = show_edit_menu();
        switch (c)
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
            selection = page * 9 + c - '1';
            break;
        case '0':
            return;
        case ' ':
            break;
        case 'p':
            page = page == 0 ? 0 : page - 1;
            break;
        case 'n':
            page++;
            break;
        case 'd':
            if (selection != -1)
            {
                delete_product_from_index(user, selection);
                selection = -1;
            }
            break;
        }
    }
}

char show_edit_menu()
{
    const int col = COLS / 4;
    mvprintw(21, col - 3, "e) Edit");
    mvprintw(21, col + 10, "d) Delete");
    mvprintw(21, col + 25, "p) Previous");
    mvprintw(21, col + 38, "n) Next");
    mvprintw(21, col + 55, "0) Back");
    while (true)
    {
        char c = getch();
        switch (c)
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
            return c;
        case 'e':
        case 'E':
            headMessage("UPDATING PRODUCTS");
            product_edit_screen();
            return ' ';
        case 'd':
        case 'D':
            return 'd';
        case '0':
            return '0';
        case 'p':
        case 'P':
            return 'p';
        case 'n':
        case 'N':
            return 'n';
        default:
            show_message("Invalid selection");
            break;
        }
    }
}

void show_users()
{
    clear();
    headMessage("USERS");
    int col = COLS / 4;
    int page = 0;
    User *selected_user = NULL; // Here we store the current selection or NULL if nothing is seletec.
    User *displayed_users[9]; // Here we remember which user is displayed at which position on the screen.
    mvprintw(10, col - 10, "No.");
    mvprintw(10, col, "Seller ID");
    mvprintw(10, col + 15, "Name");
    mvprintw(10, col + 30, "Email");
    mvprintw(10, col + 60, "Products");
    while(true)
    {
        int line = 12; // Display always starts at line 12.
        move(line, 0);
        clrtobot();
        for (int i = 0; i < 9; i++) displayed_users[i] = NULL; // Erase the array of displayed users.
        User *current = get_users(); // Get the start of the list of users.
        int i = 0; // Variable for counting.
        while (current != NULL && i < 9 * (page + 1)) // Only need to loop until the last user that is on the current page.
        {
            if (!current->is_admin) // Only display users who are not admins.
            {
                if (i >= 9 * page && i < 9 * (page + 1)) // Check if the current user should be displayed on the current page.
                {
                    displayed_users[i % 9] = current; // Now remember we are displaying the current user at the current position.
                    mvprintw(line, col - 10, "%d   =>", i % 9 + 1);
                    mvprintw(line, col, "%d", current->id);
                    mvprintw(line, col + 15, "%s", current->user_name);
                    clrtoeol();
                    mvprintw(line, col + 30, "%s", current->email);
                    clrtoeol();
                    if (current->is_seller)
                    {
                        mvprintw(line, col + 60, "%d", get_product_count(current));
                        clrtoeol();
                        mvprintw(line, col + 70, "(Seller)");
                    }
                    else
                    {
                        mvprintw(line, col + 70, "(Buyer)");
                    }
                    if (current == selected_user)
                    {
                        mvprintw(line, col - 15, "(Selected)");
                    }
                    line++; // Go to next line in the table.
                }
                i++; // Wether this user is on this page or not, count up.
            }
            current = current->next;
        }
        char c = admin_menu();
        switch (c)
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
            selected_user = displayed_users[c - '1'];
            break;
        case '0':
            user = NULL;
            homePage();
        case ' ':
            break;
        case 'p':
            page = page == 0 ? 0 : page - 1;
            selected_user = NULL; // We've changed the page, so unselect any selection.
            break;
        case 'n':
            page++;
            selected_user = NULL; // We've changed the page, so unselect.
            break;
        case 'd':
            if (selected_user)
            {
                if (get_product_count(selected_user))
                {
                    show_message("You can't delete a seller with active products.");
                }
                else
                {
                    delete_user(selected_user);
                    selected_user = NULL;
                }
            }
            break;
        }
    }

}

char admin_menu()
{
    const int col = COLS / 4;
    mvprintw(21, col - 3, "(d) Delete");
    mvprintw(21, col + 15, "(p) Previous");
    mvprintw(21, col + 35, "(n) Next");
    mvprintw(21, col + 50, "(0) Logout");
    while (true)
    {
        char c = getch();
        switch (c)
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
            return c;
        case 'd':
        case 'D':
            return 'd';
        case '0':
            return '0';
        case 'p':
        case 'P':
            return 'p';
        case 'n':
        case 'N':
            return 'n';
        default:
            show_message("Invalid selection");
            break;
        }
    }
}


