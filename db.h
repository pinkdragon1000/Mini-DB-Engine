//Sita Robinson
#ifndef _DB_H
#define _DB_H
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>


typedef struct db
{
    FILE *fp;
    int col_count;
    char** col_names;
    char** col_values;
    char* match_name;
    char* match_value;
    int match_reverse;

} db;

db* db_open(char * filename);
void db_close(db* d);

void db_match(db* d, char* query, int reverse);

int db_get_next_row(db* d);

int db_get_col_count(db* d);
char* db_get_col_name(db *d, int n);
char* db_get_col(db *d, int n);
char* db_get_col_by_name(db *d, char* name);


void db_print(db *d, char* query);

int stricmp(char const *a, char const *b);

#endif //_DB_H