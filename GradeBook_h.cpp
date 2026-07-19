#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <limits>
#include <cstdlib> 

#ifdef _WIN32
#include <windows.h>
#include <conio.h> // Required for password masking via _getch()
#else
#include <termios.h>
#include <unistd.h>
#endif
#include <sstream>

// ==========================================
// 1. DATA STRUCTURES & GLOBAL CONFIGURATIONS
// ==========================================

struct Course {
    std::string code;
    std::string name;
    double score = 0.0;
};

// Pre-defined available courses in the university database
struct CourseCatalog {
    std::string code;
    std::string name;
};

const std::vector<CourseCatalog> UNIVERSITY_CATALOG = {
    {"IT361", "Database Management Systems"},
    {"IT362", "System Administration"},
    {"IT363", "Web Application Development"},
    {"IT364", "Mobile Application Development"},
    {"IT365", "Human Computer Interaction"},
    {"IT366", "Research Methods in IT"}
};

struct Student {
	std::string id;
    
    // Detailed profile fields
    std::string title = "Mr.";
    std::string firstName;
    std::string middleName;
    std::string lastName;
    std::string gender = "Male";
    std::string dob = "2000-05-31";
    std::string nationality = "Ghana";
    std::string programme = "B.ED. Information Technology";
    
    std::vector<Course> courses;
    int presentCount = 0;
    int lateCount = 0;
    int totalClasses = 0;
    double outstandingBalance = 0.0;
};
void saveDatabase(const std::vector<Student>& students) {
    std::ofstream outFile("grades.txt");
    if (outFile.is_open()) {
        for (const auto& s : students) {
            // Write student core info
            outFile << "STUDENT|" << s.id << "|" << s.firstName << "|" << s.lastName 
                    << "|" << s.outstandingBalance << "|" << s.presentCount 
                    << "|" << s.lateCount << "|" << s.totalClasses << "\n";
            // Write student courses
            for (const auto& c : s.courses) {
                outFile << "COURSE|" << c.code << "|" << c.name << "|" << c.score << "\n";
            }
        }
        outFile.close();
    }
}

    Student parseStudentLine(std::string line) {
    Student s;
    std::stringstream ss(line);
    std::string item;
    std::getline(ss, s.id, '|');
    std::getline(ss, s.firstName, '|');
    std::getline(ss, s.lastName, '|');
    std::getline(ss, item, '\n');
    s.outstandingBalance = (item.empty()) ? 0.0 : std::stod(item);
    return s;
}
struct GlobalSettings {
    std::string adminPassword = "admin123";
    std::string lecturerPassword = "lecturer123";
    std::string systemAnnouncement = "Welcome to the AAMUSTED Student Portal Integration.";
    std::string securityQuestion = "What is the primary security protocol?";
    std::string securityAnswer = "encryption";
};

// ==========================================
// 2. HELPER UTILITY FUNCTIONS
// ==========================================

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void setInterfaceColor(int color) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#endif
}

void pauseScreen() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Masked password input reader
std::string getMaskedPassword() {
    std::string password = "";
    char ch;
#ifdef _WIN32
    while (true) {
        ch = _getch();
        if (ch == 13) { // Enter key
            break;
        } else if (ch == 8) { // Backspace key
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b"; // Erase character from terminal view
            }
        } else if (ch >= 32 && ch <= 126) {
            password.push_back(ch);
            std::cout << '*';
        }
    }
    std::cout << std::endl;
#else
    // Fallback security for POSIX/Linux platforms
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    std::getline(std::cin, password);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    return password;
}

std::string getLetterGrade(double score, double& gradePoint) {
    if (score >= 80) { gradePoint = 4.0; return "A"; }
    else if (score >= 75) { gradePoint = 3.5; return "B+"; }
    else if (score >= 70) { gradePoint = 3.0; return "B"; }
    else if (score >= 65) { gradePoint = 2.5; return "C+"; }
    else if (score >= 60) { gradePoint = 2.0; return "C"; }
    else if (score >= 50) { gradePoint = 1.0; return "D"; }
    else { gradePoint = 0.0; return "F"; }
}

void seedDatabase(std::vector<Student>& students) {
    students.clear();
    
    Student s1 = {
        "5250250013", 
        "Mr.", "Elvis", "Boadi", "Oppong", "Male", "2000-05-31", "Ghana", 
        "B.ED. Information Technology (IT)", {}, 4, 1, 5, 120.50
    };
    s1.courses.push_back({"IT361", "Database Management Systems", 85.5});
    s1.courses.push_back({"IT362", "System Administration", 74.0});
    s1.courses.push_back({"IT363", "Web Application Development", 81.0});
    students.push_back(s1);

    Student s2 = {
        "5250250002", 
        "Ms.", "Akosua", "", "Mavis", "Female", "2002-08-15", "Ghana", 
        "B.Sc. Information Technology", {}, 3, 0, 5, 0.0
    };
    s2.courses.push_back({"IT361", "Database Management Systems", 62.0});
    students.push_back(s2);
}

std::string getFullName(const Student& s) {
    if (s.middleName.empty()) {
        return s.firstName + " " + s.lastName;
    }
    return s.firstName + " " + s.middleName + " " + s.lastName;
}

// ==========================================
// 3. STUDENT REGISTRY & PROFILE MANAGEMENT
// ==========================================

void addNewStudent(std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "        ADD NEW STUDENT REGISTRY         \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    Student s;
    std::cout << "Enter Student ID: ";
    std::cin >> s.id;
    clearInputBuffer();
    
    std::cout << "Enter Title (e.g., Mr. / Ms.): ";
    std::getline(std::cin, s.title);

    std::cout << "Enter First Name: ";
    std::getline(std::cin, s.firstName);

    std::cout << "Enter Middle Name (Leave blank if none): ";
    std::getline(std::cin, s.middleName);

    std::cout << "Enter Last Name: ";
    std::getline(std::cin, s.lastName);

    std::cout << "Enter Gender: ";
    std::getline(std::cin, s.gender);

    std::cout << "Enter Date of Birth (YYYY-MM-DD): ";
    std::getline(std::cin, s.dob);

    std::cout << "Enter Nationality: ";
    std::getline(std::cin, s.nationality);

    std::cout << "Enter Academic Programme: ";
    std::getline(std::cin, s.programme);
    
    std::cout << "Enter Outstanding Tuition/Fees Balance (GHS): ";
    while (!(std::cin >> s.outstandingBalance)) {
        std::cout << "[Error] Input numeric values only. Re-enter: ";
        clearInputBuffer();
    }
    clearInputBuffer();

	students.push_back(s);
    setInterfaceColor(10);
    std::cout << "\n>>> Student registered successfully!\n";
    setInterfaceColor(15);
    pauseScreen();
}

