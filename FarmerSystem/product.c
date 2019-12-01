#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "product.h"

#define filename "products.csv"
#define BUF_SIZE 256

Product *append_product(Product *new_product);

Product *products; // This is where we will keep the users.
int products_max_id = 0; // We keep track of the highest ID so we know which one to use next.

/* Loads all users from the file and returns the number of users read, or -1 if there was an error. */
int load_products()
{
    FILE *f = fopen(filename, "r");
    if (f == NULL) // Could not open the file.
    {
        fprintf(stderr, "WARNING! Could not load product file.\n");
        return 0; // If we can't read any products we just forget about them.
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
        int seller_id;
        int rc = sscanf(buf, " %d,%d,%49[^,\n],%31[^,\n],%d,%d,%255[^,\n]", &u->id, &seller_id, u->product_type, u->product_name, &u->quantity, &u->price, u->location); // Read the various fields from the line in our buf variable.
        if (rc != 7) // The number of fields read is in rc. This should be 5 unless it's somehow an invalid line. If it's invalid, simply skip it.
        {
            free(u); // Free the allocated memory because we're skipping, so we don't run out of memory eventually. It's the opposite of malloc.
            fprintf(stderr, "WARNING! Skipping invalid line '%s'.\n", buf); // Be nice and print a notice.
            continue; // Loop around.
        }
        u->seller = get_user(seller_id);
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
int save_products()
{
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
        int written = fprintf(f, "%d,%d,%s,%s,%d,%d,%s\n", current->id, current->seller->id, current->product_type, current->product_name, current->quantity, current->price, current->location);
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

Product *add_product(User *seller, const char *product_type, const char *product_name, const int quantity, const int price, const char *location)
{
    /* First, we find the last user in the list. */
    Product *last = products;
    while (last != NULL && last->next != NULL)
        last = last->next;
    Product *new_product = malloc(sizeof(Product));
    new_product->id = ++products_max_id; // Assign the next id, increment *before* assignment.
    new_product->seller = seller;
    strcpy(new_product->product_type, product_type); // Copy the product type string from the parameter into the product.
    strcpy(new_product->product_name, product_name); // Same for the product name
    new_product->quantity = quantity; // This is a simple int, we can just assign it.
    new_product->price = price; // Same (we only consider full pesos.
    strcpy(new_product->location, location); // Again a string that we need to copy.
    new_product->next = NULL; // Make sure the list is properly terminated.
    if (last != NULL) // Make sure there is a previous product at all.
        last->next = new_product; // Append to the list.
    else
        products = new_product; // If there was no product before, now this one will be the beginning of our list.
    return append_product(new_product);
}

/* Write a new product to the end of the file. This more robust than writing the complete file every time but of course doesn't work for updates of exisitng products. */
Product *append_product(Product *new_product)
{
    if (new_product == NULL || new_product->seller == NULL)
    {
        fprintf(stderr, "Trying to add NULL product or product with NULL seller.\n");
        return NULL;
    }
    FILE *f = fopen(filename, "a+"); // Here we simply append a line at the end of the file. Note that this doesn't work for changes/updates.
    if (f == NULL)
    {
        fprintf(stderr, "%s:%d Could not open file.\n", __FUNCTION__, __LINE__); // Print a nice error message with function name and line number.
        return NULL;
    }
    int written = fprintf(f, "%d,%d,%s,%s,%d,%d,%s,%d \n", new_product->id, new_product->seller->id, new_product->product_type, new_product->product_name, new_product->quantity, new_product->price, new_product->location);
    if (written < 0 || written >= BUF_SIZE)
    {
        fclose(f); // We don't want dangling open files in case of an error.
        fprintf(stderr, "%s:%d Could not write file.\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    fclose(f); // We're done here.
    return new_product;
}

/* Give us the head of the list of products so we can go through them. */
Product *get_products()
{
    return products;
}

int get_product_count(const User *u)
{
    Product *current = products;
    int count = 0;
    while (current != NULL)
    {
        if (current->seller == u)
            count++;
        current = current->next;
    }
    return count;
}

void delete_product(Product *p)
{
    Product *current = products;
    Product *previous = NULL;
    while (current != NULL)
    {
        if (current == p) // And the right index, we've found the item we are looking for.
        {
            if (previous != NULL) // If there is a previous product...
            {
                previous->next = current->next; // .. then make this one's seccessor the next of the previous.
            }
            else // Otherwise this one is the first product ...
            {
                products = current->next; // ... so move the head of the list to the next one.
            }
            free(current); // Free memory of the deleted product.
            save_products();
            return;
        }
        previous = current;
        current = current->next; // Got to the next product in the list.
    }
    // At this point, we haven't found a product with the right seller and the given index. We just return.
}

void delete_product_from_index(const User *seller, const int index)
{
    Product *current = products;
    Product *previous = NULL;
    int i = 0;
    while (current != NULL)
    {
        if (current->seller == seller) // If this product has the correct seller ...
        {
            if (index == i) // And the right index, we've found the item we are looking for.
            {
                if (previous != NULL) // If there is a previous product...
                {
                    previous->next = current->next; // .. then make this one's seccessor the next of the previous.
                }
                else // Otherwise this one is the first product ...
                {
                    products = current->next; // ... so move the head of the list to the next one.
                }
                free(current); // Free memory of the deleted product.
                save_products();
                return;
            }
            i++; // This product has the right seller but it has the wrong index, so count up.
        }
        previous = current;
        current = current->next; // Got to the next product in the list.
    }
    // At this point, we haven't found a product with the right seller and the given index. We just return.
}

/* Reduces the quantity of the given product be the given amount.
 * If the amount is larger than the rmaining stock return -1, otherwise return the remaining quantity.
 */
int product_reduce_quantity(Product *p, int q)
{
    if (p->quantity < q) return -1;
    p->quantity -= q;
    save_products();
    return p->quantity;
}
