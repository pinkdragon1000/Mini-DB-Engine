//Sita Robinson
#include "stdio.h"
#include "db.h"

#define MAX_COMMAND 4096

// directory path for database files
char databasedir[MAX_COMMAND];
char buffer[MAX_COMMAND];

// look to see if s begins with t (case insensitive)
// if it does return a pointer immediately after match
// otherwise return NULL
char *match(char *s, char *t)
{
    while (*s && *t)
    {
        if (tolower(*s) == tolower(*t))
        {
            s++;
            t++;
        }
        else
            return NULL;
    }
    return s;
}

// return a pointer to the first non-space character
char *skipspaces(char *s)
{
    while (*s)
    {
        if (*s == ' ')
        {
            s++;
        }
        else
        {
            return s;
        }
    }
    return s;
}

void process_create(char *c)
{
    c = skipspaces(c);
    //printf("%s\n", c);
    if (!(c = match(c, "table")))
    {
        printf("Error: expected \"table\"\n");
        return;
    }
    c = skipspaces(c);

    // Get table name up until next space
    char table_name[MAX_COMMAND];

    char *s = strstr(c, " ");
    //printf("kkkk%s\n", s);
    if (!s)
    {
        printf("Error: expected \"fields\"\n");
        return;
    }
    else
    {
        char path[MAX_COMMAND];
        strcpy(path,databasedir);
        strncpy(table_name, c, s - c);
        table_name[s - c] = 0;
        strcat(table_name, ".csv");
        strcat(path, table_name);
        strcpy(table_name,path);
        //printf("table_name %s\n", path);
        c = s;
    }

    c = skipspaces(c);

    if (!(c = match(c, "fields")))
    {
        printf("Error: expected \"fields\"\n");
        return;
    }
    c = skipspaces(c);
    //printf("%s\n", c);
    if (!(c = match(c, "[")))
    {
        printf("Error: expected \"[\"\n");
        return;
    }
    c = skipspaces(c);
    char field_list[MAX_COMMAND];

    char *f = strstr(c, "]");
    //printf("kkkk%s\n", s);
    if (!f)
    {
        printf("Error: expected \"]\"\n");
        return;
    }
    else
    {
        strncpy(field_list, c, f - c);
        field_list[f - c] = 0;
        c = f;

        FILE *fcreate = fopen(table_name, "w");

        if (fcreate)
        {
            fprintf(fcreate, "%s\n", field_list);
            fclose(fcreate);
        }
        else
        {
            printf("Error: could not create table\n");
        }
    }
}

void process_drop(char *c)
{
    char file_name[MAX_COMMAND];
    char res[MAX_COMMAND];
    c = skipspaces(c);
    if (!(c = match(c, "table")))
    {
        printf("Error: expected \"table\"\n");
        return;
    }
    c = skipspaces(c);
    strcpy(file_name, c);
    char *pos;
    if ((pos = strchr(file_name, '\n')) != NULL)
        *pos = '\0';

    printf("Do you really want to delete the table %s?  Type yes or no.\n", file_name);
    fgets(res, MAX_COMMAND, stdin);


    char path[MAX_COMMAND];
    strcpy(path,databasedir);
        
    strcat(file_name, ".csv");
    strcat(path, file_name);
    strcpy(file_name,path);

    if (match(res, "yes"))
    {
        printf("Removing %s\n", file_name);
        remove(file_name);
    }
    else
    {
        printf("Drop table cancelled\n");
    }
}
void process_delete(char *c)
{
    c = skipspaces(c);
    if (!(c = match(c, "from")))
    {
        printf("Error: expected \"from\"\n");
        return;
    }
    c = skipspaces(c);

    char table_name[MAX_COMMAND];

    char *s = strstr(c, " ");
    if (!s)
    {
        printf("Error: expected \"where\"\n");
        return;
    }
    else
    {
        char path[MAX_COMMAND];
        strcpy(path,databasedir);
        strncpy(table_name, c, s - c);
        table_name[s - c] = 0;
        strcat(table_name, ".csv");
        strcat(path, table_name);
        strcpy(table_name, path);
        //printf("table_name %s\n", table_name);
        c = s;
    }

    c = skipspaces(c);

    if (!(c = match(c, "where")))
    {
        printf("Error expected \"where\"\n");
        return;
    }
    c = skipspaces(c);

    char newfile[MAX_COMMAND];
    strcpy(newfile, table_name);
    strcat(newfile, ".temp");
    FILE *del = fopen(newfile, "w");

    if (del)
    {
        db *d = db_open(table_name);
        if (d)
        {
            db_match(d, c, 1);
            //print out column names
            for (int h = 0; h < db_get_col_count(d); h++)
            {
                if (h != 0)
                {
                    fprintf(del, ",");
                }
                fprintf(del, "%s", db_get_col_name(d, h));
            }
            fprintf(del, "\n");
            while (db_get_next_row(d))
            {

                for (int x = 0; x < db_get_col_count(d); x++)
                {
                    // printf("Col Name: %s\n", db_get_col_name(d, x));

                    if (x != 0)
                    {
                        fprintf(del, ",");
                    }
                    fprintf(del, "%s", db_get_col(d, x));
                    // printf("Get Column by name: %s\n", db_get_col_by_name(d, "first"));
                }
                fprintf(del, "\n");
            }
            db_close(d);
        }
        else
        {
            printf("Error: could not open file\n");
        }

        fclose(del);
        rename(newfile, table_name);
    }
    else
    {
        printf("Error: temporary file could not be created\n");
    }
}

