# Mini-DB-Engine

* Compiler used was gcc. To build the project run `make` and then execute `./database DATABASENAME` or type `./database` to create table in current directory.
* The database is case insensitive except for the column names and values
* I chose CSV (Comma Separated Values) as my database file format
* The join command was not implemented

Sample Program: 
```
Database Engine by Sita Robinson
> create table contacts fields [First,Last,Email,Gender]
> insert into contacts (First="Sita",Last="Robinson",Email="sita.robinson@gmail.com",Gender="F")
> insert into contacts (Last="Robinson",First="Padma",Email="padma.robinson@gmail.com",Gender="F")
> insert into contacts (First="John",Last="Robinson",Email="johnrobinsn@gmail.com",Gender="M")
> insert into contacts (First="Jim",Last="Robinson",Email="jimr@gmail.com",Gender="M")
> update contacts set First="Sita" where Last="Robinson"
> select * from contacts where Gender="F"
> drop table contacts
```
