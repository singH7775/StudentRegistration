#include "main.h"

int main()
{
    int choice, num_students = 0;
    student_info student[MAX_STUDENTS]; // Students data

    printf("Welcome to College!\n---------------------------\n");

    do {
        printf("What would you like to do?\n1.Register as a student\n2.Current student? Update or delete classes\n3.Exit Session\nEnter:");
        scanf("%d", &choice);

        if (choice == 1)
        {
            register_student(student, &num_students);
        }
        else if (choice == 2)
        {
            int choice2;
            do {
                char username[50], password[50];
                printf("Enter account username: ");
                scanf("%s", username);
                printf("Enter password: ");
                scanf("%s", password);

                // Find the index of the user if valid user
                int student_index = login(student, num_students, username, password);
                if (student_index != -1) 
                {
                    system("cls");
                    printf("Login successful!\nWelcome %s\n", student[student_index].student_name);
                    update_student(student, student_index, &num_students);
                    break;
                } 
                else 
                {
                    printf("Login failed. Invalid username or password. Enter any number to retry or 1 to exit\n");
                    scanf("%d", &choice2);
                }
            }while(choice2 != 1);
        }
    } while (choice != 3);

    return 0;
}

void register_student(student_info *student, int *num_students) {
    if (*num_students >= MAX_STUDENTS)
    {
        printf("Maximum number of students reached. Cannot register more students.\n");
        return;
    }

    student_info *new_student = &student[*num_students];
    new_student->num_registered_classes = 0;

    // Check if username is taken
    do {
        printf("Enter a username: ");
        scanf("%s", new_student->username);
        if (is_username_taken(student, *num_students, new_student->username))
            printf("Username is already taken. Please choose another.\n");
    } while (is_username_taken(student, *num_students, new_student->username));

    printf("Enter a password: ");
    scanf("%s", new_student->password);

    // Student Info
    char first_name[50], last_name[50];
    printf("First name: ");
    scanf("%s", first_name);
    printf("Last name: ");
    scanf("%s", last_name);
    sprintf(new_student->student_name, "%s %s", first_name, last_name);
    system("cls");

    printf("\nWelcome %s\nPlease choose your major below\n\n", new_student->student_name);

    // Pick your major
    int major;
    printf("1.Computer science\n2.Economics\n3.Graphic design\n4.Marine biology\n5.Philosophy\nChoice: ");

    while(1)
    {
        scanf("%d", &major);
        if (major >= 1 && major <=5)
        {
            new_student->major = major;
            break;
        }
        else
        {
            printf("Please select a valid major!\nChoice: ");
        }
    }

    system("cls");
    display_classes(new_student, major);
    register_class(new_student, major);
    system("cls");

    printf("Registered student: %s\nMajor: %s\nClasses signed up: %s\n\n", new_student->student_name, get_major_name(new_student->major), new_student->classes);
    (*num_students)++;
}

