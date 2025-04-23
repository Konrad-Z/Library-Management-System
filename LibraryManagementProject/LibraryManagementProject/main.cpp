// Library Management System by Konrad Zajac (N1179676) 2025

// Libraries
#include <stdlib.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <regex>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
using namespace std;


// Structs used for data storage which is then applied to the database

struct LibraryUsers {
    int user_id;
    string name, email, phone;
};

struct BookTitles {
    int book_title_id;
    string title, author, genre;
    int published_year;
};

struct BookCopies {
    int copy_id;
    int book_title_id;
    bool available;
};

struct BorrowedBooks {
    int borrow_id;
    int copy_id;
    int user_id;
    string date_borrowed, return_date;
};

// Function for validating menu input
int TypeCheck() {
    int choice;
    while (true) {
        cout << "Enter: ";
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        break;
    }
    return choice;
}

// Function for getting and validating the date
string GetDate(string question) {
    string date;

    while (true) {
        cout << question << "YYYY-MM-DD: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        getline(cin, date);

        regex datePattern("^\\d{4}-\\d{2}-\\d{2}$");
        if (!regex_match(date, datePattern)) {
            cout << "Invalid format. Please use YYYY-DD-MM \n";
            continue;
        }
        break;
    }
    return date;
}

// Function for getting and validating the email
string ValidateEmail() {
    regex emailPattern(R"((\w+)(\.|\_)?(\w*)@(\w+)(\.(\w+))+)"); // email format using regex library
    /*
        (\w+) Word Characters
        (\.|\_)? Special characters like a .
        (\w*) You cant have anymore word characters
        @ Seperates the domain
        (\w+)Matches the domain
        \.(\w+))

    */
    string Email;

    while (true) { // Loops if the user enters it wrong
        cout << "Enter your email: ";
        getline(cin, Email); // Gets the users input

        if (!regex_match(Email, emailPattern)) {
            cout << "Invalid EMAIL Format, Use XXXXX@XXXX.XXXX" << endl; // Error message
            continue;
        }
        break;
    }
    return Email;
}

string ValidatePhone() {
    regex phonePattern(R"(^(\+44|0)7\d{9}$|^(\+44|0)7\d{3}[\s\-]?\d{3}[\s\-]?\d{3}$)"); // Phone number format using regex library 
    /*
        ^ Start of the string
        $ End of the string
        (\+44|0) UK Country code | Means or 
        7\d{9} Number starting with 7 with 9 digits

        |^(\+44|0)7\d{3}[\s\-]?\d{3}[\s\-]?\d{3} Optional spaces and dashes
            ^(\+44|0) dialing code
            7, 7 starting digits
            \d{3}, 3 digitls 
            [\s\-]? optional characters due to the question mark
            This was tricky to understand
    */      
    string phone;

    while (true) {
        cout << "Enter your Phone Number: ";
        getline(cin, phone); // Gets the phone number from the user

        if (!regex_match(phone, phonePattern)) {
            cout << "Invalid Phone Format, use 07XX..." << endl; // Error message
            continue;
        }
        break;
    }
    return phone;
}

class Titles { // Titles class
private:
    sql::Connection* con; // Database connection

public:
    Titles(sql::Connection* connection) : con(connection) {} // Constructor which passes the database connection into the class, required part of c++ connector

    void ViewTitles() { // View titles method
        try {
            sql::Statement* stmt = con->createStatement(); // Creates SQL Statements to execute
            sql::ResultSet* res = stmt->executeQuery("SELECT * FROM booktitles");

            cout << left << setw(6) << "ID" << left << setw(52) << "Title" << left << setw(33) << "Author" << left << setw(8) << "Year" << left << setw(16) << "Genre" << endl; // Displaying titles
            while (res->next()) { // Basically a for loop, If there is another result in the set jump to the next one
                cout << setw (5) << res->getInt("book_title_id") <<  " | " 
                    << setw(50) << res->getString("title") << " | "
                    << setw(30) << res->getString("author") << " | "
                    << setw(4) << res->getInt("published_year") << " | "
                    << setw(15) << res->getString("genre") << endl;
            }

            // Deleting unneeded resources, good coding practice
            delete res;
            delete stmt;
        }
        catch (sql::SQLException& e) { // Error class part of c++ connector, it catches an error when something goes wrong when interacting with the database
            cerr << "Error viewing book titles: " << e.what() << endl;
        }
    }

