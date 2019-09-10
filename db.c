//Sita Robinson
#include "db.h"

#define MAX_ROW_LENGTH 4096

int stricmp(char const *a, char const *b)
{
    for (;; a++, b++)
    {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

db *db_open(char *filename)
{
    db *d = NULL;
    FILE *fp;
    char buffer[MAX_ROW_LENGTH];
    fp = fopen(filename, "r");
    if (fp)
    {
        d = (db *)malloc(sizeof(db));
        if (d)
        {
            d->fp = fp;
            d->col_count = 0;
            d->col_names = NULL;
            d->col_values = NULL;
            d->match_name = NULL;
            d->match_value = NULL;
            fgets(buffer, MAX_ROW_LENGTH, d->fp);
            //Removing newline from buffer
            char *pos;
            if ((pos = strchr(buffer, '\n')) != NULL)
                *pos = '\0';
            //printf("%s\n", buffer);

            for (int i = 0; i < strlen(buffer); i++)
            {
                if (buffer[i] == ',')
                    d->col_count++;
            }
            d->col_count++;
            //printf("Col Count: %d\n", d->col_count);

            d->col_names = (char **)malloc(d->col_count * sizeof(char *));
            for (int x = 0; x < d->col_count; x++)
            {
                d->col_names[x] = NULL;
            }
            d->col_values = (char **)malloc(d->col_count * sizeof(char *));
            for (int x = 0; x < d->col_count; x++)
            {
                d->col_values[x] = NULL;
            }
            char *h = strtok(buffer, ",");
            int x = 0;
            while (h)
            {
                //printf("%s\n", h);
                d->col_names[x] = strdup(h);
                h = strtok(NULL, ",");
                x++;
            }
            //printf("Col Names %s\n", d->col_names[0]);
        }
    }
    return d;
}

void db_close(db *d)
{
}

//Function to move past header and ready to read rows
void db_match(db *d, char *query, int reverse)
{
    d->match_reverse=reverse;
    // Free any old match data
    if (d->match_name != NULL)
    {
        free(d->match_name);
        d->match_name = NULL;
    }
    if (d->match_value != NULL)
    {
        free(d->match_value);
        d->match_value = NULL;
    }
    char buffer[MAX_ROW_LENGTH];
    //Seek to beginning of file
    fseek(d->fp, 0, SEEK_SET);
    //Skip the header
    fgets(buffer, MAX_ROW_LENGTH, d->fp);
    if (query != NULL)
    {
        char *name = strtok(query, "=");
        //printf("name: %s\n", name);
        char *value = strtok(NULL, "=");
        char value_buffer[MAX_ROW_LENGTH];

        char *t = strstr(value, "\"");
        if (t)
        {
            strcpy(value_buffer, t + 1);
            char *endquote = strstr(value_buffer, "\"");
            *endquote = 0;
        }
        else
        {
            strcpy(value_buffer, value);
        }

        // printf("value: %s\n", value_buffer);
        d->match_name = strdup(name);
        d->match_value = strdup(value_buffer);
    }
}

int db_get_next_row(db *d)
{
    //Release memory for previously read values
    for (int x = 0; x < d->col_count; x++)
    {
        if (d->col_values[x] != NULL)
        {
            free(d->col_values[x]);
            d->col_values[x] = NULL;
        }
    }

    int done = 0;
    while (!done)
    {
        char buffer[MAX_ROW_LENGTH];
        char *res = fgets(buffer, MAX_ROW_LENGTH, d->fp);

        if (res != NULL)
        {
            //Removing newline from buffer
            char *pos;
            if ((pos = strchr(buffer, '\n')) != NULL)
                *pos = '\0';

            char *f = strtok(buffer, ",");
            int i = 0;
            while (f)
            {
                if (i >= d->col_count)
                {
                    printf("Too many values in row\n");
                    break;
                }
                d->col_values[i] = strdup(f);
                f = strtok(NULL, ",");
                i++;
            }

            if (d->match_name && d->match_value)
            {
                if ((strcmp(d->match_value, db_get_col_by_name(d, d->match_name))!=0) == d->match_reverse)
                {
                    return 1;
                }
            }
            else
            {
                return 1;
            }
        }
        else
        {
            done = 1;
        }
    }

    return 0;
}

int db_get_col_count(db *d)
{
    return d->col_count;
}

char *db_get_col_name(db *d, int n)
{
    if (n < 0 || n >= d->col_count)
    {
        printf("Error.  Non-existant column\n");
        return "";
    }
    return d->col_names[n];
}

char *db_get_col(db *d, int n)
{
    if (n < 0 || n >= d->col_count)
    {
        printf("Error.  Non-existant column\n");
        return "";
    }
    return d->col_values[n];
}

char *db_get_col_by_name(db *d, char *name)
{
    for (int x = 0; x < d->col_count; x++)
    {
        if (!stricmp(name, d->col_names[x]))
        {
            return db_get_col(d, x);
        }
    }
    printf("Error.  Column %s not found\n", name);
}

void db_print(db *d, char *query)
{
    db_match(d, query, 0);
    //print out column names
    for (int h = 0; h < db_get_col_count(d); h++)
    {
        if (h != 0)
        {
            printf(" | ");
        }
        printf("%s", db_get_col_name(d, h));
    }
    printf("\n");
    while (db_get_next_row(d))
    {

        for (int x = 0; x < db_get_col_count(d); x++)
        {
            // printf("Col Name: %s\n", db_get_col_name(d, x));
            
            if (x != 0)
            {
                printf(" | ");
            }
            printf("%s", db_get_col(d, x));
            // printf("Get Column by name: %s\n", db_get_col_by_name(d, "first"));
        }
        printf("\n");
    }
}
