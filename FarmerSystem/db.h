#ifndef DB_H_INCLUDED
#define DB_H_INCLUDED

#include "sqlite3.h"

void init_db();
void close_db();
sqlite3 *get_db();

#endif // DB_H_INCLUDED