    void AddTitle() { // Adding a title to the database method
        BookTitles currentTitle; // Creating the instance of the struct
        cout << "Enter The Title of the book: " << endl;
        cin.ignore(); // Clears the input just incase, I previously had some errors taking in user input without this.
        getline(cin, currentTitle.title);

        cout << "Enter The author of the book: " << endl; // Getting user inputs
        getline(cin, currentTitle.author);

        cout << "What year was the book published: (YYYY)" << endl;
        currentTitle.published_year = TypeCheck();

        cout << "Enter The Genre of the book: " << endl;
        cin.ignore(); // Clear newline after TypeCheck if it uses cin
        getline(cin, currentTitle.genre);
        
        try { // Try catch statement for validation
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "INSERT INTO booktitles (title, author, published_year, genre) VALUES (?, ?, ?, ?)");  // SQL statement inserting into database
            pstmt->setString(1, currentTitle.title); // Sets the string in the prepared statement
            pstmt->setString(2, currentTitle.author);
            pstmt->setInt(3, currentTitle.published_year);
            pstmt->setString(4, currentTitle.genre);
            pstmt->execute();   // Executes the SQL statement
            delete pstmt;
            cout << "Book title added successfully.\n";
        }
        catch (sql::SQLException& e) { // Error class part of c++ connector, it catches an error when something goes wrong when interacting with the database
            cerr << "Error adding book title: " << e.what() << endl; // Output stream specifically for error messages
        }
    }

    void UpdateTitle() { // Updating title method in date base
        BookTitles currentTitle;
        
        cout << "Enter The ID of the book you would like to edit: " << endl; // Gets the ID of the title you would like to edit
        cin.ignore(); // Clear leftover newline if needed
        cin >> currentTitle.book_title_id;

        cout << endl;   // New information
        cout << "New details of the book" << endl;
        cout << "-----------------------" << endl;
        cout << "Enter The Title of the book: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        getline(cin, currentTitle.title);

        cout << "Enter The author of the book: " << endl;
        getline(cin, currentTitle.author);

        cout << "What year was the book published: (YYYY)" << endl;
        currentTitle.published_year = TypeCheck();

        cout << "Enter The Genre of the book: " << endl;
        cin.ignore(); // Clear newline after TypeCheck if it uses cin
        getline(cin, currentTitle.genre);

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "UPDATE booktitles SET title = ?, author = ?, published_year = ?, genre = ? WHERE book_title_id = ?"); // SQL statement Updating details where ID matches
            pstmt->setString(1, currentTitle.title);
            pstmt->setString(2, currentTitle.author);
            pstmt->setInt(3, currentTitle.published_year);
            pstmt->setString(4, currentTitle.genre);
            pstmt->setInt(5, currentTitle.book_title_id);
            pstmt->execute();
            delete pstmt;
            cout << "Book title updated successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error editing book title: " << e.what() << endl;
        }
    }
    void DeleteTitle() { // Method for deleting a title
        BookTitles currentTitle;
        cout << "Enter Book Title ID to delete"; // Asks the user for which ID to delete
        currentTitle.book_title_id = TypeCheck();

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "DELETE FROM booktitles WHERE book_title_id = ?"); // Deletes the specific ID book title
            pstmt->setInt(1, currentTitle.book_title_id);
            pstmt->execute();
            delete pstmt;
            cout << "Book title deleted successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error deleting book title: " << e.what() << endl;
        }
    }

};

class Users { // Users class, contains methods to deal with any user related queries 
private:
    sql::Connection* con; // Database connection
public:
    Users(sql::Connection* connection) : con(connection) {} // Constructor for the database connection

