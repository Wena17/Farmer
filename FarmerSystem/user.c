
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define filename "data/users.csv"

typedef struct User
{
    unsigned int id;
    char user_name[32];
    char email[256];
    char pw_hash[256];
    bool is_admin;
    struct User *next;
} User;

User *users; // This is where we will keep the users.
int users_max_id; // We keep track of the highest ID so we know which one to use next.

int load_users()
{
    const FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        fprintf(stderr, "%s:%d Could not open file.\n", __FUNCTION__, __LINE__); // Print a nice error message with function name and line number.
        return -1;
    }
    char buf[256]; // We'll use this variable to load lines of our file into.
    User *previous = NULL;
    int count = 0; // Let's count the users.
    users_max_id = 0; // Keeping track of the highest id.
    while (true)
    {
        if (fgets(&buf, 256, f) == NULL) // Read next line and check if end of file reached.
            break; // File end, we're done.
        User *u = malloc(sizeof(User));
        u->next = NULL; // Initialize link in case this is the last one.
        int rc = sscanf(buf, "%d,\"%31s\",\"%255s\",\"%255s\",%d\n", u->id, u->user_name, u->email, u->pw_hash, u->is_admin);
        if (rc != 5)
        {
            fclose(f); // We don't want dangling open files in case of an error.
            fprintf(stderr, "%s:%d Could not parse file.\n", __FUNCTION__, __LINE__);
            return -1;
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
    const FILE *f = fopen(filename, "w+");
    if (f == NULL)
    {
        fprintf(stderr, "%s:%d Could not open file.\n", __FUNCTION__, __LINE__); // Print a nice error message with function name and line number.
        return -1;
    }
    User *u = users; // We start with the head of the list;
    while (u != NULL) // while we have more, we loop.
    {
        int rc = fprintf(f, "%d,\"%31s\",\"%255s\",\"%255s\",%d\n", u->id, u->user_name, u->email, u->pw_hash, u->is_admin);
        if (rc != 5)
        {
            fclose(f); // We don't want dangling open files in case of an error.
            fprintf(stderr, "%s:%d Could not write file.\n", __FUNCTION__, __LINE__);
            return -1;
        }
        u = u->next;
    }
    fclose(f); // We're done here.
}

User *check_password(char *user_name, char *pw_hash)
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
