#include "main.h"

int main()
{
    // Connection handle
    MYSQL *conn = mysql_init(NULL);
    if (!conn)
    {
        sql_error(conn);
    }

    // Connection creation
    if (mysql_real_connect(conn, "localhost", "the-programmer", "fortnite", "registered_students", 0, NULL, 0) == NULL)
    {
        sql_error(conn);
    }

    printf("Connected to MySQL database successfully\n\n");
    printf("Welcome to College!\n---------------------------\n");
    
    int choice;
    do {
        do {
            printf("What would you like to do?\n1.Register as a student\n2.Current student? Update or delete classes\n"
                    "3.Test Database\n4.Exit session\nEnter:");
            if (scanf("%d", &choice) != 1)
            {
                while(getchar() != '\n');
                printf("Please enter a number only!\n");
                choice = 0;
            } else if (choice < 1 || choice > 4) {
                printf("Please enter a valid choice!\n");
            }
        } while (choice < 1 || choice > 4);

        if (choice == 1)
        {
            if (!max_students_reached(conn)) // Max 100 allowed
            {
                register_student(conn);
            } else {
                printf("Sorry, you have reached the maximum number of students (100)\n"
                        "In order to add more students, please look to remove some students first.\n");
            }
        }
        else if (choice == 2)
        {
            int choice2;
            do {
                char username[50], password[MAX_PASSWORD_LENGTH];
                int student_id;
                printf("Enter account username(Case sensitive): ");
                scanf("%s", username);
                printf("Enter a password: ");
                scanf("%s", password);

                if (validate_login(conn, username, password, &student_id))
                {
                    system("clear");
                    printf("Login successful! Welcome %s.\n", username);
                    system_pauser();
                    update_student(conn, student_id);
                    break;
                }
                else {
                    printf("Invalid username or password.\n"
                            "Enter any number to retry or 1 to exit\n");
                    scanf("%d", &choice2);
                }
            }while(choice2 != 1);
        }
        else if (choice == 3)
        {
            bool testing = true;
            int student_id;
            char choice;
            do {
                printf("Enter a student_id you would like to test: ");
                scanf("%d", &student_id);
                test_db_info(conn, student_id);
                printf("Would you like to do another test? [y]");
                scanf("%c", &choice);
                if (choice != 'y' && choice != 'Y') {
                    testing = false;
                }
            } while (testing);
        }
    } while (choice != 4);

    mysql_close(conn);
    return 0;
}