    void ViewUsers() { // Method for displaying all users
        try {
            sql::Statement* stmt = con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT * FROM users"); // SQL statement selecting all from users

            cout << left << setw(6) << "ID" << setw(25) << "Name" << setw(30) << "Email" << setw(15) << "Phone" << endl; // SetW sets the width of each column, good for outputs
            while (res->next()) {
                cout << left << setw(6) << res->getInt("user_id")
                    << setw(25) << res->getString("name")
                    << setw(30) << res->getString("email")
                    << setw(15) << res->getString("phone") << endl;
            }

            delete res;
            delete stmt;
        }
        catch (sql::SQLException& e) { // Error Messages
            cerr << "Error viewing users: " << e.what() << endl;
        }
    }
    void AddUser() { // Method for adding users
        LibraryUsers currentUser; // Creating the instance of the struct, which is then applied to the database
        cout << "Enter the full name of the user: " << endl;
        cin.ignore(); // Needed, caused me errors grabing user input
        getline(cin, currentUser.name);

        currentUser.email = ValidateEmail(); // Gets and validates users email

        currentUser.phone = ValidatePhone(); // Gets and validates user's phoneNumber

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "INSERT INTO users (name, email, phone) VALUES (?, ?, ?)"); // SQL statement
            pstmt->setString(1, currentUser.name);
            pstmt->setString(2, currentUser.email);
            pstmt->setString(3, currentUser.phone);
            pstmt->execute(); // Executes
            delete pstmt;
            cout << "User added successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error adding user: " << e.what() << endl;
        }
    }
    void UpdateUser() { // Method for updating user data
        LibraryUsers currentUser;
        cout << "Enter The ID of the User you would like to edit: " << endl;
        cin.ignore(); // Required, caused me errors grabing user input
        cin >> currentUser.user_id;
        cout << endl;
        cout << "New details of the user" << endl; // New deta to be updated
        cout << "-----------------------" << endl;
        cout << "Enter the full name of the user: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        getline(cin, currentUser.name);

        currentUser.email = ValidateEmail(); // Gets and validates users email

        currentUser.phone = ValidatePhone(); // Gets and validates user's phoneNumber

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "UPDATE users SET name = ?, email = ?, phone = ? WHERE user_id = ?"); // SQL satement
            pstmt->setString(1, currentUser.name);
            pstmt->setString(2, currentUser.email);
            pstmt->setString(3, currentUser.phone);
            pstmt->setInt(4, currentUser.user_id);
            pstmt->execute();
            delete pstmt;
            cout << "User updated successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error editing user: " << e.what() << endl;
        }

    }

    void DeleteUser() { // Deletes user method based on ID
        LibraryUsers currentUser;
        cout << "Enter User ID to delete";
        currentUser.user_id = TypeCheck();

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "DELETE FROM users WHERE user_id = ?");
            pstmt->setInt(1, currentUser.user_id);
            pstmt->execute();
            delete pstmt;
            cout << "User deleted successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error deleting user: " << e.what() << endl;
        }
    }
};

class copies { // Copies Class
private:
    sql::Connection* con; // Database connection
public:
    copies(sql::Connection* connection) : con(connection) {} // Constructor which passes in the database connection to the methods

