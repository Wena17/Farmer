#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "product.h"

#define filename "products.csv"
#define BUF_SIZE 256

Product *products; // This is where we will keep the users.
int products_max_id = 0; // We keep track of the highest ID so we know which one to use next.

/* Loads all users from the file and returns the number of users read, or -1 if there was an error. */
int load_products()
{
    FILE *f = fopen(filename, "r");
    if (f == NULL) // Could not open the file.
    {
        /* Opening the file did not work, so we create the admin user. */
//        products = malloc(sizeof(Product)); // Create one user. In C, you always have to allocate memory before you can write to it. That's what malloc does.
//        products->id = ++products_max_id; // Increment *before* assigning. Every user gets a unique ID number.
//        strcpy(products->product_name, "banana"); // Here I'm simply copying the strings to the new product data structure.
//        products->quantity;
//        products->price;
//        strcpy(products->location, "Cebu city");
//        products->is_fruit = 1;
//        products->next = NULL; // This is a pointer to the next user, creating a linked list of users. There is only one user here, so the "next" user is NULL.
//        return 1; // Pretend we read one user.
        return 0;
    }
    char buf[BUF_SIZE]; // We'll use this variable to load lines of our file into. C is not very convenient when it comes to strings, you have to do a lot of stuff by hand.
    Product *previous = NULL; // We keep track of the previous user we've read so we can set its "next" pointer later. This will become clearer further down.
    int count = 0; // Let's count the users.
    products_max_id = 0; // Keeping track of the highest id.
    while (true) // Endless loop.
    {
        if (fgets(buf, BUF_SIZE, f) == NULL) //  We read the next line from the file into our buf variable. If we get NULL, we've reached the end of the file.
            break; // ... so exit the loop.
        Product *u = malloc(sizeof(Product));
        u->next = NULL; // Initialize link in case this is the last one.
        int rc = sscanf(buf, " %d,%31[^,\n],%d,%d,%255[^,\n],%d", &u->id, u->product_name, u->quantity, &u->price, u->location, (int *) &u->is_fruit); // Read the various fields from the line in our buf variable.
        if (rc != 5) // The number of fields read is in rc. This should be 5 unless it's somehow an invalid line. If it's invalid, simply skip it.
        {
            free(u); // Free the allocated memory because we're skipping, so we don't run out of memory eventually. It's the opposite of malloc.
            fprintf(stderr, "Skipping invalid line.\n"); // Be nice and print a notice.
            continue; // Loop around.
        }
        /* At this point, we've read the fields of one line into the User in variable u. */
        if (previous == NULL) // This means we just read the first user.
            products = u; // So remember the start of the list in our global variable. Otherwise we'll never find it again.
        else // If it's not the first user...
            previous->next = u; // ... then this is the next of the previous. Does your
        count++; // We've read one more user. Increase the count of users by one.
        if (u->id > products_max_id) // If we found a higher id ...
            products_max_id = u->id; // ... then remember it. This is only necessary if we delete users later and the numbers are not strictly ascending anymore. But hey, let's be safe.
        previous = u; // And now the current one becomes the previous one before we loop around. Yay, we're done with one user.
    }
    fclose(f); // We're done here. File is read completely. We get here only through the break statement further up.
    return count; // Return the number of users, just to be nice.
}

/* Save all users in the file and return the number of users, or -1 if something went wrong.
 * This is actually a bit bad because when the writing fails, all data would be lost. Don't worry for now but in a real program this would have to be handled better. */
int save_products() {
    FILE *f = fopen(filename, "w+");
    if (f == NULL)
    {
        fprintf(stderr, "%s:%d Could not open file.\n", __FUNCTION__, __LINE__); // Print a nice error message with function name and line number.
        return -1;
    }
    Product *current = products; // We start with the head of the list.
    int count = 0; // Let's count the users.
    while (current != NULL) // while we have more, we loop.
    {
        int written = fprintf(f, "%d,%s,%d,%d,%s,%d\n", current->id, current->product_name, current->quantity, current->price, current->location, current->is_fruit);
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
    return count;
}

Product *add_product(const char *product_name, const int quantity, const int price, const char *location)
{
    /* First, we find the last user in the list. */
    Product *last = products;
    while (last != NULL && last->next != NULL)
        last = last->next;
    Product *new_product = malloc(sizeof(Product));
    new_product->id = ++products_max_id; // Assign the next id, increment *before* assignment.
    strcpy(new_product->product_name, product_name);
    new_product->quantity = quantity;
    new_product->price = price;
    strcpy(new_product->location, location);
    new_product->is_fruit = false;
    new_product->next = NULL; // Make sure the list is properly terminated.
    if (last != NULL) // Make sure there is a previous product at all.
        last->next = new_product; // Append to the list.
    else
        products = new_product; // If there was no product before, now this one will be the beginning of our list.
    FILE *f = fopen(filename, "a+");
    if (f == NULL)
    {
        fprintf(stderr, "%s:%d Could not open file.\n", __FUNCTION__, __LINE__); // Print a nice error message with function name and line number.
        return NULL;
    }
    int written = fprintf(f, "%d,%s,%d,%d,%s,%d \n", new_product->id, new_product->product_name, new_product->quantity, new_product->price, new_product->location, new_product->is_fruit);
    if (written < 0 || written >= BUF_SIZE)
    {
        fclose(f); // We don't want dangling open files in case of an error.
        fprintf(stderr, "%s:%d Could not write file.\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    fclose(f); // We're done here.
    return new_product;
}
