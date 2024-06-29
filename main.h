#ifndef MAIN_H
#define MAIN_H

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STUDENTS 100

// Student variable
typedef struct
{
    char username[50];
    char password[50];
    char student_name[50];
    char classes[200];
    int registered_classes[4];
    int num_registered_classes;
    int major;
} student_info;

// Classes - Codes - Majors
const char *classes[] = {
        "Intro to computer programming(C++)", "Intro to computer programming(Java)", "Operating Systems", "Data Structures and Algorithms",
        "Principles of Microeconomics", "Principles of Macroeconomics", "Intermediate Microeconomic Theory", "Public Finance",
        "Fundamentals of Graphic Design", "Digital Imaging", "Layout and Composition", "Motion Graphics",
        "Introduction to Marine Biology", "Marine Invertebrate Zoology", "Marine Ecology", "Oceanography",
        "Introduction to Philosophy", "Ethics", "Ancient Greek Philosophy", "Existentialism"
};
const char *codes[] = {
        "JXLQR", "MZPWT", "FKCXD", "YBQSG",  // Computer Science
        "XZMQR", "FLBTP", "JWYND", "CKVAH",  // Economics
        "BFQTX", "RKZLM", "WSYAV", "NCTEL",  // Graphic Design
        "XHPQR", "LZMSV", "JDNBW", "KPNRX",  // Marine Biology
        "XMJPR", "LNBGZ", "DWQHF", "TYCKS"   // Philosophy
};
const char *majors[] = {"Computer Science",
                        "Economics",
                        "Graphic Design",
                        "Marine Biology",
                        "Philosophy"
};

// Student functions
void register_student(student_info *student, int *num_students);
void update_student(student_info *student, int student_index, int *num_students);
const char *check_code(const char *class_selected, int major);
const char *get_major_name(int index);

// Class functions
void register_class(student_info *student, int major);
void remove_class(student_info *student);
void display_classes(student_info *student, int major);
int get_class_index(const char *class_code, int major);
int is_class_registered(student_info *student, int class_index);

// Login and Delete function
int is_username_taken(student_info *student, int num_students, const char *username);
int login(student_info *student, int num_students, char *username, char *password);
void delete_account(student_info *student, int student_index, int *num_students);

#endif