    void ViewCopies() { // View copies Method
        try {
            sql::Statement* stmt = con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT * FROM bookcopies"); // SQL statement

            cout << left << setw(10) << "CopyID" << setw(15) << "TitleID" << setw(15) << "Available" << endl;
            while (res->next()) {
                cout << left << setw(10) << res->getInt("copy_id")
                    << setw(15) << res->getInt("book_title_id")
                    << setw(15) << (res->getBoolean("available") ? "Yes" : "No") << endl;
            }

            delete res;
            delete stmt;
        }
        catch (sql::SQLException& e) { // Catches error
            cerr << "Error viewing book copies: " << e.what() << endl;
        }
    }
    void AddCopy() { // Adds copy method
        BookCopies currentCopy;
        cout << "Enter the ID of the book you would like to add a copy of: " << endl;
        currentCopy.book_title_id = TypeCheck();
        currentCopy.available = true;

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "INSERT INTO bookcopies (book_title_id, available) VALUES (?, ?)");
            pstmt->setInt(1, currentCopy.book_title_id);
            pstmt->setBoolean(2, currentCopy.available);
            pstmt->execute();
            delete pstmt;
            cout << "Book copy added successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error adding book copy: " << e.what() << endl;
        }
    }
    void UpdateCopy() { // Update copy method
        BookCopies currentCopy;
        cout << "Enter the ID of the book copy you would like edit: " << endl;
        currentCopy.copy_id = TypeCheck();
        cout << endl;
        cout << "New details of the book copy" << endl;
        cout << "-----------------------" << endl;
        cout << "Enter the ID of the new book title your adding a copy off (If only changing availablity Leave as is)" << endl;
        currentCopy.book_title_id = TypeCheck();

        cout << "Enter Book Availability: (true/false) ";
        cin >> currentCopy.available;

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "UPDATE bookcopies SET book_title_id = ?, available = ? WHERE copy_id = ?");
            pstmt->setInt(1, currentCopy.book_title_id);
            pstmt->setBoolean(2, currentCopy.available);
            pstmt->setInt(3, currentCopy.copy_id);
            pstmt->execute();
            delete pstmt;
            cout << "Book copy updated successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error editing book copy: " << e.what() << endl;
        }
    }

    void RemoveCopy() { // Deletes the copy methods
        BookCopies currentCopy;
        cout << "Enter the ID of the copy you would like to remove: " << endl;
        currentCopy.copy_id = TypeCheck();

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "DELETE FROM bookcopies WHERE copy_id = ?");
            pstmt->setInt(1, currentCopy.copy_id);
            pstmt->execute();
            delete pstmt;
            cout << "Book copy deleted successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error deleting book copy: " << e.what() << endl;
        }
    }
};

class Borrowed { // Borrowed books Class
private:
    sql::Connection* con;

public:
    Borrowed(sql::Connection* connection) : con(connection) {}

    void ViewBorrowed() {
        try {
            sql::Statement* stmt = con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT * FROM books_borrowed");

            cout << left << setw(10) << "BorrowID" << setw(10) << "CopyID" << setw(10) << "UserID" << setw(30) << "Date Borrowed" << setw(20) << "Return Date" << setw(20) << "Status" << endl;
            while (res->next()) {
                cout << left << setw(10) << res->getInt("borrow_id")
                    << setw(10) << res->getInt("copy_id")
                    << setw(10) << res->getInt("user_id")
                    << setw(30) << res->getString("date_borrowed")
                    << setw(20) << res->getString("return_date")
                    << setw(20) << res->getString("status") << endl;
            }
            delete res;
            delete stmt;
        }
        catch (sql::SQLException& e) {
            cerr << "Error viewing borrowed books: " << e.what() << endl;
        }
    }
    void AddBorrowed(){
        BorrowedBooks currentBorrowed;
        cout << "Enter the ID of the copy you are borrowing:  " << endl;
        currentBorrowed.copy_id = TypeCheck();
        cout << endl;

        cout << "Enter the ID of the user borrowing the book:  " << endl;
        currentBorrowed.user_id = TypeCheck();
        cout << endl;

        currentBorrowed.return_date = GetDate("What is the return Date: ");
        cout << endl;

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "INSERT INTO books_borrowed (copy_id, user_id, return_date, status) VALUES (?, ?, ?, ?)");
            pstmt->setInt(1, currentBorrowed.copy_id);
            pstmt->setInt(2, currentBorrowed.user_id);
            pstmt->setString(3, currentBorrowed.return_date);
            pstmt->setString(4, "Borrowed");  // Set manually
            pstmt->execute();
            delete pstmt;
            cout << "Borrowed book record added successfully.\n";