void editStudentInformation(std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "         EDIT STUDENT INFORMATION        \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    std::string id;
    std::cout << "Enter Student ID to update: ";
    std::cin >> id;
    clearInputBuffer();

    for (auto& s : students) {
        if (s.id == id) {
            std::cout << "Current Name: " << getFullName(s) << "\n";
            std::cout << "Enter New First Name: ";
            std::getline(std::cin, s.firstName);
            std::cout << "Enter New Middle Name: ";
            std::getline(std::cin, s.middleName);
            std::cout << "Enter New Last Name: ";
            std::getline(std::cin, s.lastName);
            std::cout << "Enter New Programme: ";
            std::getline(std::cin, s.programme);
            std::cout << "Current Balance: GHS " << s.outstandingBalance << "\nEnter New Balance: ";
            while (!(std::cin >> s.outstandingBalance)) {
                std::cout << "[Error] Input numeric value only. Re-enter: ";
                clearInputBuffer();
            }
            clearInputBuffer();
            setInterfaceColor(10);
            std::cout << "\n>>> Profile updated successfully!\n";
            setInterfaceColor(15);
            pauseScreen();
            return;
        }
    }
    setInterfaceColor(12);
    std::cout << "\n[Error] Student ID not found.\n";
    setInterfaceColor(15);
    pauseScreen();
}

void searchStudent(const std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "          SEARCH STUDENT DATABASE        \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    std::string query;
    std::cout << "Enter Student ID or Last Name to search: ";
    std::getline(std::cin, query);

    bool found = false;
    for (const auto& s : students) {
        if (s.id == query || s.lastName.find(query) != std::string::npos || s.firstName.find(query) != std::string::npos) {
            std::cout << "\n-----------------------------------------\n";
            std::cout << "ID: " << s.id << "\n";
            std::cout << "Name: " << getFullName(s) << "\n";
            std::cout << "Programme: " << s.programme << "\n";
            std::cout << "Courses Enrolled: " << s.courses.size() << "\n";
            std::cout << "Outstanding Fees: GHS " << std::fixed << std::setprecision(2) << s.outstandingBalance << "\n";
            found = true;
        }
    }
    if (!found) {
        setInterfaceColor(12);
        std::cout << "\nNo matching student record found.\n";
    }
    setInterfaceColor(15);
    pauseScreen();
}

void viewAllStudents(const std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "========================================================================================\n";
    std::cout << "                                ALL REGISTERED STUDENTS                                 \n";
    std::cout << "========================================================================================\n";
    setInterfaceColor(15);
    std::cout << std::left << std::setw(15) << "Student ID" 
              << std::setw(30) << "Name" 
              << std::setw(25) << "Programme" 
              << std::setw(12) << "Fees Due" << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    for (const auto& s : students) {
        std::cout << std::left << std::setw(15) << s.id 
                  << std::setw(30) << getFullName(s) 
                  << std::setw(25) << (s.programme.length() > 22 ? s.programme.substr(0, 22) + "..." : s.programme) 
                  << "GHS " << std::fixed << std::setprecision(2) << s.outstandingBalance << "\n";
    }
    pauseScreen();
}

void deleteStudent(std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "          DELETE STUDENT PROFILE         \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    std::string id;
    std::cout << "Enter Student ID to remove: ";
    std::cin >> id;
    clearInputBuffer();

    for (auto it = students.begin(); it != students.end(); ++it) {
        if (it->id == id) {
            students.erase(it);
            setInterfaceColor(10);
            std::cout << "\n>>> Student profile successfully deleted.\n";
            setInterfaceColor(15);
            pauseScreen();
            return;
        }
    }
    setInterfaceColor(12);
    std::cout << "\n[Error] Student ID not found.\n";
    setInterfaceColor(15);
    pauseScreen();
}

// ==========================================
// 4. LECTURER-LEVEL COURSE ASSIGNMENTS
// ==========================================

void assignStudentToCourse(std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "          ASSIGN STUDENT TO COURSE       \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    std::string id, courseCode, courseName;
    std::cout << "Enter Student ID: "; 
    std::cin >> id;

    for (auto& s : students) {
        if (s.id == id) {
            std::cout << "Enter Course Code to add (e.g., CS101): "; 
            std::cin >> courseCode;
            clearInputBuffer();
            
            std::cout << "Enter Course Name: ";
            std::getline(std::cin, courseName);

            for (const auto& c : s.courses) {
                if (c.code == courseCode) {
                    std::cout << "\n[Note] Student is already registered in this course.\n";
                    pauseScreen(); 
                    return;
                }
            }
            Course newCourse;
            newCourse.code = courseCode;
            newCourse.name = courseName;
            newCourse.score = 0.0;
            s.courses.push_back(newCourse);
            
            setInterfaceColor(10);
            std::cout << "\n>>> Course successfully assigned!\n";
            setInterfaceColor(15);
            pauseScreen(); 
            return;
        }
    }
    clearInputBuffer();
    setInterfaceColor(12);
    std::cout << "\n[Error] Student ID not found.\n";
    setInterfaceColor(15);
    pauseScreen();
}

