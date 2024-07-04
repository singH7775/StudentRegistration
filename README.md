## Project Overview
Having a base program that worked off a local struct really helped in making this project simple as the main job from there was to insert SQL statements on the operations that I had already layed out. This project aims to connect to a MySQL database (on my localmachine is named studentinfo) and allow for registration, deletion, and other misc operations for students.

## Key Features
1. Student Registration
   - New student account creation with username and password
   - Major selection from predefined options
   - Automatic student ID assignment with success

2. Course Management
   - Course registration (up to 4 courses per student)
   - Course removal
   - Display of available courses based on student's major
   - Prevention of duplicate course registrations

3. User Authentication
   - Secure login system for existing students
   - Username uniqueness enforcement

4. Account Management
   - View enrolled classes
   - Delete account functionality

5. Database Integration
   - Real-time interaction with MySQL database
   - Efficient querying and data manipulation

6. Error Handling and Input Validation
   - Comprehensive error checking for database operations
   - Input validation for usernames, passwords, and course codes

## Database Schema
The system uses a relational database with the following tables:
- Majors: Stores available majors
- Students: Student personal information
- Login: Authentication details
- Courses: Available courses for each major
- Enrollments: Many-to-many relationship between students and courses

## Code Structure
- Modular design with separate functions for each major operation
- Efficient memory management with proper allocation and deallocation
- Clear separation of concerns between UI, business logic, and data access

## Potential Improvements I will be looking to add
- Implement more robust security measures (e.g., password hashing)
- Add administrative features for course and major management