void process_select(char *c)
{
    c = skipspaces(c);
    if (!(c = match(c, "*")))
    {
        printf("Error: expected \"*\"\n");
        return;
    }
    c = skipspaces(c);
    if (!(c = match(c, "from")))
    {
        printf("Error: expected \"from\"\n");
        return;
    }
    c = skipspaces(c);

    // Get table name up until next space
    char table_name[MAX_COMMAND];

    char *s = strstr(c, " ");
    if (!s)
    {
        printf("Error: expected \"where\"\n");
        return;
    }
    else
    {
        char path[MAX_COMMAND];
        strcpy(path, databasedir);
        strncpy(table_name, c, s - c);
        table_name[s - c] = 0;
        strcat(table_name, ".csv");
        strcat(path, table_name);
        strcpy(table_name, path);
        //printf("table_name %s\n", table_name);
        c = s;
    }

    c = skipspaces(c);
    if (!(c = match(c, "where")))
    {
        printf("Error expected \"where\"\n");
        return;
    }
    c = skipspaces(c);
    // Rest of command is the condition

    db *d = db_open(table_name);
    if (d)
    {
        printf("\n");
        db_print(d, c);
        printf("\n");
        db_close(d);
    }
    else
    {
        printf("Error: could not open file %s\n", table_name);
        return;
    }
}

void process_update(char *c)
{
    c = skipspaces(c);

    char table_name[MAX_COMMAND];

    char *s = strstr(c, " ");
    if (!s)
    {
        printf("Error: expected \"set\"\n");
        return;
    }
    else
    {
        char path[MAX_COMMAND];
        strcpy(path,databasedir);
        strncpy(table_name, c, s-c);
        table_name[s-c] = 0;
        strcat(table_name, ".csv");
        strcat(path, table_name);
        strcpy(table_name, path);
       //printf("table_name %s\n", table_name);
        c = s;

    }

    c = skipspaces(c);
    if (!(c = match(c, "set")))
    {
        printf("Error expected \"set\"\n");
        return;
    }
    c=skipspaces(c);

    char set_field[MAX_COMMAND];
    char set_value[MAX_COMMAND];

char *eq = strstr(c, "=");
    if (!eq)
    {
        printf("Error: expected \"=\"\n");
        return;
    }
    else
    {
        strncpy(set_field, c, eq - c);
        set_field[eq - c] = 0;
        //Pointing to after equals
        c = eq+1;
    }
    
    c=skipspaces(c);
    if(*c !='\"')
    {
        printf("Error: expected '\"'");
        return;
    }
    else
    {
        c++;
    }
    char *closq=strstr(c,"\"");
    if(!closq)
    {
        printf("Error: expected a closing quote\n");
        return;
    }
    else
    {
        strncpy(set_value, c, closq - c);
        set_value[closq - c] = 0;
        c=closq;
        c++;
    }

    c=skipspaces(c);

    if (!(c = match(c, "where")))
    {
        printf("Error expected \"where\"\n");
        return;
    }
    c = skipspaces(c);


    char where_field[MAX_COMMAND];
    char where_value[MAX_COMMAND];

    char *name = strtok(c, "=");
    //printf("name: %s\n", name);
    char *value = strtok(NULL, "=");
    //char value_buffer[MAX_COMMAND];

    strcpy(where_field,name);

    char *t = strstr(value, "\"");
    if (t)
    {
        strcpy(where_value, t + 1);
        char *endquote = strstr(where_value, "\"");
        *endquote = 0;
    }
    else
    {
        strcpy(where_value, value);
    }

    char newfile[MAX_COMMAND];
    strcpy(newfile, table_name);
    strcat(newfile, ".temp");
    FILE *del = fopen(newfile, "w");

    if (del)
    {
        db *d = db_open(table_name);
        if (d)
        {
            int set_col=0;
            db_match(d, NULL, 0);
            //print out column names
            for (int h = 0; h < db_get_col_count(d); h++)
            {
                if (h != 0)
                {
                    fprintf(del, ",");
                }
                fprintf(del, "%s", db_get_col_name(d, h));
                if(!stricmp(set_field, db_get_col_name(d, h)))
                {
                    set_col=h;
                }
            }
            fprintf(del, "\n");
            while (db_get_next_row(d))
            {

                int row_match=(!stricmp(where_value, db_get_col_by_name(d, where_field)));

                for (int x = 0; x < db_get_col_count(d); x++)
                {
                    // printf("Col Name: %s\n", db_get_col_name(d, x));

                    if (x != 0)
                    {
                        fprintf(del, ",");
                    }
                    if(row_match && x==set_col)
                    {
                        fprintf(del, "%s", set_value);
                    }
                    else
                    {
                        fprintf(del, "%s", db_get_col(d, x));
                    }
                    
                    
                    // printf("Get Column by name: %s\n", db_get_col_by_name(d, "first"));
                }
                fprintf(del, "\n");
            }
            db_close(d);
        }
        else
        {
            printf("Error: could not open file\n");
        }

        fclose(del);
        rename(newfile, table_name);
    }
    else
    {
        printf("Error: temporary file could not be created\n");
    }
}