void removeStudentFromCourse(std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "        REMOVE STUDENT FROM COURSE       \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    std::string id, courseCode;
    std::cout << "Enter Student ID: "; 
    std::cin >> id;

    for (auto& s : students) {
        if (s.id == id) {
            std::cout << "Enter Course Code to drop: "; 
            std::cin >> courseCode;
            clearInputBuffer();
            for (auto it = s.courses.begin(); it != s.courses.end(); ++it) {
                if (it->code == courseCode) {
                    s.courses.erase(it);
                    setInterfaceColor(10);
                    std::cout << "\n>>> Course dropped successfully!\n";
                    setInterfaceColor(15);
                    pauseScreen(); 
                    return;
                }
            }
            setInterfaceColor(12);
            std::cout << "\n[Error] Student is not registered in this course.\n";
            setInterfaceColor(15);
            pauseScreen(); 
            return;
        }
    }
    clearInputBuffer();
    setInterfaceColor(12);
    std::cout << "\n[Error] Student ID not found.\n";
    setInterfaceColor(15);
    pauseScreen();
}

// ==========================================
// 5. GRADE MANAGEMENT
// ==========================================

void recordOrEditGrade(std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "           RECORD / EDIT GRADE           \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    std::string id, courseCode;
    std::cout << "Enter Student ID: "; 
    std::cin >> id;

    for (auto& s : students) {
        if (s.id == id) {
            std::cout << "Enter Course Code: "; 
            std::cin >> courseCode;
            for (auto& c : s.courses) {
                if (c.code == courseCode) {
                    double newScore;
                    std::cout << "Current Score for " << c.code << ": " << c.score << "\nEnter New Score (0-100): ";
                    while (!(std::cin >> newScore) || newScore < 0 || newScore > 100) {
                        std::cout << "[Error] Input numeric value from 0 to 100: ";
                        clearInputBuffer();
                    }
                    clearInputBuffer();
                    c.score = newScore;
                    setInterfaceColor(10);
                    std::cout << "\n>>> Grade recorded successfully!\n";
                    setInterfaceColor(15);
                    pauseScreen(); 
                    return;
                }
            }
            clearInputBuffer();
            setInterfaceColor(12);
            std::cout << "\n[Error] Student is not registered in this course.\n";
            setInterfaceColor(15);
            pauseScreen(); 
            return;
        }
    }
    clearInputBuffer();
    setInterfaceColor(12);
    std::cout << "\n[Error] Student ID not found.\n";
    setInterfaceColor(15);
    pauseScreen();
}

void deleteGrade(std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "           DELETE / RESET GRADE          \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    std::string id, courseCode;
    std::cout << "Enter Student ID: "; 
    std::cin >> id;

    for (auto& s : students) {
        if (s.id == id) {
            std::cout << "Enter Course Code to reset grade: "; 
            std::cin >> courseCode;
            clearInputBuffer();
            for (auto& c : s.courses) {
                if (c.code == courseCode) {
                    c.score = 0.0;
                    setInterfaceColor(10);
                    std::cout << "\n>>> Grade reset to 0.00 successfully!\n";
                    setInterfaceColor(15);
                    pauseScreen(); 
                    return;
                }
            }
            setInterfaceColor(12);
            std::cout << "\n[Error] Student is not registered in this course.\n";
            setInterfaceColor(15);
            pauseScreen(); 
                    return;
        }
    }
    clearInputBuffer();
    setInterfaceColor(12);
    std::cout << "\n[Error] Student ID not found.\n";
    setInterfaceColor(15);
    pauseScreen();
}

void calculateGPAAndAverage(const std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "        GPA & AVERAGE PERFORMANCE        \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    std::string id;
    std::cout << "Enter Student ID: "; 
    std::cin >> id;
    clearInputBuffer();

    for (const auto& s : students) {
        if (s.id == id) {
            if (s.courses.empty()) {
                std::cout << "\nNo registered courses found for this student.\n";
                pauseScreen(); 
                return;
            }

            double sum = 0.0;
            double gpaSum = 0.0;
            std::cout << "\nStudent: " << getFullName(s) << " (" << s.id << ")\n";
            std::cout << "-----------------------------------------\n";
            std::cout << std::left << std::setw(12) << "Course" 
                      << std::setw(10) << "Score" 
                      << "Letter Grade\n";
            std::cout << "-----------------------------------------\n";

            for (const auto& c : s.courses) {
                double gp = 0.0;
                std::string letter = getLetterGrade(c.score, gp);
                sum += c.score;
                gpaSum += gp;

                std::cout << std::left << std::setw(12) << c.code 
                          << std::setw(10) << c.score 
                          << letter << "\n";
            }

            double average = sum / s.courses.size();
            double finalGPA = gpaSum / s.courses.size();

            std::cout << "-----------------------------------------\n";
            std::cout << "Class Average Score: " << std::fixed << std::setprecision(2) << average << "%\n";
            std::cout << "Calculated Cumulative GPA: " << std::fixed << std::setprecision(2) << finalGPA << " / 4.00\n";
            pauseScreen(); 
            return;
        }
    }
    setInterfaceColor(12);
    std::cout << "\n[Error] Student ID not found.\n";
    setInterfaceColor(15);
    pauseScreen();
}

void viewCoursePerformanceMetrics(const std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "       COURSE PERFORMANCE METRICS        \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    if (students.empty()) {
        std::cout << "Database contains no registered students.\n";
        pauseScreen(); 
        return;
    }

    std::string courseCode;
    std::cout << "Enter Course Code: "; 
    std::cin >> courseCode;
    clearInputBuffer();

    double totalScore = 0.0;
    int count = 0;
    double highest = -1.0;
    double lowest = 101.0;
    std::string highStudent = "N/A", lowStudent = "N/A";

    for (const auto& s : students) {
        for (const auto& c : s.courses) {
            if (c.code == courseCode) {
                totalScore += c.score;
                count++;
                if (c.score > highest) { highest = c.score; highStudent = getFullName(s); }
                if (c.score < lowest) { lowest = c.score; lowStudent = getFullName(s); }
            }
        }
    }

    if (count == 0) {
        std::cout << "\nNo registered records found for course code: " << courseCode << "\n";
    } else {
        double dummyGP;
        std::cout << "\n=== Statistics Overview for " << courseCode << " ===\n";
        std::cout << "Total Enrollments:  " << count << "\n";
        std::cout << "Course Average:     " << std::fixed << std::setprecision(2) << (totalScore / count) << "%\n";
        std::cout << "Highest Score:      " << highest << "% (" << highStudent << ") Grade: " << getLetterGrade(highest, dummyGP) << "\n";
        std::cout << "Lowest Score:       " << lowest << "% (" << lowStudent << ") Grade: " << getLetterGrade(lowest, dummyGP) << "\n";
    }
    pauseScreen();
}

