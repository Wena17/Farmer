#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

typedef struct User
{
    unsigned int id;
    char user_name[32];
    char email[256];
    char pw_hash[256];
    bool is_admin;
    struct User *next;
} User;

int load_users();
int save_users();
int add_user(User *u);

User *check_password(const char *user_name, const char *pw_hash);

#endif // USER_H_INCLUDED
