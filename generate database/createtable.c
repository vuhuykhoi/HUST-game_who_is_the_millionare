#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}



int main(int argc, char* argv[])
{
   sqlite3 *db;
   char *zErrMsg = 0;
   char *zErrMsg1= 0;
   int  rc;
   int  rc1;
   char *sql;
   char *sql1;

   /* Open database */
   rc = sqlite3_open("user.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stdout, "Opened database successfully\n");
   }

   /* Create SQL statement */
   sql = "CREATE TABLE ACCOUNT("  \
         "USERNAME TEXT PRIMARY KEY NOT NULL," \
         "PASSWORD TEXT NOT NULL );";
   
   sql1 = "CREATE TABLE USERSCORE("  \
          "ID INT PRIMARY KEY," \
          "NAME TEXT NOT NULL," \
          "SCORE INT NOT NULL," \
          "TIME TEXT NOT NULL );";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   rc1= sqlite3_exec(db, sql1, callback, 0, &zErrMsg1);
   if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Table account created successfully\n");
   }
   if( rc1 != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg1);
      sqlite3_free(zErrMsg1);
   }else{
      fprintf(stdout, "Table userscore created successfully\n");
   }   
   sqlite3_close(db);
   return 0;
}