// ==========================================
// 6. ATTENDANCE TRACKING SYSTEM
// ==========================================

void manageAttendance(std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "=========================================\n";
    std::cout << "         ATTENDANCE MANAGEMENT           \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    
    if (students.empty()) {
        std::cout << "No students registered to take attendance.\n";
        pauseScreen(); 
        return;
    }

    std::cout << "Mark choices: [1] Present  [2] Late  [3] Absent\n";
    std::cout << "-----------------------------------------\n";
    for (auto& s : students) {
        int choice = 0;
        std::cout << "Student: " << getFullName(s) << " (" << s.id << ")\nChoice: ";
        while (!(std::cin >> choice) || choice < 1 || choice > 3) {
            std::cout << "[Error] Input choice (1-3): ";
            clearInputBuffer();
        }

        s.totalClasses++;
        if (choice == 1) s.presentCount++;
        else if (choice == 2) s.lateCount++;
    }
    clearInputBuffer();
    setInterfaceColor(10);
    std::cout << "\n>>> Attendance session updated successfully!\n";
    setInterfaceColor(15);
    pauseScreen();
}

void viewAttendancePercentage(const std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "====================================================================\n";
    std::cout << "                        ATTENDANCE OVERVIEW                         \n";
    std::cout << "====================================================================\n";
    setInterfaceColor(15);
    std::cout << std::left << std::setw(15) << "Student ID" 
              << std::setw(25) << "Name" 
              << std::setw(15) << "Classes Tracked" 
              << std::setw(15) << "Attendance %" << "\n";
    std::cout << "--------------------------------------------------------------------\n";

    for (const auto& s : students) {
        double attPct = (s.totalClasses == 0) ? 0.0 : 
                        (((double)(s.presentCount + s.lateCount) / s.totalClasses) * 100.0);
        
        std::cout << std::left << std::setw(15) << s.id 
                  << std::setw(25) << getFullName(s) 
                  << std::setw(15) << s.totalClasses 
                  << std::fixed << std::setprecision(2) << attPct << "%\n";
    }
    pauseScreen();
}

void showPoorAttendance(const std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(11);
    std::cout << "====================================================================\n";
    std::cout << "                  CRITICAL ATTENDANCE WARNINGS (< 75%)               \n";
    std::cout << "====================================================================\n";
    setInterfaceColor(15);
    std::cout << std::left << std::setw(15) << "Student ID" 
              << std::setw(25) << "Name" 
              << std::setw(15) << "Attendance %" << "\n";
    std::cout << "--------------------------------------------------------------------\n";

    bool alerts = false;
    for (const auto& s : students) {
        double attPct = (s.totalClasses == 0) ? 0.0 : 
                        (((double)(s.presentCount + s.lateCount) / s.totalClasses) * 100.0);
        
        if (s.totalClasses > 0 && attPct < 75.0) {
            std::cout << std::left << std::setw(15) << s.id 
                      << std::setw(25) << getFullName(s) 
                      << std::fixed << std::setprecision(2) << attPct << "%\n";
            alerts = true;
        }
    }
    if (!alerts) std::cout << "No student attendance concerns reported at this time.\n";
    pauseScreen();
}

// ==========================================
// 7. FILE REPORT EXPORTER
// ==========================================

void exportToCSV(const std::vector<Student>& students) {
    clearScreen();
    setInterfaceColor(10);
    std::cout << "Exporting live compilation to 'report_cards.csv'...\n";

    std::ofstream file("report_cards.csv");
    if (!file) {
        setInterfaceColor(12);
        std::cout << "Cannot generate spreadsheet file structure!\n";
        setInterfaceColor(15);
        pauseScreen();
        return;
    }

    file << "Student ID,Student Name,Courses Count,Final CGPA,Attendance Percentage,Fees Outstanding\n";
    for (const auto& s : students) {
        double sum = 0.0;
        double gpa = 0.0;
        for (const auto& c : s.courses) {
            sum += c.score;
        }
        if (!s.courses.empty()) {
            gpa = sum / s.courses.size();
        }

        double attPct = (s.totalClasses == 0) ? 0.0 : 
                        (((double)(s.presentCount + s.lateCount) / s.totalClasses) * 100.0);

        file << s.id << "," << getFullName(s) << "," << s.courses.size() << ","
             << gpa << "," << attPct << "," << s.outstandingBalance << "\n";
    }
    
    file.close();
    std::cout << "Spreadsheet generated successfully!\n";
    setInterfaceColor(15);
    pauseScreen();
}

// ==========================================
// 8. ADMINISTRATOR CONTROL PANELS
// ==========================================