void process_insert(char *c)
{
    c=skipspaces(c);
    //FILE *appendfile = fopen(insertfile, "a");
     if (!(c = match(c, "into")))
    {
        printf("Error expected \"into\"\n");
        return;
    }
    c=skipspaces(c);
    char table_name[MAX_COMMAND];

    char *s = strstr(c, " ");
    
if (!s)
    {
        printf("Error: expected \"(\"\n");
        return;
    }
    else
    {
        char path[MAX_COMMAND];
        strcpy(path, databasedir);
        strncpy(table_name, c, s - c);
        table_name[s - c] = 0;
        strcat(table_name, ".csv");
        strcat(path, table_name);
        strcpy(table_name, path);
        c = s;
    }
    c=skipspaces(c);
    
    if (!(c = match(c, "(")))
    {
        printf("Error: expected \"(\"\n");
        return;
    }

    c = skipspaces(c);
    char field_list[MAX_COMMAND];

    char *f = strstr(c, ")");
    if (!f)
    {
        printf("Error: expected \")\"\n");
        return;
    }
    else
    {
        strncpy(field_list, c, f - c);
        field_list[f - c] = 0;
        c = f;


       // printf("tablename %s\n", table_name);
        db* d=db_open(table_name);
        if(d)
        {
            char ** col_val=(char**)malloc(sizeof(char*)*db_get_col_count(d));
            for(int x=0;x<db_get_col_count(d);x++)
            {
                col_val[x]=NULL;
            }
            char * keyval=strtok(field_list,",");
            while(keyval)
            {
                //printf("%s\n",keyval);
                char key[MAX_COMMAND];
                char value[MAX_COMMAND];
                char* eq=strstr(keyval, "=");
                if(!eq)
                {
                    printf("Error: expected \"equal\" sign\n");
                    return;
                }
                else
                {
                    strncpy(key, keyval, eq-keyval);
                    key[eq-keyval] = 0;
                    eq++;
                    eq = skipspaces(eq);
                    if (*eq == '\"')
                        eq++;
                    strcpy(value,eq);
                    char* pos = strstr(value,"\"");
                    if (pos)
                        *pos = 0;

                    for (int j = 0; j < db_get_col_count(d); j++)
                    {
                        if (!stricmp(key, db_get_col_name(d, j)))
                        {
                            col_val[j] = strdup(value);
                        }
                    }
                }
                
                keyval=strtok(NULL,",");
                
            }


            FILE* f = fopen(table_name, "a");
            if (f)
            {
                for (int z = 0; z < db_get_col_count(d); z++)
                {
                    if (z != 0)
                        fprintf(f, ",");
                    fprintf(f, "%s", col_val[z]?col_val[z]:"");
                }
                fprintf(f, "\n");
                fclose(f);
            }
            else
            {
                printf("Error: could not open file for append\n");
            }

            for (int y = 0; y < db_get_col_count(d); y++)
            {
                if (col_val[y])
                {
                    free(col_val[y]);
                    col_val[y] = NULL;
                }
            }
            free(col_val);

            db_close(d);
        }
        //printf("%s\n",field_list);
        char *name = strtok(c, "=");
        char *value = strtok(NULL, "=");
    }
 
    
    
}

void process(char *c)
{
    char *n;
    c = skipspaces(c);
    if (n = match(c, "select"))
    {
        process_select(n);
    }
    else if (n = match(c, "create"))
    {
        process_create(n);
    }
    else if (n = match(c, "drop"))
    {
        process_drop(n);
    }
    else if (n=match(c, "delete"))
    {
        process_delete(n);
    }
    else if (n=match(c, "update"))
    {
        process_update(n);
    }
    else if (n=match(c, "insert"))
    {
        process_insert(n);
    }
    else if (match(c, "exit"))
    {
        exit(0);
    }
    else
    {
        printf("Unsupported command\n");
    }
}

int main(int argc, char *argv[])
{

    printf("Database Engine by Sita Robinson\n");

    // default directory for database files is the current directory
    strcpy(databasedir, "");
    if (argc >= 2)
    {
        strcpy(databasedir, argv[1]);
        strcat(databasedir, "/");
        mkdir(databasedir, 0777);
    }

    while (1) // loop forever
    {
        printf(">");
        fgets(buffer, MAX_COMMAND, stdin);
        process(buffer);
        //printf("%s\n", buffer);
    }

    return 0;
}