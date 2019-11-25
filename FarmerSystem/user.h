#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

typedef struct user User;

int load_users();
int save_users();

User *check_password(const char *user_name, const char *pw_hash);

#endif // USER_H_INCLUDED