void manageStudentFees(std::vector<Student>& students) {
    while (true) {
        clearScreen();
        setInterfaceColor(13);
        std::cout << "=========================================\n";
        std::cout << "      ADMINISTRATIVE FINANCIAL OVERRIDE   \n";
        std::cout << "=========================================\n";
        setInterfaceColor(15);
        std::cout << " 1. View Outstanding Student Balances\n";
        std::cout << " 2. Override/Edit Student Balance Directly\n";
        std::cout << " 3. Nullify All Debts (Set All Balances to 0.00)\n";
        std::cout << " 0. Return to Administrator Control Room\n";
        std::cout << "=========================================\n";
        std::cout << "Enter Choice: ";

        int choice;
        std::cin >> choice;
        if (std::cin.fail()) { clearInputBuffer(); continue; }
        clearInputBuffer();

        if (choice == 0) break;

        if (choice == 1) {
            viewAllStudents(students);
        }
        else if (choice == 2) {
            clearScreen();
            std::string id;
            std::cout << "Enter Student ID: ";
            std::cin >> id;
            clearInputBuffer();
            
            bool found = false;
            for (auto& s : students) {
                if (s.id == id) {
                    found = true;
                    std::cout << "Current Debt Balance for " << getFullName(s) << ": GHS " << s.outstandingBalance << "\n";
                    std::cout << "Enter Override Balance Value (GHS): ";
                    while (!(std::cin >> s.outstandingBalance)) {
                        std::cout << "[Error] Value must be numerical. Try again: ";
                        clearInputBuffer();
                    }
                    clearInputBuffer();
                    setInterfaceColor(10);
                    std::cout << "\n>>> Debt records overwritten in core application storage!\n";
                    setInterfaceColor(15);
                    pauseScreen();
                    break;
                }
            }
            if (!found) {
                setInterfaceColor(12);
                std::cout << "[Error] ID reference not found.\n";
                setInterfaceColor(15);
                pauseScreen();
            }
        }
        else if (choice == 3) {
            clearScreen();
            std::cout << "WARNING: You are about to clear all fees across the entire directory.\n";
            std::cout << "Are you absolutely sure? (Type 'YES' to confirm): ";
            std::string confirm;
            std::cin >> confirm;
            clearInputBuffer();
            if (confirm == "YES" || confirm == "yes") {
                for (auto& s : students) {
                    s.outstandingBalance = 0.00;
                }
                setInterfaceColor(10);
                std::cout << "\n>>> All student debt records successfully wiped.\n";
                setInterfaceColor(15);
            } else {
                std::cout << "\nOperation aborted.\n";
            }
            pauseScreen();
        }
    }
}

void broadcastAnnouncement(GlobalSettings& settings) {
    clearScreen();
    setInterfaceColor(13);
    std::cout << "=========================================\n";
    std::cout << "        ADMIN BROADCAST CONSOLE          \n";
    std::cout << "=========================================\n";
    setInterfaceColor(15);
    std::cout << "Current Active Announcement:\n";
    setInterfaceColor(14);
    std::cout << ">> " << settings.systemAnnouncement << "\n\n";
    setInterfaceColor(15);

    std::cout << "Enter NEW Announcement Board Message: ";
    std::getline(std::cin, settings.systemAnnouncement);

    setInterfaceColor(10);
    std::cout << "\n>>> Broadcast announcement updated. Live feed deployed.\n";
    setInterfaceColor(15);
    pauseScreen();
}

void runAdminControlRoom(std::vector<Student>& students, GlobalSettings& settings) {
    while (true) {
        clearScreen();
        setInterfaceColor(13);
        std::cout << "=========================================================\n";
        std::cout << "             ADMINISTRATOR CONTROL PANEL (MASTER)        \n";
        std::cout << "=========================================================\n";
        setInterfaceColor(15);
        std::cout << " 1. Override Student Fee Balances\n";
        std::cout << " 2. Broadcast System-Wide Board Announcement\n";
        std::cout << " 3. Edit System Credentials (Passwords & Answers)\n";
        std::cout << " 4. Reset Memory (Reload Standard Seed Test Profiles)\n";
        std::cout << " 5. Clean Wipe Entire Student Database\n";
        std::cout << " 0. Relinquish Control / Return to User Interface Gate\n";
        std::cout << "=========================================================\n";
        std::cout << "Secure Console Action Choice: ";

        int choice;
        std::cin >> choice;
        if (std::cin.fail()) { clearInputBuffer(); continue; }
        clearInputBuffer();

        if (choice == 0) break;

        switch (choice) {
            case 1:
                manageStudentFees(students);
                break;
            case 2:
                broadcastAnnouncement(settings);
                break;
            case 3: {
                clearScreen();
                setInterfaceColor(13);
                std::cout << "==================================================\n";
                std::cout << "             CREDENTIAL CONFIGURATION             \n";
                std::cout << "==================================================\n";
                setInterfaceColor(15);
                std::cout << "Enter NEW Admin Access Password: ";
                std::getline(std::cin, settings.adminPassword);
                std::cout << "Enter NEW Lecturer Access Password: ";
                std::getline(std::cin, settings.lecturerPassword);
                std::cout << "Enter Verification Security Question: ";
                std::getline(std::cin, settings.securityQuestion);
                std::cout << "Enter Security Question Verification Key: ";
                std::getline(std::cin, settings.securityAnswer);
                setInterfaceColor(10);
                std::cout << "\nSystem validation hashes modified successfully.\n";
                setInterfaceColor(15);
                pauseScreen();
                break;
            }
            case 4:
                seedDatabase(students);
                setInterfaceColor(10);
                std::cout << "\n>>> Database refreshed with seeded student records.\n";
                setInterfaceColor(15);
                pauseScreen();
                break;
            case 5:
                clearScreen();
                std::cout << "CRITICAL: You are about to purge ALL registered students from storage.\n";
                std::cout << "Are you completely sure? (Type 'DESTROY' to confirm): ";
                {
                    std::string validation;
                    std::getline(std::cin, validation);
                    if (validation == "DESTROY" || validation == "destroy") {
                        students.clear();
                        setInterfaceColor(12);
                        std::cout << "\n>>> System memory purged. Database empty.\n";
                        setInterfaceColor(15);
                    } else {
                        std::cout << "\nPurge canceled.\n";
                    }
                }
                pauseScreen();
                break;
            default:
                std::cout << "Selection reference unrecognized.\n";
                pauseScreen();
        }
    }
}

// ==========================================
// 9. SYSTEM EXECUTIVE MENUS
// ==========================================

