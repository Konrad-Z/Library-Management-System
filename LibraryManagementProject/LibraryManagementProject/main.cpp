// Library Management System by Konrad Zajac (N1179676) 2025

// Libraries
#include <stdlib.h>
#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
using namespace std;

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

// Function for validating menu input
int TypeCheck() {
    int choice;
    while (true) {
        cout << "Enter your choice: ";
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

void MainMenu() { // Main menu function
    // List of options the user may choose
    string MainOptions[6] = { "Manage users", "Manage book titles", "Manage book copies", "Manage borrowed books", "Search", "Quit" };
    string SearchOptions[4] = { "Search for a user", "Search for a Book", "Search for borrowed books", "Main menu" };
    string UserOptions[5] = { "View all users", "Add User", "Edit User", "Delete User", "Main menu" };
    string BookTitleOptions[5] = { "View all book titles", "Add new book title", "Edit book title", "Delete book title", "Main menu" };
    string BookCopyOptions[4] = { "View all copies of a book title","Add copy to a book title", "Change copy availability", "Main menu" };
    string BorrowOptions[4] = { "Borrow a book", "Return a book", "View Borrowed books by a user", "Main menu" };

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
                    cout << "Viewing all users...\n";
                    break;
                case 1:
                    cout << "Adding a user...\n";
                    break;
                case 2:
                    cout << "Editing a user...\n";
                    break;
                case 3:
                    cout << "Deleting a user...\n";
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
                    cout << "Viewing all book titles...\n";
                    break;
                case 1:
                    cout << "Adding a book title...\n";
                    break;
                case 2:
                    cout << "Editing a book title...\n";
                    break;
                case 3:
                    cout << "Deleting a book title...\n";
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
                    cout << "Viewing all book copies...\n";
                    break;
                case 1:
                    cout << "Adding book copy...\n";
                    break;
                case 2:
                    cout << "Changing copy availability...\n";
                    break;
                case 3:
                    cout << "Returning to Main Menu...\n";
                    break;
                default:
                    cout << "Invalid choice in book copy menu.\n";
                    break;
                }
            } while (copyChoice != 3); // Returns to main menu
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
                    cout << "Borrowing a book...\n";
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
 
    }
    catch (sql::SQLException& e) // Error message if the system could not connect to the database
    {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }
    MainMenu();
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