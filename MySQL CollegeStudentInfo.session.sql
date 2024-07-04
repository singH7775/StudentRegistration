USE studentinfo;

CREATE TABLE Majors (
    major_id INT AUTO_INCREMENT PRIMARY KEY,
    major_name VARCHAR(100) NOT NULL
);

CREATE TABLE Students (
    student_id INT AUTO_INCREMENT PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    major_id INT,
    FOREIGN KEY (major_id) REFERENCES Majors(major_id)
);

CREATE TABLE Login (
    login_id INT AUTO_INCREMENT PRIMARY KEY,
    student_id INT UNIQUE NOT NULL,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(50) NOT NULL,
    FOREIGN KEY (student_id) REFERENCES Students(student_id)
);

CREATE TABLE Courses (
    course_id INT AUTO_INCREMENT PRIMARY KEY,
    course_code VARCHAR(10) UNIQUE NOT NULL,
    course_name VARCHAR(100) UNIQUE NOT NULL,
    major_id INT,
    FOREIGN KEY (major_id) REFERENCES Majors(major_id)
);

CREATE TABLE Enrollments (
    enrollment_id INT AUTO_INCREMENT PRIMARY KEY,
    student_id INT NOT NULL,
    course_id INT NOT NULL,
    FOREIGN KEY (student_id) REFERENCES Students(student_id),
    FOREIGN KEY (course_id) REFERENCES Courses(course_id),
    UNIQUE(student_id, course_id)
);

INSERT INTO Majors (major_name) VALUES 
('Computer Science'),
('Economics'),
('Graphic design'),
('Marine Biology'),
('Philosophy');

INSERT IGNORE INTO Courses (course_code, course_name, major_id) VALUES
-- Computer Science (major_id 1)
('JXLQR', 'Intro to computer programming(C++)', 1),
('MZPWT', 'Intro to computer programming(Java)', 1),
('FKCXD', 'Operating Systems', 1),
('YBQSG', 'Data Structures and Algorithms', 1),

-- Economics (major_id 2)
('XZMQR', 'Principles of Microeconomics', 2),
('FLBTP', 'Principles of Macroeconomics', 2),
('JWYND', 'Intermediate Microeconomic Theory', 2),
('CKVAH', 'Public Finance', 2),

-- Graphic Design (major_id 3)
('BFQTX', 'Fundamentals of Graphic Design', 3),
('RKZLM', 'Digital Imaging', 3),
('WSYAV', 'Layout and Composition', 3),
('NCTEL', 'Motion Graphics', 3),

-- Marine Biology (major_id 4)
('XHPQR', 'Introduction to Marine Biology', 4),
('LZMSV', 'Marine Invertebrate Zoology', 4),
('JDNBW', 'Marine Ecology', 4),
('KPNRX', 'Oceanography', 4),

-- Philosophy (major_id 5)
('XMJPR', 'Introduction to Philosophy', 5),
('LNBGZ', 'Ethics', 5),
('DWQHF', 'Ancient Greek Philosophy', 5),
('TYCKS', 'Existentialism', 5);