void runLecturerMenu(std::vector<Student>& students) {
    while (true) {
        clearScreen();
        setInterfaceColor(11);
        std::cout << "==================================================\n";
        std::cout << "             LECTURER PORTAL SUITE                \n";
        std::cout << "==================================================\n";
        setInterfaceColor(15);
        std::cout << "--- STUDENT REGISTRY MANAGEMENT ---\n";
        std::cout << " 1. Register a New Student\n";
        std::cout << " 2. Edit Student Information\n";
        std::cout << " 3. Search Student by ID or Name\n";
        std::cout << " 4. View All Registered Students\n";
        std::cout << " 5. Delete Student Profile\n\n";

        std::cout << "--- COURSE MANAGEMENT ---\n";
        std::cout << " 6. Assign Student to Course\n";
        std::cout << " 7. Remove Student from Course\n\n";

        std::cout << "--- GRADE MANAGEMENT ---\n";
        std::cout << " 8. Record / Edit Grades\n";
        std::cout << " 9. Delete or Reset Grade\n";
        std::cout << "10. Calculate GPA and Class Performance\n";
        std::cout << "11. View Course Performance Metrics\n\n";

        std::cout << "--- ATTENDANCE SYSTEM ---\n";
        std::cout << "12. Take Lecture Session Attendance\n";
        std::cout << "13. View Attendance Percentages\n";
        std::cout << "14. Show Students with Poor Attendance (< 75%)\n\n";

        std::cout << "--- REPORT GENERATOR ---\n";
        std::cout << "15. Export Database Report to CSV Spreadsheet\n\n";

        std::cout << " 0. Return to User Interface Gate\n";
        std::cout << "==================================================\n";
        std::cout << "Action Choice: ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: addNewStudent(students); break;
            case 2: editStudentInformation(students); break;
            case 3: searchStudent(students); break;
            case 4: viewAllStudents(students); break;
            case 5: deleteStudent(students); break;
            case 6: assignStudentToCourse(students); break;
            case 7: removeStudentFromCourse(students); break;
            case 8: recordOrEditGrade(students); break;
            case 9: deleteGrade(students); break;
            case 10: calculateGPAAndAverage(students); break;
            case 11: viewCoursePerformanceMetrics(students); break;
            case 12: manageAttendance(students); break;
            case 13: viewAttendancePercentage(students); break;
            case 14: showPoorAttendance(students); break;
            case 15: exportToCSV(students); break;
            case 0: return;
            default:
                setInterfaceColor(12);
                std::cout << "Invalid selection. Try again.\n";
                setInterfaceColor(15);
                pauseScreen();
        }
    }
}

// Interactive course self-registration panel for students
void runStudentRegistrationMenu(Student& s) {
    while (true) {
        clearScreen();
        setInterfaceColor(11);
        std::cout << "=========================================================\n";
        std::cout << "               ONLINE COURSE REGISTRATION                \n";
        std::cout << "=========================================================\n";
        setInterfaceColor(15);
        std::cout << " 1. View Available University Course Catalog\n";
        std::cout << " 2. Register for a Course\n";
        std::cout << " 3. Drop an Existing Registered Course\n";
        std::cout << " 0. Return to main portal directory\n";
        std::cout << "=========================================================\n";
        std::cout << "Action Choice: ";

        int regChoice;
        std::cin >> regChoice;
        if (std::cin.fail()) { clearInputBuffer(); continue; }
        clearInputBuffer();

        if (regChoice == 0) break;

        if (regChoice == 1) {
            clearScreen();
            setInterfaceColor(11);
            std::cout << "=========================================================\n";
            std::cout << "               UNIVERSITY COURSE CATALOG                 \n";
            std::cout << "=========================================================\n";
            setInterfaceColor(15);
            std::cout << std::left << std::setw(15) << "Course Code" << "Course Title\n";
            std::cout << "---------------------------------------------------------\n";
            for (const auto& item : UNIVERSITY_CATALOG) {
                std::cout << std::left << std::setw(15) << item.code << item.name << "\n";
            }
            pauseScreen();
        } 
        else if (regChoice == 2) {
            clearScreen();
            setInterfaceColor(11);
            std::cout << "=========================================================\n";
            std::cout << "               REGISTER NEW COURSE                       \n";
            std::cout << "=========================================================\n";
            setInterfaceColor(15);
            std::cout << "Enter Course Code to Register (e.g., IT364): ";
            std::string codeToReg;
            std::cin >> codeToReg;
            clearInputBuffer();

            // Check if student is already registered
            bool alreadyRegistered = false;
            for (const auto& existing : s.courses) {
                if (existing.code == codeToReg) {
                    alreadyRegistered = true;
                    break;
                }
            }

            if (alreadyRegistered) {
                setInterfaceColor(14);
                std::cout << "\n[Notice] You are already registered for this course.\n";
                setInterfaceColor(15);
                pauseScreen();
                continue;
            }

            // Find match in catalog
            bool matchedCatalog = false;
            for (const auto& catItem : UNIVERSITY_CATALOG) {
                if (catItem.code == codeToReg) {
                    matchedCatalog = true;
                    Course newCourse;
                    newCourse.code = catItem.code;
                    newCourse.name = catItem.name;
                    newCourse.score = 0.0;
                    s.courses.push_back(newCourse);
                    
                    setInterfaceColor(10);
                    std::cout << "\n>>> Success! " << catItem.code << " has been added to your profile.\n";
                    setInterfaceColor(15);
                    break;
                }
            }

            if (!matchedCatalog) {
                setInterfaceColor(12);
                std::cout << "\n[Error] Invalid Course Code. Course not found in university catalog.\n";
                setInterfaceColor(15);
            }
            pauseScreen();
        }
        else if (regChoice == 3) {
            clearScreen();
            setInterfaceColor(11);
            std::cout << "=========================================================\n";
            std::cout << "                   DROP REGISTERED COURSE                \n";
            std::cout << "=========================================================\n";
            setInterfaceColor(15);
            if (s.courses.empty()) {
                std::cout << "You currently have no registered courses to drop.\n";
                pauseScreen();
                continue;
            }

            std::cout << "Your Active Registrations:\n";
            for (const auto& c : s.courses) {
                std::cout << " * " << c.code << " : " << c.name << "\n";
            }
            std::cout << "\nEnter Course Code to Drop: ";
            std::string codeToDrop;
            std::cin >> codeToDrop;
            clearInputBuffer();

            bool dropped = false;
            for (auto it = s.courses.begin(); it != s.courses.end(); ++it) {
                if (it->code == codeToDrop) {
                    s.courses.erase(it);
                    dropped = true;
                    setInterfaceColor(10);
                    std::cout << "\n>>> Success! Course " << codeToDrop << " has been successfully dropped.\n";
                    setInterfaceColor(15);
                    break;
                }
            }

            if (!dropped) {
                setInterfaceColor(12);
                std::cout << "\n[Error] You are not registered for that course code.\n";
                setInterfaceColor(15);
            }
            pauseScreen();
        }
    }
}

