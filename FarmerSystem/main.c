#include <stdio.h>
#include <string.h>


    int main()
    {
        welcomeMessage();
        homePage();
        return 0;
    }

    void printMessageCenter(const char *message)
    {
        int space = 0;
        int pos = 0;
    /*calculate how many space need to print*/
        space = 58;
        printf("\n");
        for(pos =0 ; pos < space ; pos++)
        {
        /*print space*/
            printf(" ");
        }
        /*print message*/
        printf("%s",message);
    }

    void headMessage(const char *message)
    {
        system("CLS");
        printf("\t\t\t°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");
        printf("\n\t\t\t°                                                                         °");
        printf("\n\t\t\t°                     Farmer's Cooperative Sales Platform                 °");
        printf("\n\t\t\t°                                                                         °");
        printf("\n\t\t\t°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");
        printf("\n\n\t\t\t*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*");
        printMessageCenter(message);
        printf("\n\t\t\t*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*");

    }

    void welcomeMessage()
    {
        headMessage("Welcome");
        printf("\n\n\n\t\t\t\t\t\t°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°");
        printf("\n\t\t\t\t\t      °°                              °°");
        printf("\n\t\t\t\t\t   °°                                    °°");
        printf("\n\t\t\t\t\t°°                                          °°");
        printf("\n\t\t\t\t     °°  ||                 WELCOME                ||  °°");
        printf("\n\t\t\t\t\t  ||                  TO                  ||  ");
        printf("\n\t\t\t\t\t   ||               FARMER's             ||  ");
        printf("\n\t\t\t\t\t   ||             COOPERATIVE            ||  ");
        printf("\n\t\t\t\t\t   ||               SYSTEM               ||  ");
        printf("\n\t\t\t\t\t   ||                                    ||  ");
        printf("\n\t\t\t\t\t   ||                                    ||  ");
        printf("\n\t\t\t\t\t   ||                                    ||  ");
        printf("\n\t\t\t\t\t   ||*_**_**_**_**_**_**_**_**_**_**_**_*||  ");
        printf("\n\n\n\n\t\t\t\t\t     Press any key to continue.....");
        getch();

    }

    void homePage()
    {
        int num;

        headMessage("HOMEPAGE");
        printf("\n\n\n\t\t\t\t\t1.Admin");
        printf("\n\t\t\t\t\t2.Seller");
        printf("\n\t\t\t\t\t3.Buyer");
        printf("\n\n\n\t\t\t\t\t Enter choice  => ");
        getch();

    }
