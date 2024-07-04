#include "main.h"

void register_student(MYSQL *conn) 
{ 
    char username[50], password[50], first_name[50], last_name[50];
    int student_id;

    // Check if username is taken
    do {
        printf("Enter a username: ");
        scanf("%s", username);
        if (is_username_taken(conn, username))
            printf("Username is already taken. Please choose another.\n");
    } while (is_username_taken(conn, username));

    do {
    printf("Enter a password: ");
    scanf("%s", password);
    if (strlen(password) < 10 || strchr(password, ' ') != NULL)
    {
        fflush(stdin);
        printf("Please make your password atleast 10 characters and have no spaces!\n");
    }
    } while(strlen(password) < 10 || strchr(password, ' ') != NULL);

    // Student Information
    printf("First name: ");
    scanf("%s", first_name);
    printf("Last name: ");
    scanf("%s", last_name);

    system("cls");

    printf("\nWelcome %s %s\nPlease choose your major below\n\n", first_name, last_name);
    printf("1.Computer science\n2.Economics\n3.Graphic design\n4.Marine biology\n5.Philosophy\nChoice: ");

    int major;
    while(1)
    {
        scanf("%d", &major);
        if (major >= 1 && major <= 5) break;
        printf("Please select a valid major!\nChoice: ");
    }

    // Inserting in student table
    char query[100];
    sprintf(query, "INSERT INTO Students (first_name, last_name, major_id) VALUES('%s', '%s', %d)",
            first_name, last_name, major);

    execute_query(conn, query);

    if (mysql_affected_rows(conn) > 0) // Row was inserted
    {
        printf("Student added successfully.\n");
        student_id = mysql_insert_id(conn); // Retrieve the student id from students table

        // Login table
        sprintf(query, "INSERT INTO Login (student_id, username, password) VALUES (%d, '%s', '%s')",
                        student_id, username, password);

        execute_query(conn, query);

        if (mysql_affected_rows(conn) > 0)
        {
            printf("Login information added successfully.\n");
        } else {
            printf("Failed to add login information. Removing student entry. Please try again.\n");
            sprintf(query, "DELETE FROM Students WHERE student_id = %d", student_id);
            execute_query(conn, query);
            return;
        }
    } else {
        printf("Failed to add student. Please try again\n");
        return;
    }

    // Notify user of success
    getchar();
    printf("\nPress Enter to continue to registration of class...");
    while(getchar() != '\n');

    // Class operations
    system("cls");
    display_classes(conn, student_id, major);
    register_class(conn, student_id, major);
    system("cls");

    char *major_name = get_major_name_from_db(conn, major);
    char *student_classes = get_student_classes(conn, student_id);
    if (major_name && student_classes)
    {
        printf("Registered student: %s %s\nMajor: %s\nClasses signed up: %s\n\n", first_name, last_name, 
        major_name, student_classes);
    } else {
        printf("Failed to display student information.\n");
    }
    free(major_name);
    free(student_classes);
}

void update_student(MYSQL *conn, int student_id)
{
    int choice;
    do {
        system("cls");
        printf("What would you like to do?\n1.Add a Class\n2.Remove a class\n3.Display classes enrolled\n4.Delete account\n5.Exit\nEnter: ");
        scanf("%d", &choice);
        system("cls");
        switch(choice)
        {
            case 1:
                int major = get_student_major(conn, student_id);
                display_classes(conn, student_id, major);
                register_class(conn, student_id, major);
                break;
            case 2:
                remove_class(conn, student_id);
                break;
            case 3:
                char *classes = get_student_classes(conn, student_id);
                if (classes)
                {
                    printf("Classes currently enrolled: %s\n\n", classes);
                    free(classes);
                } else {
                    printf("Failed to retrieve the student classes.\n");
                }
                break;
            case 4:
                system("cls");
                int delete;
                printf("Are you sure?\n1.Yes\n2.Exit\n");
                scanf("%d", &delete);
                if (delete == 1)
                {
                    delete_account(conn, student_id);
                    return;
                }
                break;
            case 5:
                printf("Exiting...\n");
                break;
            default:    
                printf("Invalid option. Please try again.\n");
        }
    }while(choice != 5);
}