// Student Portal Main Directory
void runStudentDashboard(Student& s) {
    while (true) {
        clearScreen();
        setInterfaceColor(11);
        std::cout << "=========================================================\n";
        std::cout << "               AAMUSTED STUDENT WEB PORTAL               \n";
        std::cout << "=========================================================\n";
        setInterfaceColor(14);
        std::cout << " Welcome back, " << s.title << " " << s.firstName << " " << s.lastName << "\n";
        setInterfaceColor(11);
        std::cout << "=========================================================\n";
        setInterfaceColor(15);
        
        std::cout << " [1] Personal Profile Information\n";
        std::cout << " [2] Registration (Course Self-Registration)\n"; // Modified option
        std::cout << " [3] Results & CGPA\n";
        std::cout << " [4] Fees (Account Statement)\n";
        std::cout << " [5] Resits and ICs\n";
        std::cout << " [6] Internship Placement Status\n";
        std::cout << " [7] Submit Academic Complaint\n";
        std::cout << " [8] Medical Clearance Status\n";
        std::cout << " [9] Course / Lecturer Evaluation\n";
        std::cout << " [0] Log Out & Terminate Portal Session\n";
        std::cout << "=========================================================\n";
        std::cout << "Portal Action Choice: ";

        int choice;
        std::cin >> choice;
        if (std::cin.fail()) { clearInputBuffer(); continue; }
        clearInputBuffer();

        if (choice == 0) break;

        switch (choice) {
            case 1: {
                clearScreen();
                setInterfaceColor(11);
                std::cout << "=========================================\n";
                std::cout << "     PERSONAL PROFILE & PROGRAMME        \n";
                std::cout << "=========================================\n";
                setInterfaceColor(15);
                std::cout << " Title:         " << s.title << "\n";
                std::cout << " Last Name:     " << s.lastName << "\n";
                std::cout << " Middle Name:   " << (s.middleName.empty() ? "N/A" : s.middleName) << "\n";
                std::cout << " First Name:    " << s.firstName << "\n";
                std::cout << " Gender:        " << s.gender << "\n";
                std::cout << " Date of Birth: " << s.dob << "\n";
                std::cout << " Nationality:   " << s.nationality << "\n";
                std::cout << "-----------------------------------------\n";
                setInterfaceColor(14);
                std::cout << " Programme:     " << s.programme << "\n";
                setInterfaceColor(15);
                pauseScreen();
                break;
            }
            case 2: {
                // Interactive self registration
                runStudentRegistrationMenu(s);
                break;
            }
            case 3: {
                clearScreen();
                setInterfaceColor(11);
                std::cout << "=========================================\n";
                std::cout << "        SEMESTER GRADE REPORT SHEET      \n";
                std::cout << "=========================================\n";
                setInterfaceColor(15);
                if (s.courses.empty()) {
                    std::cout << "No graded academic entries found.\n";
                } else {
                    double totalSum = 0.0;
                    double gpaSum = 0.0;
                    std::cout << std::left << std::setw(12) << "Code" 
                              << std::setw(10) << "Score" 
                              << "Grade\n";
                    std::cout << "-----------------------------------------\n";
                    for (const auto& c : s.courses) {
                        double gp = 0.0;
                        std::string letter = getLetterGrade(c.score, gp);
                        totalSum += c.score;
                        gpaSum += gp;
                        std::cout << std::left << std::setw(12) << c.code 
                                  << std::setw(10) << c.score 
                                  << letter << "\n";
                    }
                    double average = totalSum / s.courses.size();
                    double cgpa = gpaSum / s.courses.size();
                    std::cout << "-----------------------------------------\n";
                    std::cout << "Cumulative GPA (CGPA): " << std::fixed << std::setprecision(2) << cgpa << " / 4.00\n";
                }
                pauseScreen();
                break;
            }
            case 4: {
                clearScreen();
                setInterfaceColor(11);
                std::cout << "=========================================\n";
                std::cout << "         FINANCIAL STATEMENT SHEET       \n";
                std::cout << "=========================================\n";
                setInterfaceColor(15);
                std::cout << " Registered Student ID:  " << s.id << "\n";
                std::cout << " Name of Account:        " << getFullName(s) << "\n";
                std::cout << "-----------------------------------------\n";
                if (s.outstandingBalance <= 0) {
                    setInterfaceColor(10);
                    std::cout << " Status:                 COMPLETELY CLEARED\n";
                    std::cout << " Outstanding Fees Due:   GHS 0.00\n";
                } else {
                    setInterfaceColor(12);
                    std::cout << " Status:                 PENDING DUES\n";
                    std::cout << " Outstanding Fees Due:   GHS " << std::fixed << std::setprecision(2) << s.outstandingBalance << "\n";
                }
                setInterfaceColor(15);
                pauseScreen();
                break;
            }
            case 5: {
                clearScreen();
                setInterfaceColor(11);
                std::cout << "=========================================\n";
                std::cout << "             RESITS & INCOMPLETES        \n";
                std::cout << "=========================================\n";
                setInterfaceColor(15);
                std::cout << "No outstanding Incomplete Courses (IC) or Scheduled Resit Examinations listed.\n";
                pauseScreen();
                break;
            }
            case 6: {
                clearScreen();
                setInterfaceColor(11);
                std::cout << "=========================================\n";
                std::cout << "           INTERNSHIP PLACEMENT          \n";
                std::cout << "=========================================\n";
                setInterfaceColor(15);
                std::cout << "Academic Internship Status: REGISTERED & PENDING REVIEW\n";
                pauseScreen();
                break;
            }
            case 7: {
                clearScreen();
                setInterfaceColor(11);
                std::cout << "=========================================\n";
                std::cout << "         ACADEMIC COMPLAINTS PORTAL      \n";
                std::cout << "=========================================\n";
                setInterfaceColor(15);
                std::cout << "Enter your issue details (e.g., missing mark, wrong registration):\n";
                std::string complaint;
                std::getline(std::cin, complaint);
                setInterfaceColor(10);
                std::cout << "\n>>> Complaint successfully queued for Academic Review.\n";
                setInterfaceColor(15);
                pauseScreen();
                break;
            }
            case 8: {
                clearScreen();
                setInterfaceColor(11);
                std::cout << "=========================================\n";
                std::cout << "        UNIVERSITY MEDICAL CLEARANCE     \n";
                std::cout << "=========================================\n";
                setInterfaceColor(15);
                std::cout << "Clearance Status: APPROVED / CLEARED (AAMUSTED Clinic System)\n";
                pauseScreen();
                break;
            }
            case 9: {
                clearScreen();
                setInterfaceColor(11);
                std::cout << "=========================================\n";
                std::cout << "        LECTURE & COURSE EVALUATIONS     \n";
                std::cout << "=========================================\n";
                setInterfaceColor(15);
                std::cout << "All available courses for this semester have been successfully evaluated.\n";
                pauseScreen();
                break;
            }
            default:
                std::cout << "Invalid Option. Choose from the available portal actions.\n";
                pauseScreen();
        }
    }
}