            // Sets copy availability To false
            pstmt = con->prepareStatement(
                "UPDATE bookcopies SET available = false WHERE copy_id = ?");
            pstmt->setInt(1, currentBorrowed.copy_id);
            pstmt->execute();
            delete pstmt;

        }
        catch (sql::SQLException& e) {
            cerr << "Error adding borrowed book: " << e.what() << endl;
        }
    }
    void ReturnBook() {
        BorrowedBooks currentBorrowed;
        cout << "Enter the ID of the borrowed book:  " << endl;
        currentBorrowed.borrow_id = TypeCheck();
        cout << endl;

        // Set the Book Copy as available now since it was returned
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE bookcopies SET available = 1 WHERE copy_id = ?");
        pstmt->setInt(1, currentBorrowed.copy_id);
        pstmt->execute();
        pstmt = con->prepareStatement(
            "UPDATE books_borrowed SET status = 'Returned' WHERE borrow_id = ?");
        pstmt->setInt(1, currentBorrowed.borrow_id);
        pstmt->execute();
        delete pstmt;
    }
    void DeleteBorrowRecord() {
        BorrowedBooks currentBorrowed;
        cout << "Enter the ID of the borrowed book:  " << endl;
        currentBorrowed.borrow_id = TypeCheck();
        cout << endl;

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "DELETE FROM books_borrowed WHERE borrow_id = ?");
            pstmt->setInt(1, currentBorrowed.borrow_id);
            pstmt->execute();
            delete pstmt;
            cout << "Borrow Record deleted successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error deleting Borrow records: " << e.what() << endl;
        }
    }
};

class Search { // Search Class
private:
    sql::Connection* con;

public:
    Search(sql::Connection* connection) : con(connection) {}

    void UserSearch() {
        LibraryUsers currentUser;
        cout << "Enter the full name of the user you are searching for: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        getline(cin, currentUser.name);

        char UserInput;
        cout << "Do you want to save the results to a text file? (y/n): "; // Asks the User if they would like to 
        cin >> UserInput;
        cin.ignore(); // Clear leftover newline if needed

        // Open the file for writing if the user chooses 'y'
        ofstream File;
        if (UserInput == 'y' || UserInput == 'Y') {
            File.open("User_Search_Results.txt");
            if (File.is_open()) {
                File << left << setw(6) << "ID"
                    << left << setw(25) << "Name"
                    << left << setw(30) << "Email"
                    << left << setw(15) << "Phone" << endl;
            }
        }

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "SELECT * FROM users WHERE name LIKE ?");
            pstmt->setString(1, "%" + currentUser.name + "%");

            sql::ResultSet* res = pstmt->executeQuery();

            cout << left << setw(6) << "ID" << setw(25) << "Name" << setw(30) << "Email" << setw(15) << "Phone" << endl;
            while (res->next()) {
                cout << left << setw(6) << res->getInt("user_id")
                    << setw(25) << res->getString("name")
                    << setw(30) << res->getString("email")
                    << setw(15) << res->getString("phone") << endl;

                if (UserInput == 'y' || UserInput == 'Y') {
                    File << setw(6) << res->getInt("user_id") << " | "
                        << setw(25) << res->getString("name") << " | "
                        << setw(30) << res->getString("email") << " | "
                        << setw(15) << res->getString("phone") << endl;
                }
            }
            delete res;
            delete pstmt;
        }
        catch (sql::SQLException& e) {
            cerr << "Error searching users: " << e.what() << endl;
        }



    }
    void TitleSearch() {
        BookTitles currentTitle;
        cout << "Enter the Title of the book you are searching for: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        getline(cin, currentTitle.title);

        char UserInput;
        cout << "Do you want to save the results to a text file? (y/n): ";
        cin >> UserInput;
        cin.ignore(); // Clear leftover newline if needed

        ofstream File;
        if (UserInput == 'y' || UserInput == 'Y') {
            File.open("Title_Search_Results.txt");
            if (File.is_open()) {
                File << left << setw(6) << "ID" << left << setw(52) << "Title" << left << setw(33) << "Author" << left << setw(8) << "Year" << left << setw(16) << "Genre" << endl;
            }
        }

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "SELECT * FROM booktitles WHERE title LIKE ?");
            pstmt->setString(1, "%" + currentTitle.title + "%");

            sql::ResultSet* res = pstmt->executeQuery();

            cout << left << setw(6) << "ID" << left << setw(52) << "Title" << left << setw(33) << "Author" << left << setw(8) << "Year" << left << setw(16) << "Genre" << endl;
            while (res->next()) {
                cout << setw(5) << res->getInt("book_title_id") << " | "
                    << setw(50) << res->getString("title") << " | "
                    << setw(30) << res->getString("author") << " | "
                    << setw(4) << res->getInt("published_year") << " | "
                    << setw(15) << res->getString("genre") << endl;

                if (UserInput == 'y' || UserInput == 'Y') {
                    File << setw(5) << res->getInt("book_title_id") << " | "
                        << setw(50) << res->getString("title") << " | "
                        << setw(30) << res->getString("author") << " | "
                        << setw(4) << res->getInt("published_year") << " | "
                        << setw(15) << res->getString("genre") << endl;
                }

            }
            delete res;
            delete pstmt;
        }
        catch (sql::SQLException& e) {
            cerr << "Error searching users: " << e.what() << endl;
        }
    }
};

