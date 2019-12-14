#include "curses.h"
#include "login.h"
#include "product.h"
#include "ui_seller.h"
#include "ui.h"
#include "sales.h"
#include "ctype.h"

void show_sales_for_seller()
{
    clear();
    headMessage("ALL YOUR SALES");
    int page = 0;
    mvprintw(10, 15, "Product");
    mvprintw(10, 35, "Buyer Name");
    mvprintw(10, 55, "Quantity");
    mvprintw(10, 70, "Price");
    mvprintw(10, 80, "Total Price");
    mvprintw(10, 98, "<1>Pick-up");
    mvprintw(11, 98, "<2>Delivery");
    while(true)
    {
        Sale *current = get_sales();
        int line = 13;
        int i = 0;
        move(line, 0);
        clrtobot();
        while (current != NULL)
        {
            if (current->product->seller == get_logged_in_user())
            {
                if (i >= 9 * page && i < 9 * (page + 1))
                {
                    mvprintw(line, 16, "%s", current->product->product_name);
                    clrtoeol();
                    mvprintw(line, 36, "%s", current->buyer->user_name);
                    clrtoeol();
                    mvprintw(line, 56, "%6d", current->quantity);
                    clrtoeol();
                    mvprintw(line, 70, "%5d", current->price);
                    clrtoeol();
                    mvprintw(line, 82, "%8d", current->quantity * current->price);
                    clrtoeol();
                    mvprintw(line, 100, "%d", current->mode);
                    clrtoeol();
                    line++;
                }
                i++;
            }
            current = current->next;
        }
        bool is_last_page = current == NULL;

        mvprintw(LINES - 2, 30, "(p) Previous");
        mvprintw(LINES - 2, 50, "(n) Next");
        mvprintw(LINES - 2, 70, "(0) Back");
        refresh();
        char c = getch();
        switch (tolower(c))
        {
        case '0':
            move(10,0);
            clrtobot();
            refresh();
            return;
        case 'p':
            if (page == 0)
                show_message("You're already on the first page.");
            else
                page--;
            break;
        case 'n':
            if(is_last_page)
            {
                show_message("No Records Found");
            }
            else
                page++;
            break;
        default:
            show_message("Invalid selection");
            break;
        }
    }


}

void product_edit_screen(Product *p)
{
    clear();
    int width = COLS / 2;
    int height = 12;
    WINDOW *win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
    wclear(win);
    mvwprintw(win, 2, 4, "Product type: %s", p->product_type);
    mvwprintw(win, 3, 4, "Product name: %s", p->product_name);
    mvwprintw(win, 4, 4, "Quantity:     %d", p->quantity);
    mvwprintw(win, 5, 4, "Price:        %d", p->price);
    mvwprintw(win, 6, 4, "Location:     %s", p->location);
    wrefresh(win);
    echo(); // Turn on echo so the user sees what they are typing.
    curs_set(1); // Show the cursor so the user sees where they are typing.
    mvwscanw(win, 2, 18, "%255[^\n]", p->product_type);
    mvwscanw(win, 3, 18, "%255[^\n]", p->product_name);
    mvwscanw(win, 4, 18, "%d", &p->quantity);
    mvwscanw(win, 5, 18, "%d", &p->price);
    mvwscanw(win, 6, 18, "%255[^\n]", p->location);
    curs_set(0); // Hide the cursor again.
    wrefresh(win);
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

