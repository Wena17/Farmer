
#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"
#include "db.h"

void create_db();
int begin_tx();
int commit_tx();
int sql_exec(char *sql);

sqlite3 *db; // This is a handle to the database. It's global because we'll need it everywhere and there is only one.

void init_db()
{
    int rc = sqlite3_open("db.sqlite3", &db); // Open (or create) the database file.
    if (rc)
    {
        printf("%s", "Could not open database; quitting.");
        exit(1);
    }
    char *err_msg;
    rc = sqlite3_exec(db, "SELECT version FROM farmer_version LIMIT 1;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        sqlite3_free(&err_msg);
        create_db();
    }
}

void close_db()
{
    sqlite3_close(db);
}

void create_db()
{
    printf("Need to create database schema.\n");
    if (sql_exec("CREATE TABLE IF NOT EXISTS farmer_version (id INTEGER PRIMARY KEY AUTOINCREMENT, version INTEGER NOT NULL);"));
    {
        printf("%s\n", "Could not create table farmer_version; quitting.");
        exit(1);
    }
    if (begin_tx() == SQLITE_OK)
    {
        int rc = sql_exec("INSERT INTO farmer_version (version) VALUES (0);");
        if (rc || !commit_tx())
        {
            printf("%s", "Quitting\n");
            exit(1);
        }
    }
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
