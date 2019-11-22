#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curses.h"
#include "ui.h"
#include "sqlite3.h"

/* Declare functions before use. */
void init_db();
void welcomeMessage();
void homePage();

sqlite3 *db; // This is a handle to the database. It's global because we'll need it everywhere and there is only one.

int main()
{
    init_db();
    initscr(); // Initialize the curses library.
    int cursor_setting = curs_set(0);
    noecho(); // Typed characters are not automatically printed on the screen.
    welcomeMessage();
    homePage();
    curs_set(cursor_setting); // Back to normal.
    echo(); // Back to normal.
    endwin(); // Close the curses library.
    sqlite3_close(db);
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

void homePage()
{
    clear();
    headMessage("HOMEPAGE");
    const char *m[] = { "Admin", "Seller", "Buyer", NULL }; // The NULL pointer marks the end of the list.
    menu(m);
}

void init_db()
{
    int rc = sqlite3_open("db.sqlite3", &db); // Open (or create) the database file.
    if (rc)
    {
        printf("%s", "Could not open database; quitting.");
        exit(1);
    }
    char *err_msg;
    rc = sqlite3_exec(db, "SELECT version FROM farmer_version LIMIT 1;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(&err_msg);
        create_db();
    }
}

void create_db()
{
    printf("Need to create database schema.\n");
    if (sql_exec("CREATE TABLE IF NOT EXISTS farmer_version (id INTEGER PRIMARY KEY AUTOINCREMENT, version INTEGER NOT NULL);"));
    {
        printf("%s\n", "Could not create table farmer_version; quitting.");
        exit(1);
    }
    if (begin_tx() == SQLITE_OK)
    {
        int rc = sql_exec("INSERT INTO farmer_version (version) VALUES (0);");
        if (rc || !commit_tx())
        {
            printf("%s", "Quitting\n");
            exit(1);
        }
    }
}

int sql_exec(char *sql)
{
    char *err_msg;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    if (rc)
    {
        fprintf(stderr, "%s\n", err_msg);
        sqlite3_free(err_msg);
    }
    return rc;
}

int begin_tx()
{
    char *err_msg;
    int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "%s", err_msg);
        sqlite3_free(err_msg);
    }
    return rc;
}

int commit_tx()
{
    char *err_msg;
    int rc = sqlite3_exec(db, "COMMIT;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "%s", err_msg);
        sqlite3_free(err_msg);
    }
    else
    {
        rc = sqlite3_exec(db, "ROLLBACK;", NULL, NULL, &err_msg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "%s", "Rollback failed, quitting.\n");
            fprintf(stderr, "%s", err_msg);
            exit(2);
        }
    }
    return rc == 0;
}
