
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "user.h"

#define filename "users.csv"
#define BUF_SIZE 256

User *users; // This is where we will keep the users.
int users_max_id; // We keep track of the highest ID so we know which one to use next.

int load_users()
{
    FILE *f = fopen(filename, "r");
    if (f == NULL) // Could not open the file.
    {
        users = malloc(sizeof(User)); // Create one user.
        users->id = ++users_max_id; // Increment *before* assigning.
        strcpy(users->user_name, "admin");
        strcpy(users->email, "test@example.org");
        strcpy(users->pw_hash, "xyz");
        users->is_admin = 1;
        users->next = NULL;
        return 1; // Pretend we read one user.
    }
    char buf[BUF_SIZE]; // We'll use this variable to load lines of our file into.
    User *previous = NULL;
    int count = 0; // Let's count the users.
    users_max_id = 0; // Keeping track of the highest id.
    while (true)
    {
        if (fgets(buf, BUF_SIZE, f) == NULL) //  We've reached the end of the file.
            break; // ... so exit the loop.
        User *u = malloc(sizeof(User)); // Allocate memory for one user.
        u->next = NULL; // Initialize link in case this is the last one.
        int rc = sscanf(buf, " %d,%31[^,\n],%255[^,\n],%255[^,\n],%d", &u->id, u->user_name, u->email, u->pw_hash, (int *) &u->is_admin);
        if (rc != 5) // This is an invalid line, skip it.
        {
            free(u); // Free the allocated memory because we're skipping.
            fprintf(stderr, "Skipping invalid line.\n");
            continue; // Loop around.
        }
        if (previous == NULL) // This means we just read the first user.
            users = u; // So remember the start of the list in our global variable.
        else // If it's not the first user...
            previous->next = u; // ... then this is the next for the previous.
        count++; // We've read one more user.
        if (u->id > users_max_id) // If we found a higher id ...
            users_max_id = u->id; // ... then remember it.
        previous = u; // And now the current one becomes the previous one before we loop around.
    }
    fclose(f); // We're done here.
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
