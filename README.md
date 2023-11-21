# Relational Database

## Overview 
This project implements a relational database management solution with interface similar to Structured Query Language (SQL). It is a standalone project that handles the storage, manipulation, searching, and filtering of the data. You can interact with the data in a database including: 

* Creating tables
* Adding, updating, and deleting records 
* Searching, filtering, and selecting records
* Joining different table records

This project implements a parser to process user input as SQL syntax, and return corresponding functionality. All outputs are generated as command line console output with formatted tables and text feedback. 


## Example Usage 
* Creating Database and use database
```
> CREATE DATABASE testing1;
Query OK, 1 row affected (0.01 secs)

> show databases;
+--------------------+
| Database           |
+--------------------+
| foo                |
| bar                |
| testing1           |
+--------------------+
3 rows in set (0.02 sec)  

> use testing1;
Database changed

```

* Table definition and creation
```
> create table users (id int NOT NULL auto_increment primary key, first_name varchar(50) NOT NULL, last_name varchar(50), email varchar(50), zipcode integer);
Query OK, 1 row affected (0.002 sec)

> DESCRIBE users;
+-----------+--------------+------+-----+---------+-----------------------------+
| Field     | Type         | Null | Key | Default | Extra                       |
+-----------+--------------+------+-----+---------+-----------------------------+
| id        | integer      | NO   | YES | NULL    | auto_increment primary key  |
| first_name| varchar(50)  | NO   |     | NULL    |                             |
| last_name | varchar(50)  | YES  |     | NULL    |                             |
| email     | varchar(50)  | YES  |     | NULL    |                             |
| zipcode   | integer      | YES  |     | NULL    |                             |
+-----------+--------------+------+-----+---------+-----------------------------+
5 rows in set (0.000043 sec.)
```
* Record interaction with inserting, selecting, filtering, updating, and deleting.
```
> INSERT INTO users 
  ('first_name', 'last_name', 'email', 'zipcode') 
VALUES 
  ('Gaopo','Huang', 'ghuang@ucsd.edu', '92022'),
  ('Alice', 'Bob', 'alice@bob.com', '12345');
Query OK, 2 rows affected (0.002 sec)

> SELECT first_name, last_name from users;
+--------------------+--------------+
| id  | first_name   | last_name    |
+-----+--------------+--------------+
| 1   | Gaopo        | Huang        |
| 2   | Alice        | Bob          |
+-----+--------------+--------------+
2 rows in set (0.00231 sec)

> SELECT id, first_name, last_name, email from Users where zipcode>90000 order by id LIMIT 2
+--------------------+--------------+----------------+
| id  | first_name   | last_name    | email          |
+-----+--------------+--------------+----------------+
| 1   | Gaopo        | Huang        | ghuang@ucsd.edu|
+-----+--------------+--------------+----------------+
1 rows in set (0.00123 sec)

> UPDATE users SET "zipcode" = 99999 WHERE zipcode>92000;
Query Ok. 1 row affected (0.000087 sec)

> DELETE from users where zipcode<99999;
Query Ok. 1 rows affected (0.000023 sec)
```

* Joining operation demonstration. Suppose that we have two tables, Authors and Books, and we want to join these two tables with matching columns.
```
> select * from Authors;
+----+------------+-----------+
| id | first_name | last_name |
+----+------------+-----------+
|  1 | Stephen    | King      |
|  2 | JK         | Rowling   |
|  3 | Truong     | Nguyen    |
+----+------------+-----------+
3 rows in set (0.00 sec)

> select * from Books;
+----+-------------------------------------------+-----------+
| id | title                                     | author_id |
+----+-------------------------------------------+-----------+
|  1 | Harry Potter and the Sorcerer's Stone     |         2 |
|  2 | Harry Potter and the Philosopher's Stone  |         2 |
|  3 | Harry Potter and the Prisoner of Azkaban  |         2 |
|  4 | Harry Potter and the Chamber of Secrets   |         2 |
|  5 | Harry Potter and the Goblet of Fire       |         2 |
|  6 | Harry Potter and the Order of the Phoenix |         2 |
|  7 | Harry Potter and the Half-Blood Prince    |         2 |
|  8 | Carrie                                    |         1 |
|  9 | The Dark Tower                            |         1 |
| 10 | The Green Mile                            |         1 |
| 11 | Wavelets and Filter Banks                 |         0 |
+----+-------------------------------------------+-----------+
11 rows in set (0.00 sec)```

> select last_name, title from Authors left join Books on Authors.id=Books.author_id;
+-----------+-------------------------------------------+
| last_name | title                                     |
+-----------+-------------------------------------------+
| Rowling   | Harry Potter and the Sorcerer's Stone     |
| Rowling   | Harry Potter and the Philosopher's Stone  |
| Rowling   | Harry Potter and the Prisoner of Azkaban  |
| Rowling   | Harry Potter and the Chamber of Secrets   |
| Rowling   | Harry Potter and the Goblet of Fire       |
| Rowling   | Harry Potter and the Order of the Phoenix |
| Rowling   | Harry Potter and the Half-Blood Prince    |
| King      | Carrie                                    |
| King      | The Dark Tower                            |
| King      | The Green Mile                            |
| Nguyen    | NULL                                      |
+-----------+-------------------------------------------+
11 rows in set (0.00 sec)
```

* Deleting database.
```
> DROP DATABASE testing1
Query OK, 0 rows affected (0.00) 
```

## Implementations
### Source files: 
**Structure and Storage IO**
* ```Database.hpp/cpp```: Encapsulated high-level database with interfaces
* ```Storage.hpp/cpp```: High-level data input/output as "stream-oriented IO" for data stream
* ```BlockIO.hpp/cpp```: Low-level implementation to assign system storages for data records
* ```Index.hpp/cpp```: Provides the storage block index for each chunk of data for faster access
* ```Entity.hpp/cpp```: An in-memory representation of a table definition. 
* ```Attribute.hpp/cpp```: Each attribute represents the property of a single "field" (or entity) of a table, e.g., field type(bool, integer, timestamp)
* ```Row.hpp/cpp```: Each row is a user data entry. Also participate in storage process 

**Database operation** 
* ```Tokenizer.hpp/cpp```: Parse the user input in SQL syntax to validate, tokenize them into recognizable commands, and convert them into queries for processing with the help of ```Keywords, Parser, TokenSequence```.
* ```SQLProcessor.hpp/cpp```: Command handler as a high-level encapsulation for all types of commands including create, delete, add, dump, etc. 
* ```SQLStatement.hpp/cpp```: Encapsulated general statement used to identify operations.
* ```Filter.hpp/cpp```: Helper class to perform conditional logic on data when we are on a select or any update operation.
* ```Join.hpp```: Small helper class to operate data entries for JOIN clause to link multiple related tables using matching column values.

**Tools**
* ```Timer.hpp```: Timer used to time operations 
* ```View.hpp```: with subclasses ```FolderView, TabularView, DumbView, EntityView``` to generate different table views for console output.

# Usage
* Run ```make``` in console to create executables, then run myDatabase executable to make any operations on the data base. 

# Reference 
[UCSD ECE 141B: Software Foundations](https://slidespace.io/courses/7/view)



