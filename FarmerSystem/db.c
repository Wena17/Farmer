
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"
#include "db.h"

char schema_sql[] =
    "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, user_name VARCHAR(255), email VARCHAR(255) NOT NULL, pw_hash VARCHAR(255));"
    "CREATE UNIQUE INDEX IF NOT EXISTS users_email ON users (email);"
    "BEGIN;"
    "INSERT INTO users (user_name, email, pw_hash) VALUES ('admin', 'test@example.org', 'xyz') ON CONFLICT DO NOTHING;"
    "COMMIT;"
    ;

sqlite3 *db; // This is a handle to the database. It's global because we'll need it everywhere and there is only one.

void init_db()
{
    int rc;
    rc = sqlite3_open("db.sqlite3", &db); // Open (or create) the database file.
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Could not open database, code %d. Quitting.\n", rc);
        exit(1);
    }
    char *err_msg;
    rc = sqlite3_exec(db, schema_sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(&err_msg);
        fprintf(stderr, "Code %d. Quitting.", rc);
    }
}

void close_db()
{
    sqlite3_close(db);
}

int sql_exec(char *sql)
{
    char *err_msg;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    if (rc)
    {
        fprintf(stderr, "%s\n", err_msg);
        sqlite3_free(err_msg);
    }
    return rc;
}

int begin_tx()
{
    char *err_msg;
    int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "%s", err_msg);
        sqlite3_free(err_msg);
    }
    return rc;
}

int commit_tx()
{
    char *err_msg;
    int rc = sqlite3_exec(db, "COMMIT;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "%s", err_msg);
        sqlite3_free(err_msg);
    }
    else
    {
        rc = sqlite3_exec(db, "ROLLBACK;", NULL, NULL, &err_msg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "%s", "Rollback failed, quitting.\n");
            fprintf(stderr, "%s", err_msg);
            exit(2);
        }
    }
    return rc == 0;
}
