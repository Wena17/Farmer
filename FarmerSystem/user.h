#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

#include <stdlib.h>

#define MAXLEN 256

typedef struct User
{
    unsigned int id;
    char user_name[MAXLEN];
    char email[MAXLEN];
    char pw_hash[MAXLEN];
    char full_name[MAXLEN];
    char address[MAXLEN];
    char phone[MAXLEN];
    bool is_admin;
    bool is_seller;
    struct User *next;
} User;

int load_users();
int save_users();

User *get_users();
User *add_user(const char *user_name, const char *email, const char *pw_hash, const bool is_seller);
User *get_user(const int id);
void delete_user(User *user);

User *check_password(const char *user_name, const char *pw_hash);

int get_user_count();

User *get_user_by_id(int id);

bool user_has_details(const User *user);

#endif // USER_H_INCLUDED
