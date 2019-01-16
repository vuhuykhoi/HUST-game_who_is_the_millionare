#include <sqlite3.h>

#define DATABASE_NAME "user.db"
#define MAX_STRING_LENGTH_SQL 500

/**
 * static int on_search_user(void* result, int num_cols, char** row, char** cols)
 */
typedef sqlite3_callback DatabaseCallback;

sqlite3** open_database();
void close_database(sqlite3** db);
int insert_query(char* _sql);
void select_query(char* sql, DatabaseCallback handler_per_row, void* result);
