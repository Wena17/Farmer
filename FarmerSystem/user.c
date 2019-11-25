
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "user.h"

#define filename "users.csv"
#define BUF_SIZE 256

User *users; // This is where we will keep the users.
int users_max_id = 0; // We keep track of the highest ID so we know which one to use next.

/* Loads all users from the file and returns the number of users read, or -1 if there was an error. */
int load_users()
{
    FILE *f = fopen(filename, "r");
    if (f == NULL) // Could not open the file.
    {
        /* Opening the file did not work, so we create the admin user. */
        users = malloc(sizeof(User)); // Create one user. In C, you always have to allocate memory before you can write to it. That's what malloc does.
        users->id = ++users_max_id; // Increment *before* assigning. Ever user gets a unique ID number.
        strcpy(users->user_name, "admin"); // Here I'm simply copying the strings to the new user data structure.
        strcpy(users->email, "test@example.org"); // Same for the email.
        strcpy(users->pw_hash, "xyz"); // And finally the (unencrypted) password.
        users->is_admin = 1; // The first user is the admin. Everybody else will be admin=0.
        users->next = NULL; // This is a pointer to the next user, creating a linked list of users. There is only one user here, so the "next" user is NULL.
        return 1; // Pretend we read one user.
    }
    char buf[BUF_SIZE]; // We'll use this variable to load lines of our file into. C is not very convenient when it comes to strings, you have to do a lot of stuff by hand.
    User *previous = NULL; // We keep track of the previous user we've read so we can set its "next" pointer later. This will become clearer further down.
    int count = 0; // Let's count the users.
    users_max_id = 0; // Keeping track of the highest id.
    while (true) // Endless loop.
    {
        if (fgets(buf, BUF_SIZE, f) == NULL) //  We read the next line from the file into our buf variable. If we get NULL, we've reached the end of the file.
            break; // ... so exit the loop.
        User *u = malloc(sizeof(User)); // Allocate memory for one user. Again, we need to do the memory management by hand because we don't know the number of users in advance and C has no resizable arrays.
        u->next = NULL; // Initialize link in case this is the last one.
        int rc = sscanf(buf, " %d,%31[^,\n],%255[^,\n],%255[^,\n],%d", &u->id, u->user_name, u->email, u->pw_hash, (int *) &u->is_admin); // Read the various fields from the line in our buf variable.
        if (rc != 5) // The number of fields read is in rc. This should be 5 unless it's somehow an invalid line. If it's invalid, simply skip it.
        {
            free(u); // Free the allocated memory because we're skipping, so we don't run out of memory eventually. It's the opposite of malloc.
            fprintf(stderr, "Skipping invalid line.\n"); // Be nice and print a notice.
            continue; // Loop around.
        }
        /* At this point, we've read the fields of one line into the User in variable u. */
        if (previous == NULL) // This means we just read the first user.
            users = u; // So remember the start of the list in our global variable. Otherwise we'll never find it again.
        else // If it's not the first user...
            previous->next = u; // ... then this is the next of the previous. Does your
        count++; // We've read one more user. Increase the count of users by one.
        if (u->id > users_max_id) // If we found a higher id ...
            users_max_id = u->id; // ... then remember it. This is only necessary if we delete users later and the numbers are not strictly ascending anymore. But hey, let's be safe.
        previous = u; // And now the current one becomes the previous one before we loop around. Yay, we're done with one user.
    }
    fclose(f); // We're done here. File is read completely. We get here only through the break statement further up.
    return count; // Return the number of users, just to be nice.
}

int save_users() {
    FILE *f = fopen(filename, "w+");
    if (f == NULL)
    {
        fprintf(stderr, "%s:%d Could not open file.\n", __FUNCTION__, __LINE__); // Print a nice error message with function name and line number.
        return -1;
    }
    User *u = users; // We start with the head of the list.
    int count = 0; // Let's count the users.
    while (u != NULL) // while we have more, we loop.
    {
        int written = fprintf(f, "%d,%s,%s,%s,%d\n", u->id, u->user_name, u->email, u->pw_hash, u->is_admin);
        if (written < 0 || written >= BUF_SIZE)
        {
            fclose(f); // We don't want dangling open files in case of an error.
            fprintf(stderr, "%s:%d Could not write file.\n", __FUNCTION__, __LINE__);
            return -1;
        }
        count++;
        u = u->next;
    }
    fclose(f); // We're done here.
    return count;
}

User *check_password(const char *user_name, const char *pw_hash)
{
    User *u = users; // Start search at the head of the list.
    while (u != NULL) // While we have more...
    {
        if (strcmp(u->user_name, user_name) == 0 && strcmp(u->pw_hash, pw_hash) == 0) // Name and password match.
        {
            return u; // Return this user.
        }
        u = u->next; // Else just continue with the next one.
    }
    return NULL; // We're at the end of the list and haven't found a match.
}

int add_user(User *u)
{
    /* First, we find the last user in the list. */
    User *last = users;
    while (last->next != NULL)
        last = last->next;
    u->id = ++users_max_id; // Assign the next id, increment *before* assignment.
    last->next = u; // Append to the list.
    u->next = NULL; // Make sure the list is properly terminated.
    save_users(); // Update the file.
    return u->id;
}
