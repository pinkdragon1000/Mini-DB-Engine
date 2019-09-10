//Sita Robinson
#include "db.h"

int main(int argc, char* argv[])
{
    db* d;
    d=db_open("tablename.csv");
    if(d)
    {
        printf("Dumping all rows\n");
        db_match(d,NULL,0);
        while(db_get_next_row(d))
        {
            for(int x=0;x<db_get_col_count(d);x++)
            {
                printf("Col Name: %s\n",db_get_col_name(d, x));
                printf("Col Value: %s\n", db_get_col(d, x));
                printf("Get Column by name: %s\n", db_get_col_by_name(d,"first"));
            }
        }
    
        printf("Rows that match value\n");

        char query[255];
        strcpy(query,"gender=\"M\"");
        db_print(d, NULL);
        /*
        db_match(d,query);
        while(db_get_next_row(d))
        {
            for(int x=0;x<db_get_col_count(d);x++)
            {
                printf("Col Name: %s\n",db_get_col_name(d, x));
                printf("Col Value: %s\n", db_get_col(d, x));
                printf("Get Column by name: %s\n", db_get_col_by_name(d,"first"));
            }
        }
        */
        db_close(d);
    }
    else
    {
        printf("Error Opening db\n");
    }
    
}