const char *check_code(MYSQL *conn, const char *class_selected, int major)
{
    char query[200];
    sprintf(query, "SELECT course_name, course_code FROM Courses "
                    "WHERE major_id = %d AND course_code = '%s'", major, class_selected);
    
    MYSQL_RES *result = execute_query(conn, query);

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) // Success
    {
        char *course_name = strdup(row[0]);
        if (!course_name) {
            printf("Memory allocation failed!\n");
            mysql_free_result(result);
            return NULL;
        }
        mysql_free_result(result);
        return course_name;
    }

    mysql_free_result(result);
    return NULL;
}

char* get_major_name_from_db(MYSQL *conn, int major_id)
{
    char *major_name;
    char query[100];

    sprintf(query, "SELECT major_name FROM Majors WHERE major_id = %d", major_id);

    MYSQL_RES *result = execute_query(conn, query);

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0])
    {
        major_name = strdup(row[0]);
    } else {
        major_name = strdup("Unknown Major");
    }

    if (!major_name)
    {
        printf("Memory allocation failed!\n");
        mysql_free_result(result);
        return NULL;
    }

    mysql_free_result(result);
    return major_name;
}

int get_student_major(MYSQL *conn, int student_id)
{
    char query[100];
    sprintf(query, "SELECT major_id FROM Students WHERE student_id = %d", student_id);

    MYSQL_RES *result = execute_query(conn, query);

    MYSQL_ROW row = mysql_fetch_row(result);
    int major_id = row ? atoi(row[0]) : 0;

    mysql_free_result(result);
    return major_id;
}

int max_students_reached(MYSQL *conn)
{
    int count = 0;
    char query[100];

    sprintf(query, "SELECT COUNT(*) FROM Students");
    MYSQL_RES *result = execute_query(conn, query);

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row)
        count = atoi(row[0]);

    mysql_free_result(result);
    return count >= 100;
}

void register_class(MYSQL *conn, int student_id, int major)
{
    int valid_class = 1;
    while (valid_class && !max_classes_reached(conn, student_id)) 
    {
        char class_selected[10];
        printf("Enter Class Code: ");
        scanf("%s", class_selected);
        const char *class_name = check_code(conn, class_selected, major); // Also return class name

        if (class_name != NULL)
        {
            if (!is_class_registered(conn, student_id, class_selected))
            {
                char query[100];
                sprintf(query, "INSERT INTO Enrollments (student_id, course_id) "
                                "SELECT %d, course_id FROM Courses WHERE course_code = '%s'",
                                student_id, class_selected); 

                execute_query(conn, query);
                if (mysql_affected_rows(conn) > 0) // Success
                {
                    printf("Successfully registered for class %s!\n", class_name);
                } else {
                    printf("Failed to register for class. Please try again.\n");
                }

                if (!max_classes_reached(conn, student_id))
                {
                    printf("Would you like to add another class?\n[1 - Yes] / [0 - No]\n");
                    scanf("%d", &valid_class);
                    if (valid_class == 1)
                    {
                        system("cls");
                        display_classes(conn, student_id, major);
                    }
                }
            }
            else
            {
                printf("This class has already been registered.\n");
            }
        }
        else 
        {
            printf("Invalid class code. Please enter a valid class code!\n");
        }

        free((void*)class_name);
    }

    // No more classes
    if (max_classes_reached(conn, student_id))
    {
        printf("You have signed up for all classes available!\n");
        system("pause");
    }
}

void remove_class(MYSQL *conn, int student_id)
{
    char *enrolled_classes = get_student_classes(conn, student_id);
    if (strcmp(enrolled_classes, "No Classes") == 0)
    {
        printf("You are not enrolled in any classes.\n");
        return;
    }
    printf("Current classes enrolled: %s\n", enrolled_classes);

    char class_code[10];
    int major = get_student_major(conn, student_id);

    bool delete = true; 
    do {
        printf("Enter the code of the class you would like to remove: ");
        scanf("%s", class_code);
        const char* class_name = check_code(conn, class_code, major);

        if (class_name != NULL)
        {
            char query[200];
            sprintf(query,  "DELETE FROM Enrollments "
                            "WHERE student_id = %d AND course_id = "
                            "(SELECT course_id FROM Courses WHERE course_code = '%s')",
                            student_id, class_code);

            execute_query(conn, query);

            if (mysql_affected_rows(conn) > 0) // Check if delete operation took place
            {
                printf("Class %s removed successfully.\n", class_name);
            } else {
                printf("Failed to remove class. Please try again!\n");
            }
            delete = true;
            free((void*)class_name);
        } else {
            printf("Invalid class code! Please enter a valid code!\n");
            delete = false;
        }
    } while(!delete);

    free(enrolled_classes);
}

