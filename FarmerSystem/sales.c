
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "sales.h"

#define filename "sales.csv"
#define filename2 "sales-tmp.csv"
#define BUF_SIZE 256

Sale *sales;

int load_sales()
{
    FILE *f = fopen(filename, "r");
    if (f == NULL) // Could not open the file.
    {
        fprintf(stderr, "%s:%d WARNING! Could not load product file.\n", __FUNCTION__, __LINE__);
        return 0; // If we can't read any products we just forget about them.
    }
    char buf[BUF_SIZE]; // We'll use this variable to load lines of our file into. C is not very convenient when it comes to strings, you have to do a lot of stuff by hand.
    Sale *previous = NULL; // We keep track of the previous user we've read so we can set its "next" pointer later. This will become clearer further down.
    int count = 0; // Let's count the users.
    while (true) // Endless loop.
    {
        if (fgets(buf, BUF_SIZE, f) == NULL) //  We read the next line from the file into our buf variable. If we get NULL, we've reached the end of the file.
            break; // ... so exit the loop.
        int product_id;
        int buyer_id;
        int quantity;
        int price;
        int rc = sscanf(buf, " %d,%d,%d,%d", &product_id, &buyer_id, &quantity, &price);
        if (rc != 4) // The number of fields read is in rc. This should be 5 unless it's somehow an invalid line. If it's invalid, simply skip it.
        {
            fprintf(stderr, "%s:%d WARNING! Skipping invalid line '%s'.\n", __FUNCTION__, __LINE__, buf); // Be nice and print a notice.
            continue; // Loop around.
        }
        Product *p = get_product_by_id(product_id);
        if (p == NULL)
        {
            fprintf(stderr, "%s:%d WARNING! Skipping invalid line '%s'.\n", __FUNCTION__, __LINE__, buf); // Be nice and print a notice.
            continue; // Loop around.
        }
        User *u = get_user_by_id(buyer_id);
        if (u == NULL)
        {
            fprintf(stderr, "%s:%d WARNING! Skipping invalid line '%s'.\n", __FUNCTION__, __LINE__, buf); // Be nice and print a notice.
            continue; // Loop around.
        }
        Sale *current = malloc(sizeof(Sale));
        current->buyer = u;
        current->product = p;
        current->price = price;
        current->quantity = quantity;
        current->next = NULL; // Initialize link in case this is the last one.
        if (previous == NULL) // This means we just read the first user.
            sales = current; // So remember the start of the list in our global variable. Otherwise we'll never find it again.
        else // If it's not the first user...
            previous->next = current; // ... then this is the next of the previous. Does your
        count++; // We've read one more user. Increase the count of users by one.
        previous = current; // And now the current one becomes the previous one before we loop around. Yay, we're done with one user.
    }
    fclose(f); // We're done here. File is read completely. We get here only through the break statement further up.
    return count; // Return the number of users, just to be nice.
}

int save_sales()
{
    if (remove(filename2))
        fprintf(stderr, "%s:%d WARNING: I/O error %d\n", __FUNCTION__, __LINE__, errno);
    FILE *f = fopen(filename2, "w+");
    if (f == NULL)
    {
        fprintf(stderr, "%s:%d Could not open file.\n", __FUNCTION__, __LINE__); // Print a nice error message with function name and line number.
        return -1;
    }
    Sale *current = sales; // We start with the head of the list.
    int count = 0; // Let's count the users.
    while (current != NULL) // while we have more, we loop.
    {
        int written = fprintf(f, "%d,%d,%d,%d\n", current->product->id, current->buyer->id, current->quantity, current->price);
        if (written < 0 || written >= BUF_SIZE)
        {
            fclose(f); // We don't want dangling open files in case of an error.
            fprintf(stderr, "%s:%d Could not write file.\n", __FUNCTION__, __LINE__);
            return -1;
        }
        count++;
        current = current->next;
    }
    fclose(f); // We're done here.
    if (remove(filename))
    {
        fprintf(stderr, "%s:%d I/O error %d\n", __FUNCTION__, __LINE__, errno);
        return -1;
    }
    if (rename(filename2, filename))
    {
        fprintf(stderr, "%s:%d I/O error %d\n", __FUNCTION__, __LINE__, errno);
        return -1;
    }
    return count;
}

Sale *get_sales()
{
    return sales;
}

int add_sale(Product *product, User *buyer, int quantity, int price)
{
    Sale *tail = sales;
    while (tail)
    {
        if (tail->next == NULL)
            break;
        tail = tail->next;
    }
    Sale *s = malloc(sizeof(Sale));
    if (s == NULL)
    {
        fprintf(stderr, "%s:%d ERROR! Cannot allocate memory.\n", __FUNCTION__, __LINE__); // Be nice and print a notice.
        return -1;
    }
    s->product = product;
    s->buyer = buyer;
    s->quantity = quantity;
    s->price = price;
    s->next = NULL;
    if (tail) // If the list isn't empty ...
    {
        tail->next = s;
    }
    else
    {
        sales = s;
    }
    save_sales();
    return 0;
}
