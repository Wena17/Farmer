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
    mvprintw(10, 24, "Product");
    mvprintw(10, 44, "Buyer Name");
    mvprintw(10, 59, "Quantity");
    mvprintw(10, 75, "Price");
    while(true)
    {
        Sale *current = get_sales();
        int line = 12;
        int i = 0;
        move(line, 0);
        clrtobot();
        while (current != NULL)
        {
            if (current->product->seller == get_logged_in_user())
            {
                if (i >= 9 * page && i < 9 * (page + 1))
                {
                    mvprintw(line, 24, "%s", current->product->product_name);
                    clrtoeol();
                    mvprintw(line, 44, "%s", current->buyer->user_name);
                    clrtoeol();
                    mvprintw(line, 59, "%d", current->quantity);
                    clrtoeol();
                    mvprintw(line, 75, "%d", current->price);
                    clrtoeol();
                    mvprintw(line, 80, "%d", current->mode);
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
