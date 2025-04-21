// Library Management System by Konrad Zajac (N1179676) 2025

// Libraries
#include <stdlib.h>
#include <iostream>
#include <string>
#include <iomanip>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
using namespace std;


// Structs For data storage, Temporary used for holding data whilst accessing the database etc

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

class Titles {
private:
    sql::Connection* con;

public:
    Titles(sql::Connection* connection) : con(connection) {}

    void ViewTitles() {
        try {
            sql::Statement* stmt = con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT * FROM booktitles");

            cout << left << setw(6) << "ID" << left << setw(52) << "Title" << left << setw(33) << "Author" << left << setw(8) << "Year" << left << setw(16) << "Genre" << endl;
            while (res->next()) {
                cout << setw (5) << res->getInt("book_title_id") <<  " | " 
                    << setw(50) << res->getString("title") << " | "
                    << setw(30) << res->getString("author") << " | "
                    << setw(4) << res->getInt("published_year") << " | "
                    << setw(15) << res->getString("genre") << endl;
            }

            // Cleaning Up resources, Frees up memory
            delete res;
            delete stmt;
        }
        catch (sql::SQLException& e) {
            cerr << "Error viewing book titles: " << e.what() << endl;
        }
    }

    void AddTitle() {
        BookTitles currentTitle; // Creating the instance of the struct
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
                "INSERT INTO booktitles (title, author, published_year, genre) VALUES (?, ?, ?, ?)");
            pstmt->setString(1, currentTitle.title);
            pstmt->setString(2, currentTitle.author);
            pstmt->setInt(3, currentTitle.published_year);
            pstmt->setString(4, currentTitle.genre);
            pstmt->execute();
            delete pstmt;
            cout << "Book title added successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error adding book title: " << e.what() << endl;
        }
    }

    void UpdateTitle() {
        BookTitles currentTitle;
        
        cout << "Enter The ID of the book you would like to edit: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        cin >> currentTitle.book_title_id;

        cout << endl;
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
                "UPDATE booktitles SET title = ?, author = ?, published_year = ?, genre = ? WHERE book_title_id = ?");
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

    void DeleteTitle() {
        BookTitles currentTitle;
        cout << "Enter Book Title ID to delete";
        currentTitle.book_title_id = TypeCheck();

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "DELETE FROM booktitles WHERE book_title_id = ?");
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

class Users {
private:
    sql::Connection* con;
public:
    Users(sql::Connection* connection) : con(connection) {}