// ==========================================
// 10. ENTRY POINT: GATE SELECTOR
// ==========================================

int main() {
    std::vector<Student> students;
    GlobalSettings settings;

    // Load existing data
 std::ifstream inFile("grades.txt");
    if (inFile.is_open()) {
        std::string line;
        Student* currentStudent = nullptr;
        while (std::getline(inFile, line)) {
            std::stringstream ss(line);
            std::string type;
            std::getline(ss, type, '|');

            if (type == "STUDENT") {
                Student s;
                std::getline(ss, s.id, '|');
                std::getline(ss, s.firstName, '|');
                std::getline(ss, s.lastName, '|');
                std::string bal, p, l, t;
                std::getline(ss, bal, '|'); s.outstandingBalance = std::stod(bal);
                std::getline(ss, p, '|'); s.presentCount = std::stoi(p);
                std::getline(ss, l, '|'); s.lateCount = std::stoi(l);
                std::getline(ss, t, '\n'); s.totalClasses = std::stoi(t);
                students.push_back(s);
                currentStudent = &students.back();
            } else if (type == "COURSE" && currentStudent != nullptr) {
                Course c;
                std::getline(ss, c.code, '|');
                std::getline(ss, c.name, '|');
                std::string sc;
                std::getline(ss, sc, '\n');
                c.score = std::stod(sc);
                currentStudent->courses.push_back(c);
            }
        }
        inFile.close();
    } else {
        seedDatabase(students);
    }

    int interfaceChoice = 0;

    while (true) {
        clearScreen();
        setInterfaceColor(11);
        std::cout << "=======================================================\n";
        std::cout << "               CAMPUS ANNOUNCEMENT BOARD               \n";
        std::cout << "=======================================================\n";
        setInterfaceColor(14);
        std::cout << " " << settings.systemAnnouncement << "\n";
        setInterfaceColor(11);
        std::cout << "=======================================================\n\n";
        
        std::cout << "=======================================================\n";
        std::cout << "                 SELECT USER INTERFACE                 \n";
        std::cout << "=======================================================\n";
        setInterfaceColor(15);
        std::cout << " 1. Administrator Dashboard (High Security)\n";
        std::cout << " 2. Lecturer Portal Dashboard (Protected)\n";
        std::cout << " 3. Student Portal Dashboard (Individual Verification)\n";
        std::cout << " 4. Terminate Program Execution\n";
        std::cout << "=======================================================\n";
        std::cout << "Enter Choice (1, 2, 3, or 4): ";
        
        std::cin >> interfaceChoice;

        if (std::cin.fail()) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        if (interfaceChoice == 4) {
            clearScreen();
            std::cout << "\nTerminating application... Goodbye!\n";
            break;
        }

        // ==========================================
        // ADMINISTRATOR DASHBOARD ACCESS
        // ==========================================
        if (interfaceChoice == 1) {
            std::cout << "Enter Admin Security Password: ";
            std::string adminPass = getMaskedPassword(); // Masked Input
            
            if (adminPass == settings.adminPassword) {
                runAdminControlRoom(students, settings);
            } else {
                setInterfaceColor(12);
                std::cout << "\nAccess Denied.\n";
                setInterfaceColor(15);
                pauseScreen();
            }
        }

        // ==========================================
        // LECTURER PORTAL ACCESS
        // ==========================================
        else if (interfaceChoice == 2) {
            std::cout << "Enter Lecturer Access Key Password: ";
            std::string passInput = getMaskedPassword(); // Masked Input

            if (passInput == settings.lecturerPassword) {
                runLecturerMenu(students); 
            } else {
                setInterfaceColor(12);
                std::cout << "\nAuthentication Failure. Unauthorized execution attempt.\n";
                setInterfaceColor(15);
                pauseScreen();
            }
        }

        // ==========================================f
        // STUDENT PORTAL ACCESS
        // ==========================================
        else if (interfaceChoice == 3) {
            std::cout << "Enter your registered Student ID to log in: ";
            std::string studentID = getMaskedPassword(); // Masked Input

            bool studentFound = false;
            for (auto& s : students) {
                if (s.id == studentID) {
                    studentFound = true;
                    runStudentDashboard(s); 
                    break;
                }
            }

            if (!studentFound) {
                setInterfaceColor(12);
                std::cout << "\n[Error] Student ID not found.\n";
                setInterfaceColor(15);
                pauseScreen();
            }
        }
    }
    return 0;
}