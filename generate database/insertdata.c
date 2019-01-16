#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#define DATABASE_NAME "user.db"
#define MAX_STRING_LENGTH_SQL 500

typedef sqlite3_callback DatabaseCallback;

sqlite3** open_database();
void close_database(sqlite3** db);
int insert_query(char* _sql);
void select_query(char* sql, DatabaseCallback handler_per_row, void* result);


static int on_get_row(void* NotUsed, int num_cols, char** row, char** cols) {
	printf("%s-%s\n", row[0], row[1]);
	return 0;
}

static int on_get_score(void* result, int num_cols, char** row, char** cols) {
	//printf("%s-%s\n", row[0], row[1]);
	char*score =(char*)result;
//	strcat(score,row[0]);
//	strcat(score,"  ");
	strcat(score,row[1]);
	strcat(score,"  ");
	strcat(score,row[2]);
	strcat(score,"  ");
	strcat(score,row[3]);
	strcat(score,"\n");
	return 0;
}

void insert_a_user() {
	// insert a user
	char sql[] = "INSERT INTO " \
					"account(username, password) VALUES " \
					"('quocanh', '123')";
	if(insert_query(sql)) {
		printf("TEST_DATABASE: Success\n->\"%s\"\n", sql);
	} else {
		printf("TEST_DATABASE: Failed\n->\"%s\"\n", sql);
	}
}

void insert_a_score() {
	sqlite3** db = open_database();	
	sqlite3_int64 lastrow = sqlite3_last_insert_rowid(*db);
	printf("%d\n",(int)lastrow);
	close_database(db);
	char sql[] = "INSERT INTO " \
					"userscore(id, name, score, time) VALUES " \
					"(1,'abcd',200, '1234')";
	if(insert_query(sql)) {
		printf("TEST_DATABASE: Success\n->\"%s\"\n", sql);
	} else {
		printf("TEST_DATABASE: Failed\n->\"%s\"\n", sql);
	}	
}

int main(int argc, char const *argv[])
{
	char scoreinfo[100]="";
	sqlite3** db = open_database();
	if(db != NULL) {
		printf("db != NULL\n");
	} else {
		printf("db == NULL\n");
	}

//	insert_a_user();
	insert_a_score();

	// get user
	char sql_get_user[] = "SELECT * FROM userscore ORDER BY score DESC;";
	select_query(sql_get_user, on_get_score, &scoreinfo);
	printf("%s\n",scoreinfo);

	close_database(db);
	return 0;
}

sqlite3** open_database() {
	sqlite3** db = (sqlite3**)malloc(sizeof(sqlite3*));
	int isNotOk = sqlite3_open(DATABASE_NAME, db);
	//printf("sqlite3_open() == %d\n", isOk);
	if(isNotOk) {
		return NULL;
	}
	return db;
}

// close database
void close_database(sqlite3** db) {
	sqlite3_close(*db);
}

// execute a insert query, return 0 if failed
int insert_query(char* _sql) {
	char* sql = (char*)malloc(MAX_STRING_LENGTH_SQL);
	strcpy(sql, _sql);
	sqlite3** db = open_database();

	// check if opening failed
	if(db == NULL) {
		// printf("DATABASE: Failed\n");
		// printf("INSERT_QUERY: open_database() is failed\n");
		return 0;
	}

	// if opening database is successful
	// then execute query sql
	char* error_message = 0;
	int isOk = sqlite3_exec(*db, sql, NULL, 0, &error_message);
	// if insert faild then return 0
	if(isOk != SQLITE_OK) {
		// printf("DATABASE: Failed\n");
		// printf("\tinsert_query\n");
		// printf("\t\tsqlite3_exec(): %s\n", sql);
		// printf("\t\terror_message: %s\n", error_message);
		sqlite3_free(error_message);
		close_database(db);
		return 0;
	}

	// if insert success then return 1
	close_database(db);
	return 1;
}

// get data from database by select query
void select_query(char* sql, DatabaseCallback handler_per_row, void* result) {
	sqlite3** db = open_database();

	// check if opening failed
	if(db == NULL) {
		// printf("DATABASE: Failed\n");
		// printf("SELECT_QUERY: open_database() is failed\n");
		return;
	}

	char* error_message = 0;
	int isOk = sqlite3_exec(*db, sql, handler_per_row, result, &error_message);
	if(isOk != SQLITE_OK) {
		// printf("DATABASE: Failed\n");
		// printf(" ->select_query()\n");
		// printf("  ->sqlite3_exec(): \"%s\"\n", sql);
		// printf("  ->error: %s\n", error_message);
		close_database(db);
		return;
	}

	close_database(db);
}
