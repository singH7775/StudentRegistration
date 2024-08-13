## Project Overview
This is a simple student courses registration system that I have built in the C language using the MySQL database. The main features of this program is a terminal interface which allows a user to register multiple students into the database, while being allowed to perform many operations on those entries.

## Key Features
- slightly improved security with hashing and salting
- adding/deleting classes
- displaying all classes enrolled
- sign in feature/delete account feature
- testing the database for accurate information

## Database Schema
The system uses a relational database with the following tables:
- Majors: Stores available majors
- Students: Student personal information
- Login: Authentication details
- Courses: Available courses for each major
- Enrollments: Many-to-many relationship between students and courses

## Next improvement I will be looking to add
- Creating a better UI for users
- Prevention of SQL Injections


## How to run : 

## Prerequisites
- C compiler (e.g., GCC)
- MySQL server
- MySQL Connector/C library
- IDE or text editor (e.g., Visual Studio Code)
- SQLTools and SQLTools MySQL/MariaDB/TiDB extensions (if using VS Code)

## Setup

1. Install the required software mentioned in Prerequisites.

2. Create the DATABASE for the program to be ran:

- mysql -u root -p
- CREATE DATABASE registered_students; (My DATABASE name is set to registered_students)
- USE registered_students;

3. Configure the database connection in your IDE of choice (I have used VSCODE text editor):

- In your IDE, use tools/settings to connect to a MySQL DATABASE, for example, in VSCODE the tools I use are SQLTools and SQLTools MySQL/MariaDB/TiDB
- Run the .sql file I have provided (MySQL CollegeStudentInfo), this will create the neccessary tables needed to start running this program. (Naming conventions may cause errors, simple name changes can fix this)

4. Update database connection details in the program: Open the file containing your database connection code (main.c) and update the following line with your credentials : if (mysql_real_connect(conn, "localhost", "your_username", "your_password", "registered_students", 0, NULL, 0) == NULL)


## Compilation

Compile the program using:
gcc -o student_registration main.c functions.c -lmysqlclient

Execute the compiled program:
./student_registration


## Troubleshooting

- If you encounter a "library not found" error, ensure MySQL Connector/C is properly installed and linked in your program.
- For connection issues, verify your database credentials and that the MySQL server is running.
- If you encounter naming convention errors, check table and column names in both your SQL file and C code for consistency.
- Make sure the database name in your connection string matches the one you created ("registered_students" in this case).
- If using a different IDE or setup, adjust the database connection instructions accordingly.
- In my program I use random naming conventions to connect to my DATABASE, yours could be different 
   if (mysql_real_connect(conn, "localhost", "the-programmer", "fortnite", "registered_students", 0, NULL, 0) == NULL)
   {  
    sql_error(conn);
   } 

Contact : harsingh10000@gmail.com / discord : dark691400