// Function to get the database password from the user, the password isnt just hardcoded
string GetDatabasePassword() { 
    string Password;
    cout << "Enter MySQL password: ";
    cin >> Password;  // For further improvement, add character hiding
    return Password;
}

// Database constants
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = GetDatabasePassword();



// Main menu function
void MainMenu(sql::Connection* con) {

    // Setting Class instances
    Users UserManager(con);
    Titles TitlesManager(con);
    copies CopiesManager(con);
    Borrowed Borrowed_BooksManager(con);
    Search SearchManager(con);

    // Array of options the user may choose,
    string MainOptions[6] = { "Manage users", "Manage book titles", "Manage book copies", "Manage borrowed books", "Search", "Quit" };
    string SearchOptions[3] = { "Search for a user", "Search for a book title", "Main menu" };
    string UserOptions[5] = { "View all users", "Add User", "Edit User", "Delete User", "Main menu" };
    string BookTitleOptions[5] = { "View all book titles", "Add new book title", "Edit book title", "Delete book title", "Main menu" };
    string BookCopyOptions[5] = { "View all copies of a book title","Add copy to a book title", "Edit Book Copy", "Remove Book Copy" ,"Main menu" };
    string BorrowOptions[5] = { "View records", "Borrow a book", "Return a Book", "Delete Record", "Main menu"};

    int UserInput; // Declaring Users input

    do { // Repeats the program until the user quits
        cout << "\nWelcome to the Library Management App" << endl;
        cout << "---------------------------------------" << endl;
        cout << "Choose your option - use the number" << endl;
        cout << "#" << "\t" << "Option" << endl;;

        for (int option = 0; option < sizeof(MainOptions) / sizeof(MainOptions[0]); option++) { // For loop displaying every option
            cout << option << "\t" << MainOptions[option] << endl;
        }

        UserInput = TypeCheck(); // Validates the users input as a number

        switch (UserInput) { // Condition to check what the user entered
        case 0: { 
            int userChoice;
            do { // 2nd do while to allow the user to retur to main menu
                cout << "\nManage Users" << endl;
                cout << "--------------" << endl;
                for (int option = 0; option < sizeof(UserOptions) / sizeof(UserOptions[0]); option++) {
                    cout << option << "\t" << UserOptions[option] << endl;
                }
                userChoice = TypeCheck();

                switch (userChoice) {
                case 0:
                    UserManager.ViewUsers();
                    break;
                case 1:
                    UserManager.AddUser();
                    break;
                case 2:
                    UserManager.UpdateUser();
                    break;
                case 3:
                    UserManager.DeleteUser();
                    break;
                case 4:
                    cout << "Back to Main Menu\n";
                    break;
                default:
                    cout << "Invalid choice in user menu.\n"; // Error message range check
                    break;
                }
            } while (userChoice != 4);
            break;
        }

        case 1: { // Manage Book Titles
            int bookTitleChoice;
            do {
                cout << "\nManage Book Titles" << endl;
                cout << "-----------------------------" << endl;
                for (int option = 0; option < sizeof(BookTitleOptions) / sizeof(BookTitleOptions[0]); option++) { // For loop displaying title options
                    cout << option << "\t" << BookTitleOptions[option] << endl;
                }
                bookTitleChoice = TypeCheck();

                switch (bookTitleChoice) {
                case 0:
                    TitlesManager.ViewTitles();
                    break;
                case 1:
                    TitlesManager.AddTitle();
                    break;
                case 2:
                    TitlesManager.UpdateTitle();
                    break;
                case 3:
                    TitlesManager.DeleteTitle();
                    break;
                case 4:
                    cout << "Back to Main Menu..\n";
                    break;
                default:
                    cout << "Invalid choice in book title menu.\n"; // Error message
                    break;
                }
            } while (bookTitleChoice != 4); // Returns to main menu
            break;
        }

        case 2: { // Manage Book Copies
            int copyChoice;
            do {
                cout << "\nManage Book Copies" << endl;
                cout << "--------------------" << endl;
                for (int option = 0; option < sizeof(BookCopyOptions) / sizeof(BookCopyOptions[0]); option++) { // For loop Displaying Copy option
                    cout << option << "\t" << BookCopyOptions[option] << endl;
                }
                copyChoice = TypeCheck();

                switch (copyChoice) {
                case 0:
                    CopiesManager.ViewCopies();
                    break;
                case 1:
                    CopiesManager.AddCopy();
                    break;
                case 2:
                    CopiesManager.UpdateCopy();
                    break;
                case 3:
                    CopiesManager.RemoveCopy();
                    break;
                case 4:
                    cout << "Returning to Main Menu...\n";
                    break;
                default:
                    cout << "Invalid choice in book copy menu.\n";
                    break;
                }
            } while (copyChoice != 4); // Returns to main menu
            break;
        }

        case 3: { // Manage Borrowed Books
            int borrowChoice;
            do {
                cout << "\nManage Borrowed Books" << endl;
                cout << "-----------------------" << endl;
                for (int option = 0; option < sizeof(BorrowOptions) / sizeof(BorrowOptions[0]); option++) { // Displays Borrow options
                    cout << option << "\t" << BorrowOptions[option] << endl;
                }
                borrowChoice = TypeCheck(); // Type check validation

                switch (borrowChoice) {
                case 0:
                    Borrowed_BooksManager.ViewBorrowed();
                    break;
                case 1:
                    Borrowed_BooksManager.AddBorrowed();
                    break;
                case 2:
                    Borrowed_BooksManager.ReturnBook();
                    break;
                case 3:
                    Borrowed_BooksManager.DeleteBorrowRecord();
                    break;
                case 4:
                    cout << "Back to Main Menu...\n";
                    break;
                default:
                    cout << "Invalid choice in this menu\n"; // Error message
                    break;
                }
            } while (borrowChoice != 4); // Returns to main menu
            break;
        }

        case 4: { // Search Options
            int searchChoice;
            do {
                cout << "\nSearch Menu" << endl; // Menu for searching
                cout << "-------------" << endl;
                for (int option = 0; option < sizeof(SearchOptions) / sizeof(SearchOptions[0]); option++) { // Displays Search options from the array
                    cout << option << "\t" << SearchOptions[option] << endl;
                }

                searchChoice = TypeCheck();

                switch (searchChoice) {
                case 0:
                    SearchManager.UserSearch();
                    break;
                case 1:
                    SearchManager.TitleSearch();
                    break;
                case 2:
                    cout << "Returning to main menu\n";
                    break;
                default:
                    cout << "Invalid choice in search menu.\n";
                    break;
                }
            } while (searchChoice != 2); // Returns to main menu
            break;
        }

        case 5:
            cout << "Exiting, Goodbye\n";
            break;

        default:
            cout << "Invalid input\n";
            break;
        }

    } while (UserInput != 5);
}

int main()
{
    // Variable declarations using SQL connector, Gotten from MySQL connector documentation
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    try // Try catch statement for validation and error handling if the system cannot connect to the database
    {
        // Get MySQL driver instance (Gets the specific MySQL program that is running to manage the database)
        driver = get_driver_instance();
        // Logs into the database using user credentials
        con = driver->connect(server, username, password);
        //Sets which database to use, The system can store multiple, I only require 1
        con->setSchema("librarysystem");

        MainMenu(con);
 
    }
    catch (sql::SQLException& e) // Error message if the system could not connect to the database
    {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }
    return 0;
}

