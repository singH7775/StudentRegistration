#include "main.h"

void register_student(MYSQL *conn) 
{ 
    char username[50], password[MAX_PASSWORD_LENGTH], first_name[50], last_name[50];
    int student_id;

    // Check if username is taken
    do {
        printf("Enter a username(case sensitive) : ");
        scanf("%s", username);
        if (is_username_taken(conn, username))
            printf("Username is already taken. Please choose another.\n");
    } while (is_username_taken(conn, username));

    do {
    printf("Enter a password: ");
    scanf("%s", password);
    if (strlen(password) < 10 || strlen(password) > 50 || strchr(password, ' ') != NULL)
    {
        fflush(stdin);
        printf("Please make your password is between 10 and 50 characters and having no spaces!\n");
    }
    } while(strlen(password) < 10 || strlen(password) > 50 || strchr(password, ' ') != NULL);

    srand(time(NULL));
    unsigned int salt_key = rand() % (4,294,967,295 + 1);
    char salted_password[MAX_SALTED_PASSWORD_LENGTH];
    snprintf(salted_password, sizeof(salted_password), "%s%u", password, salt_key);
    
    // Student Information
    printf("First name: ");
    scanf("%s", first_name);
    printf("Last name: ");
    scanf("%s", last_name);

    system("clear");

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
    char query[QUERY_SIZE];
    sprintf(query, "INSERT INTO Students (first_name, last_name, major_id) VALUES('%s', '%s', %d)",
            first_name, last_name, major);

    execute_query(conn, query);

    if (mysql_affected_rows(conn) > 0) // Row was inserted
    {
        printf("Student added successfully.\n");
        student_id = mysql_insert_id(conn); // Retrieve the student id from students table

        // Login table
        sprintf(query, "INSERT INTO Login (student_id, username, password, Salt_Key) VALUES (%d, '%s', SHA2('%s', 256), %u)",
                                            student_id, username, salted_password, salt_key);

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
    system_pauser();
    
    // Class operations
    system("clear");
    display_classes(conn, student_id, major);
    register_class(conn, student_id, major);
    system("clear");

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
        system("clear");
        printf("What would you like to do?\n1.Add a Class\n2.Remove a class\n3.Display classes enrolled\n4.Delete account\n5.Exit\nEnter: ");
        scanf("%d", &choice);
        system("clear");
        switch(choice)
        {
            case 1:
                if (max_classes_reached(conn, student_id))
                {
                printf("You have signed up for all classes available!\n");
                system_pauser();
                break;
                } else {
                int major = get_student_major(conn, student_id);
                display_classes(conn, student_id, major);
                register_class(conn, student_id, major);
                break;
                }
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
                system_pauser();
                break;
            case 4:
                system("clear");
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
    char query[QUERY_SIZE];
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
    char query[QUERY_SIZE];

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
    char query[QUERY_SIZE];
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
    char query[QUERY_SIZE];

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
                char query[QUERY_SIZE];
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
                    while(valid_class != 1 || valid_class != 0) {
                    printf("Would you like to add another class?\n[1 - Yes] / [0 - No]\n");
                    scanf("%d", &valid_class);
                    if (valid_class == 1 || valid_class == 0)
                        break;
                    printf("Please enter a valid choice\n");
                    }
                    if (valid_class == 1)
                    {
                        system("clear");
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
        system_pauser();
    }
}

void remove_class(MYSQL *conn, int student_id)
{
    char *enrolled_classes = get_student_classes(conn, student_id);
    if (strcmp(enrolled_classes, "No Classes") == 0)
    {
        printf("You are not enrolled in any classes.\n");
        system_pauser();
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
            char query[QUERY_SIZE];
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
            system_pauser();
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

    char query[QUERY_SIZE];
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
    char query[QUERY_SIZE];
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
    char query[QUERY_SIZE];
    sprintf(query, "SELECT count(*) FROM Enrollments WHERE student_id = %d", student_id);

    MYSQL_RES *result = execute_query(conn, query);

    MYSQL_ROW row = mysql_fetch_row(result);
    int class_count = row ? atoi(row[0]) : 0;

    mysql_free_result(result);

    return (class_count >= 4) ? 1 : 0;
}

char* get_student_classes(MYSQL *conn, int student_id)
{
    char query[QUERY_SIZE];
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
    char query[QUERY_SIZE];
    sprintf(query, "SELECT COUNT(*) FROM Login WHERE username = '%s'", username); // Checking if username exists in db

    MYSQL_RES *result = execute_query(conn, query);
    MYSQL_ROW row = mysql_fetch_row(result);

    int count = row ? atoi(row[0]) : 0;

    mysql_free_result(result);
    return count > 0;
}

int validate_login(MYSQL *conn, const char* username, const char* password, int *student_id)
{
    char query[QUERY_SIZE];
    // Get salt key
    unsigned int salt_key = 0;
    sprintf(query, "SELECT salt_key FROM Login WHERE BINARY username = '%s'", username);
    MYSQL_RES *result = execute_query(conn, query);
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        salt_key = atoi(row[0]);
    }
    mysql_free_result(result);

    char salted_password[MAX_SALTED_PASSWORD_LENGTH];
    snprintf(salted_password, sizeof(salted_password), "%s%u", password, salt_key);
    
    sprintf(query, "SELECT student_id FROM Login WHERE BINARY username = '%s' AND password = SHA2('%s', 256)", username, salted_password);

    MYSQL_RES *result2 = execute_query(conn, query);
    MYSQL_ROW row2 = mysql_fetch_row(result2);

    int valid = 0;
    if (row2)
    {
        valid = 1;
        *student_id = atoi(row2[0]);
    }

    mysql_free_result(result2);

    return valid;
}

void delete_account(MYSQL *conn, int student_id)
{
    char query[QUERY_SIZE];

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

void system_pauser() 
{
    printf("Press Enter to continue...");
    while (getchar() != '\n');
    getchar();
}

void test_db_info(MYSQL *conn, int student_id) 
{
    int choice;
    char query[QUERY_SIZE];
    printf("Which table would you like to test?\n1.Students\n2.Enrollments\n3.Login\n");
    scanf("%d", &choice);
    while (choice < 1 || choice > 3) {
        printf("Please enter a valid choice: ");
        scanf("%d", &choice);
    }    
    while (getchar() != '\n');

    switch(choice) {
        MYSQL_RES *result;
        case 1 :
            sprintf(query, "SELECT * FROM Students WHERE student_id = %d", student_id);
            result = execute_query(conn, query);
            if (result) {
                MYSQL_ROW row = mysql_fetch_row(result);
                if (row) {
                    int num_fields = mysql_num_fields(result);
                    MYSQL_FIELD *fields = mysql_fetch_fields(result);
                    for (int i = 0; i<num_fields; i++) {
                        printf("%s - %s\n", fields[i].name, row[i] ? row[i] : "NULL");
                    }
                    printf("\n");
                } else {
                    printf("No data found\n");
                }
                mysql_free_result(result);
            } else {
                printf("Query failed\n");
            }
            break;
        case 2 :
            sprintf(query, "SELECT * FROM Enrollments WHERE student_id = %d", student_id);
            result = execute_query(conn, query);

            if (result) {
                MYSQL_ROW row = mysql_fetch_row(result);
                if (row) {
                    int num_fields = mysql_num_fields(result);
                    MYSQL_FIELD *fields = mysql_fetch_fields(result);
                    for(int i = 0; i<num_fields; i++) {
                            printf("%s - %s\n", fields[i].name, row[i] ? row[i] : "NULL"); // Print out the first row fetched
                    }
                    while (row = mysql_fetch_row(result)) { // Print out the rest of the rows fetched
                        for(int i = 0; i<num_fields; i++) {
                            printf("%s - %s\n", fields[i].name, row[i] ? row[i] : "NULL");
                        }
                    }
                    printf("\n");
                } else {
                    printf("No data found\n");
                }
                mysql_free_result(result);
            } else {
                printf("Query failed\n");
            }
            break;
        case 3 :
            sprintf(query, "SELECT * FROM Login WHERE student_id = %d", student_id);
            result = execute_query(conn, query);

            if (result) {
                MYSQL_ROW row = mysql_fetch_row(result);
                if (row) {
                    int num_fields = mysql_num_fields(result);
                    MYSQL_FIELD *fields = mysql_fetch_fields(result);
                    for(int i = 0; i<num_fields; i++) {
                        printf("%s - %s\n", fields[i].name, row[i] ? row[i] : "NULL");
                    }
                } else {
                    printf("No data found");
                }
                mysql_free_result(result);
            } else {
                printf("Query failed");
            }
            break;
    }
}