    void ViewUsers() {
        try {
            sql::Statement* stmt = con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT * FROM users");

            cout << left << setw(6) << "ID" << setw(25) << "Name" << setw(30) << "Email" << setw(15) << "Phone" << endl;
            while (res->next()) {
                cout << left << setw(6) << res->getInt("user_id")
                    << setw(25) << res->getString("name")
                    << setw(30) << res->getString("email")
                    << setw(15) << res->getString("phone") << endl;
            }

            delete res;
            delete stmt;
        }
        catch (sql::SQLException& e) {
            cerr << "Error viewing users: " << e.what() << endl;
        }
    }
    void AddUser() {
        LibraryUsers currentUser; // Creating the instance of the struct
        cout << "Enter the full name of the user: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        getline(cin, currentUser.name);

        cout << "Enter the email of the user: " << endl;
        getline(cin, currentUser.email);

        cout << "Enter the Phone number of the user: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        getline(cin, currentUser.phone);

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "INSERT INTO users (name, email, phone) VALUES (?, ?, ?)");
            pstmt->setString(1, currentUser.name);
            pstmt->setString(2, currentUser.email);
            pstmt->setString(3, currentUser.phone);
            pstmt->execute();
            delete pstmt;
            cout << "User added successfully.\n";
        }
        catch (sql::SQLException& e) {
            cerr << "Error adding user: " << e.what() << endl;
        }
    }
    void UpdateUser() {
        LibraryUsers currentUser;
        cout << "Enter The ID of the User you would like to edit: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        cin >> currentUser.user_id;
        cout << endl;
        cout << "New details of the user" << endl;
        cout << "-----------------------" << endl;
        cout << "Enter the full name of the user: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        getline(cin, currentUser.name);

        cout << "Enter the email of the user: " << endl;
        getline(cin, currentUser.email);

        cout << "Enter the Phone number of the user: " << endl;
        cin.ignore(); // Clear leftover newline if needed
        getline(cin, currentUser.phone);

        try {
            sql::PreparedStatement* pstmt = con->prepareStatement(
                "UPDATE users SET name = ?, email = ?, phone = ? WHERE user_id = ?");
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

    void DeleteUser() {
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

class copies {
private:
    sql::Connection* con;
public:
    copies(sql::Connection* connection) : con(connection) {}

    void ViewCopies() {
        try {
            sql::Statement* stmt = con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT * FROM bookcopies");

            cout << left << setw(10) << "CopyID" << setw(15) << "TitleID" << setw(15) << "Available" << endl;
            while (res->next()) {
                cout << left << setw(10) << res->getInt("copy_id")
                    << setw(15) << res->getInt("book_title_id")
                    << setw(15) << (res->getBoolean("available") ? "Yes" : "No") << endl;
            }

            delete res;
            delete stmt;
        }
        catch (sql::SQLException& e) {
            cerr << "Error viewing book copies: " << e.what() << endl;
        }
    }
    void AddCopy() {
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
    void UpdateCopy() {
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

    void RemoveCopy() {
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

class Borrowed {
private:
    sql::Connection* con;

public:
    Borrowed(sql::Connection* connection) : con(connection) {}

    void viewBorrowed() {
        try {
            sql::Statement* stmt = con->createStatement();
            sql::ResultSet* res = stmt->executeQuery("SELECT * FROM books_borrowed");

            cout << left << setw(10) << "BorrowID" << setw(10) << "CopyID" << setw(10) << "UserID" << setw(20) << "Date Borrowed" << setw(20) << "Return Date" << endl;
            while (res->next()) {
                cout << left << setw(10) << res->getInt("borrow_id")
                    << setw(10) << res->getInt("copy_id")
                    << setw(10) << res->getInt("user_id")
                    << setw(20) << res->getString("date_borrowed")
                    << setw(20) << res->getString("return_date") << endl;
            }

            delete res;
            delete stmt;
        }
        catch (sql::SQLException& e) {
            cerr << "Error viewing borrowed books: " << e.what() << endl;
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

    // List of options the user may choose
    string MainOptions[6] = { "Manage users", "Manage book titles", "Manage book copies", "Manage borrowed books", "Search", "Quit" };
    string SearchOptions[4] = { "Search for a user", "Search for a Book", "Search for borrowed books", "Main menu" };
    string UserOptions[5] = { "View all users", "Add User", "Edit User", "Delete User", "Main menu" };
    string BookTitleOptions[5] = { "View all book titles", "Add new book title", "Edit book title", "Delete book title", "Main menu" };
    string BookCopyOptions[5] = { "View all copies of a book title","Add copy to a book title", "Edit Book Copy", "Remove Book Copy" ,"Main menu" };
    string BorrowOptions[4] = { "View records", "Borrow a book", "View Borrowed books by a user", "Main menu" };

    int UserInput; // Declaring Users input

    do { // Repeats the program until the user quits
        cout << "\nWelcome to the Library Management App" << endl;
        cout << "---------------------------------------" << endl;
        cout << "Choose your option - use the number" << endl;
        cout << "#" << "\t" << "Option" << endl;;

        for (int option = 0; option < sizeof(MainOptions) / sizeof(MainOptions[0]); option++) { // Displays Main options
            cout << option << "\t" << MainOptions[option] << endl;
        }

        UserInput = TypeCheck(); // Validates the users input as a number

        switch (UserInput) { // Condition to check what the user entered
        case 0: { // Manage Users
            int userChoice;
            do { // 2nd do while to allow the user to retun
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
                    cout << "Returning to Main Menu...\n";
                    break;
                default:
                    cout << "Invalid choice in user menu.\n";
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
                for (int option = 0; option < sizeof(BookTitleOptions) / sizeof(BookTitleOptions[0]); option++) {
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
                    cout << "Returning to Main Menu...\n";
                    break;
                default:
                    cout << "Invalid choice in book title menu.\n";
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
                for (int option = 0; option < sizeof(BookCopyOptions) / sizeof(BookCopyOptions[0]); option++) {
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
                for (int option = 0; option < sizeof(BorrowOptions) / sizeof(BorrowOptions[0]); option++) {
                    cout << option << "\t" << BorrowOptions[option] << endl;
                }
                borrowChoice = TypeCheck();

                switch (borrowChoice) {
                case 0:
                    Borrowed_BooksManager.viewBorrowed();
                    break;
                case 1:
                    cout << "Returning a book...\n";
                    break;
                case 2:
                    cout << "Viewing borrowed books by user...\n";
                    break;
                case 3:
                    cout << "Returning to Main Menu...\n";
                    break;
                default:
                    cout << "Invalid choice in borrow menu.\n";
                    break;
                }
            } while (borrowChoice != 3); // Returns to main menu
            break;
        }

        case 4: { // Search
            int searchChoice;
            do {
                cout << "\nSearch Menu" << endl;
                cout << "-------------" << endl;
                for (int option = 0; option < sizeof(SearchOptions) / sizeof(SearchOptions[0]); option++) {
                    cout << option << "\t" << SearchOptions[option] << endl;
                }

                searchChoice = TypeCheck();

                switch (searchChoice) {
                case 0:
                    cout << "Searching for a user...\n";
                    break;
                case 1:
                    cout << "Searching for a book...\n";
                    break;
                case 2:
                    cout << "Searching borrowed books...\n";
                    break;
                case 3:
                    cout << "Returning to Main Menu...\n";
                    break;
                default:
                    cout << "Invalid choice in search menu.\n";
                    break;
                }
            } while (searchChoice != 3); // Returns to main menu
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
    // Variable declarations using SQL connector
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
    
}

/*
// Execute the query
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT DISTINCT author FROM booktitles");

        cout << "Authors:\n";
        while (res->next()) {
            string author = res->getString("author");
            cout << author << endl;
        }

*/

// Clean up resources
        //delete res;
       //delete stmt;
        //delete con;


//    while (true) {
//        cout << "\n";
//        switch (UserInput) {
//        case 0:
//            cout << "Manage Users" << endl;
//            cout << "------------" << endl;
//            cout << "Choose your option - use the number" << endl;
//            cout << "#" << "\t" << "Option" << endl;
//
//            for (int option = 0; option < sizeof(UserOptions) / sizeof(UserOptions[0]); option++) {
//                cout << option << "\t" << UserOptions[option] << endl;
//            }
//
//            
//             
//            switch (UserInput) {
//            case 0:
//              // call function to view users
//            case 1:
//                // call function to add user
//            case 2:
//                // call function to edit user
//            case 3:
//                // call function to delete user
//            case 4:
//                cout << "Returning to main menu...\n";
//                break;
//            }
//        case 1:
//            cout << "Manage Book Titles" << endl;
//            cout << "------------" << endl;
//            cout << "Choose your option - use the number" << endl;
//            cout << "#" << "\t" << "Option" << endl;
//
//            for (int option = 0; option < sizeof(BookTitleOptions) / sizeof(BookTitleOptions[0]); option++) {
//                cout << option << "\t" << BookTitleOptions[option] << endl;
//            }
//
//           
//            switch (UserInput) {
//            case 0:
//                // call function to view Book Titles
//            case 1:
//                // call function to add Book Titles
//            case 2:
//                // call function to edit Book Titles
//            case 3:
//                // call function to delete Book Titles
//            case 4:
//                cout << "Returning to main menu...\n";
//                break;
//            }
//            break;
//            
//        }
//        
//
//    }
//
//
//
//
//
//
//
//
//
//    return 0;