void update_student(student_info *student, int student_index, int *num_students)
{
    student_info *current_student = &student[student_index];

    // Update account options
    int choice;
    do {
        printf("What would you like to do?\n1.Add a Class\n2.Remove a class\n3.Display classes enrolled\n4.Delete account\n5.Exit\nEnter: ");
        scanf("%d", &choice);
        system("cls");
        switch(choice)
        {
            case 1:
                display_classes(current_student,current_student->major);
                register_class(current_student, current_student->major);
                printf("\nSuccessful update of account!\n");
                break;
            case 2:
                remove_class(current_student);
                printf("\nSuccessful update of account!\n");
                break;
            case 3:
                printf("Classes enrolled: %s\n", current_student->num_registered_classes == 0 ? "You have no classes" : current_student->classes);
                break;
            case 4:
                system("cls");
                int t;
                printf("Are you sure?\n1.Yes\n2.Exit\n");
                scanf("%d", &t);
                if (t == 1)
                {
                    delete_account(student, student_index, num_students);
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

// Function to check if code entered is valid
const char *check_code(const char *class_selected, int major)
{
    // Get the return index of the valid code, and return the string
    int index = get_class_index(class_selected, major);
    if (index != -1) 
    {
        return classes[(major-1)*4 + index];
    }

    return NULL;
}

const char *get_major_name(int index)
{
    if (index >= 1 && index <= 5)
    {
        return(majors[index-1]);
    }
    else
    {
        return "Invalid Major!\n";
    }
}

void register_class(student_info *student, int major)
{
    int valid_class = 1;
    while (valid_class && student->num_registered_classes < 4) {
        char class_selected[10];
        printf("Enter Class Code: ");
        scanf("%s", class_selected);
        const char *class_name = check_code(class_selected, major); // Check valid code

        if (class_name != NULL)
        {
            int class_index = get_class_index(class_selected, major);
            if (class_index != -1 && !is_class_registered(student, class_index))
            {
                student->registered_classes[student->num_registered_classes] = class_index; // Keep record of the classes that are registered
                student->num_registered_classes++;
                if(student->classes[0] != '\0')
                            strcat(student->classes, "//");
                strcat(student->classes, class_name);
                printf("Successfully registered for class %s!\n", class_name);

                // Are there classes remaining?
                if (student->num_registered_classes < 4)
                {
                    printf("Would you like to add another class?\n[1 - Yes] / [0 - No]\n");
                    scanf("%d", &valid_class);
                    if (valid_class == 1)
                    {
                        system("cls");
                        display_classes(student, major);
                    }
                }
            }
            else
            {
                printf("This class has already been registered or is invalid.\n");
            }
        }
        else 
        {
            printf("Invalid class code. Please enter a valid class code!\n");
        }
    }

    // No more classes
    if (student->num_registered_classes == 4)
    {
        printf("You have signed up for all classes!\n");
        system("pause");
    }
}

void remove_class(student_info *student)
{
    if (student->num_registered_classes == 0)
    {
        printf("You are not enrolled in any classes.\n");
        return;
    }

    printf("Current classes enrolled: \n");
    char temp_classes[200]; // Temp array to tokenize the classes for safety
    strcpy(temp_classes, student->classes);
    char *class_names[4];
    int class_count = 0;

    // Tokenize the string by the delimiter
    char *token = strtok(temp_classes, "//");
    while (token != NULL && class_count < 4)
    {
        class_names[class_count] = token;
        printf("%d. %s\n", class_count + 1, token);
        class_count++;
        token = strtok(NULL, "//"); // continue tokenization
    }

    printf("Enter the number of the class you want to remove: ");
    int choice;
    scanf("%d", &choice);

    if (choice < 1 || choice > class_count)
    {
        printf("Invalid choice.\n");
        return;
    }

    // Remove the class
    for (int i = choice - 1; i < student->num_registered_classes - 1; i++)
        student->registered_classes[i] = student->registered_classes[i + 1];

    student->num_registered_classes--;

    // Rebuild the classes string
    student->classes[0] = '\0';
    for (int i = 0; i < class_count; i++)
    {
        if (i != choice - 1)
        {
            if (student->classes[0] != '\0')
            {
                strcat(student->classes, "//");
            }
            strcat(student->classes, class_names[i]);
        }
    }

    printf("Class removed successfully.\n");
}

// Display the classes to the student
void display_classes(student_info *student, int major)
{
    printf("Welcome to %s!\nPlease choose a class to register for the semester...\n\n", majors[major-1]);

    // Only display the classes that student has not been registered
    for (int i = 0; i < 4; i++)
    {
        if (!is_class_registered(student, i))
        {
            printf("%-35s\tCode: %s\n", classes[(major-1)*4 + i], codes[(major-1)*4 + i]);
        }
    }
    printf("\n");
}

int get_class_index(const char *class_code, int major)
{
    // Step through each code and check if valid, return index of the valid code
    for (int i = 0; i < 4; i++)
    {
        if (strcasecmp(class_code, codes[(major-1)*4 + i]) == 0)
            return i;
    }
    return -1;
}

int is_class_registered(student_info *student, int class_index)
{
    // Compare to see if the class code is already in the registered_classes
    for (int i = 0; i < student->num_registered_classes; i++)
    {
        if (student->registered_classes[i] == class_index)
            return 1;
    }
    return 0;
}

int is_username_taken(student_info *student, int num_students, const char *username)
{
    for(int i = 0; i < num_students; i++)
    {
        if (strcmp(student[i].username, username) == 0) // Compare each username
            return 1;
    }
    return 0;
}

int login(student_info *student, int num_students, char *username, char *password)
{
    for (int i = 0; i < num_students; i++)
    {
        if (strcmp(student[i].username, username) == 0 &&
            strcmp(student[i].password,password) == 0) {
                return i;
        }
    }
    return -1;
}

void delete_account(student_info *student, int student_index, int *num_students)
{
    if (student_index < 0 || student_index >= *num_students)
    {
        printf("Invalid student index.\n");
        return;
    }

    for (int i = student_index; i < *num_students - 1; i++)
    {
        student[i] = student[i + 1]; // Remove student by shift array by 1
    }

    (*num_students)--;

    printf("Deleted the account.\n");
}