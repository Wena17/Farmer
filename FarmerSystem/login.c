#include <stdlib.h>

#include "login.h"
#include "curses.h"
#include "ui.h"
#include "user.h"

User *user; // This will be the logged in user.
WINDOW *user_win;

void login_init()
{
    user = NULL;
    user_win = NULL;
}

void display_user()
{
    if (!user_win)
        user_win = newwin(1, 20, 0, 0);
    wclear(user_win);
    mvwprintw(user_win, 0, 0, "%s", get_logged_in_user() ? get_logged_in_user()->user_name : "Anonymous");
    wrefresh(user_win);
}

bool login_or_signup(const bool is_seller)
{
    if (user)
        return true; // Already logged in.
    clear();
    headMessage("WHAT'S NEXT?");
    const char *solm[] = { "Login", "Sign up", "Exit", NULL };
    while (user == NULL)
    {
        switch (menu(solm))
        {
        case 0:
            login();
            break;
        case 1:
            signup(is_seller); // If user name already exists, there should be error.
            break;
        case 2:
            break;
        }
    }
    if (user && user->is_seller != is_seller)
    {
        user = NULL;
        if (is_seller)
            show_message("User is not a seller.");
        else
            show_message("User is not a buyer.");
    }
    return user != NULL;
}

bool login()
{
    headMessage("LOGIN");
    if (user)
        return true;
    char user_name[30];
    char pass[20];
    int width = COLS / 2;
    int height = 7;
    int i = 0;
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
            i++;
        }
        if(i >= 3)
        {
            show_message("You've reached the times to login.");
            break;
        }
    }
    while (user == NULL);
    wclear(win);
    wrefresh(win);
    delwin(win);
    display_user();
    return true;
}

bool signup(const bool is_seller)
{
    headMessage("SIGN UP");
    if (user)
        return true;
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
    mvwscanw(win, 3, 12, "%s", email);
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
    display_user();
    return user != NULL;
}

User *get_logged_in_user()
{
    return user;
}

void logout()
{
    user = NULL;
    display_user();
    refresh();
}

bool have_admin_rights()
{
    return user && user->is_admin;
}
