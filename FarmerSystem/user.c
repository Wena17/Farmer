
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "db.h"
#include "sqlite3.h"

typedef struct {
    char *user_name;
    char *email;
    char *pw_hash;
    bool is_admin;
} User;

int check_password(char *user_name, char *pw_hash) {
    /* Prepare a statement to compare the provided pw_hash with the one stored for the user. */
    char sql[] = "SELECT (pw_hash = ?) FROM users WHERE user_name = ?;";
    sqlite3_stmt *stmt = NULL;
    int rc;
    rc = sqlite3_prepare_v2(get_db(), sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK || stmt == NULL) {
        sqlite3_finalize(stmt);
        fprintf(stderr, "check_password 1: DB error %d\n", rc);
        return false; // Check failed.
    }
    /* Now we have a prepared statement that we can bind the parameter values to. */
    rc = sqlite3_bind_text(stmt, 1, pw_hash, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        fprintf(stderr, "check_password 2: DB error %d\n", rc);
        return false; // Check failed.
    }
    rc = sqlite3_bind_text(stmt, 2, user_name, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        fprintf(stderr, "check_password 3: DB error %d\n", rc);
        return false; // Check failed.
    }

    /* Finally, we can execute the statement. */
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        fprintf(stderr, "check_password 4: DB error %d\n", rc);
        return false; // Check failed.
    }

    /* And get the result. */
    int result = sqlite3_column_int(stmt, 0);

    /* Clean up the prepared statement and return the result. */
    sqlite3_finalize(stmt);
    return result;

}
