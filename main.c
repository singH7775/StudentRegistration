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
    if (mysql_real_connect(conn, "localhost", "user1", "Wifi7000", "studentinfo", 0, NULL, 0) == NULL)
    {
        sql_error(conn);
    }

    printf("Connected to MySQL database successfully\n\n");
    printf("Welcome to College!\n---------------------------\n");
    
    int choice;
    do {
        do {
            printf("What would you like to do?\n1.Register as a student\n2.Current student? Update or delete classes\n3.Exit Session\nEnter:");
            if (scanf("%d", &choice) != 1)
            {
                while(getchar() != '\n');
                printf("Please enter a number only!\n");
                choice = 0;
            } else if (choice < 1 || choice > 3) {
                printf("Please enter a valid choice!\n");
            }
        } while (choice < 1 || choice > 3);

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
                char username[50], password[50];
                int student_id;
                printf("Enter account username: ");
                scanf("%s", username);
                printf("Enter a password: ");
                scanf("%s", password);

                if (validate_login(conn, username, password, &student_id))
                {
                    system("cls");
                    printf("Login successful!\n");
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
    } while (choice != 3);

    mysql_close(conn);
    return 0;
}