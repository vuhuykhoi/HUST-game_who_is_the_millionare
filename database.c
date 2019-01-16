#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>


#define MAX_STRING_LENGTH_SQL 500

// open a connection to database
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