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
    cin >> Password;  // For actual security, replace with a method that hides input
    return Password;
}

// Database constants
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = GetDatabasePassword();

int main()
{
    // Variable declarations using SQL connector
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;

    try // Try catch statement  if the system cannot connect to the server
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

    system("pause");
    return 0;
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