void display_classes(MYSQL *conn, int student_id, int major)
{
    char *current_major = get_major_name_from_db(conn, major);
    printf("Welcome to %s!\nPlease choose a class to register for the semester...\n\n", current_major);

    char query[100];
    sprintf(query, "SELECT course_code, course_name FROM Courses WHERE major_id = %d", major);
    
    MYSQL_RES *result = execute_query(conn, query);

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        if (!is_class_registered(conn, student_id, row[0])) // Display non-registered classes
        {
            printf("%-35s\tCode: %s\n", row[1], row[0]);
        }
    }

    mysql_free_result(result);
    free(current_major);
    printf("\n");
}

int is_class_registered(MYSQL *conn, int student_id, const char *course_code)
{
    char query[200];
    sprintf(query,  "SELECT COUNT(*) FROM Enrollments e "
                    "JOIN Courses c ON e.course_id = c.course_id "
                    "WHERE e.student_id = %d AND c.course_code = '%s'",
                    student_id, course_code);

    MYSQL_RES *result = execute_query(conn, query);
    MYSQL_ROW row = mysql_fetch_row(result);

    int count = atoi(row[0]); // If count 1, then enrolled

    mysql_free_result(result);

    return count > 0;
}

int max_classes_reached(MYSQL *conn, int student_id)
{
    char query[100];
    sprintf(query, "SELECT count(*) FROM Enrollments WHERE student_id = %d", student_id);

    MYSQL_RES *result = execute_query(conn, query);

    MYSQL_ROW row = mysql_fetch_row(result);
    int class_count = row ? atoi(row[0]) : 0;

    mysql_free_result(result);

    return (class_count >= 4) ? 1 : 0;
}

char* get_student_classes(MYSQL *conn, int student_id)
{
    char query[200];
    sprintf(query,  "SELECT GROUP_CONCAT(CONCAT(c.course_name, ' (', c.course_code, ')') SEPARATOR ', ') " // Function to concat all rows in 1 row
                    "FROM Enrollments e "
                    "JOIN Courses c ON e.course_id = c.course_id "
                    "WHERE e.student_id = %d", student_id);
    
    MYSQL_RES *result = execute_query(conn, query);
    MYSQL_ROW row = mysql_fetch_row(result);

    char *classes;
    if (row && row[0] != NULL)
    {
        classes = strdup(row[0]);
    } else {
        classes = strdup("No Classes");
    }

    mysql_free_result(result);
    if (!classes)
    {
        fprintf(stderr, "Failed memory allocation\n");
        return NULL;
    }

    return classes;
}

int is_username_taken(MYSQL *conn, const char* username)
{
    char query[100];
    sprintf(query, "SELECT COUNT(*) FROM Login WHERE username = '%s'", username); // Checking if username exists in db

    MYSQL_RES *result = execute_query(conn, query);
    MYSQL_ROW row = mysql_fetch_row(result);

    int count = row ? atoi(row[0]) : 0;

    mysql_free_result(result);
    return count > 0;
}

int validate_login(MYSQL *conn, const char* username, const char* password, int *student_id)
{
    char query[100];
    sprintf(query, "SELECT student_id FROM Login WHERE username = '%s' AND password = '%s'", username, password);

    MYSQL_RES *result = execute_query(conn, query);
    MYSQL_ROW row = mysql_fetch_row(result);

    int valid = 0;
    if (row)
    {
        valid = 1;
        *student_id = atoi(row[0]);
    }

    mysql_free_result(result);

    return valid;
}

void delete_account(MYSQL *conn, int student_id)
{
    char query[100];

    // Delete from enrollments
    sprintf(query, "DELETE FROM Enrollments WHERE student_id = %d", student_id);
    execute_query(conn, query);

    // Delete from login
    sprintf(query, "DELETE FROM Login WHERE student_id = %d", student_id);
    execute_query(conn, query);

    // Delete from students
    sprintf(query, "DELETE FROM Students WHERE student_id = %d", student_id);
    execute_query(conn, query);

    printf("Account deleted successfully.\n");
}

void sql_error(MYSQL *conn)
{
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
}

MYSQL_RES* execute_query(MYSQL *conn, const char *query)
{
    if (mysql_query(conn, query))
    {
        sql_error(conn);
    }

    if (mysql_field_count(conn) == 0)
    {
        return NULL; // Not expecting data
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL)
    {
        sql_error(conn);
    }

    return result;
}