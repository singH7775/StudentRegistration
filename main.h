#ifndef MAIN_H
#define MAIN_H

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define QUERY_SIZE 300
#define MAX_PASSWORD_LENGTH 100
#define MAX_SALTED_PASSWORD_LENGTH (MAX_PASSWORD_LENGTH + 10)

// Student functions
void register_student(MYSQL *conn);
void update_student(MYSQL *conn, int student_id); // Provides options for what to do with account
const char *check_code(MYSQL *conn, const char *class_selected, int major);
char* get_major_name_from_db(MYSQL *conn, int major_id);
int get_student_major(MYSQL *conn, int student_id);
int max_students_reached(MYSQL *conn);

// Class functions
void register_class(MYSQL *conn, int student_id, int major);
void remove_class(MYSQL *conn, int student_id);
void display_classes(MYSQL *conn, int student_id, int major);
int is_class_registered(MYSQL *conn, int student_id, const char *course_code);
int max_classes_reached(MYSQL *conn, int student_id);
char* get_student_classes(MYSQL *conn, int student_id);

// Login and Delete
int is_username_taken(MYSQL *conn, const char* username);
int validate_login(MYSQL *conn, const char* username, const char* password, int *student_id);
void delete_account(MYSQL *conn, int student_id);

// SQL Functions
void sql_error(MYSQL *conn);
MYSQL_RES* execute_query(MYSQL *conn, const char *query);
void test_db_info(MYSQL *conn, int student_id);
void system_pauser();

#endif