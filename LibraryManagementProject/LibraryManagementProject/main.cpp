#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

using namespace std;

string GetDatabasePassword() {
    string Password;
    cout << "Enter MySQL password: ";
    cin >> Password;  // For actual security, replace with a method that hides input
    return Password;
}


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
        // Get MySQL driver instance
        driver = get_driver_instance();

        // Establish connection
        con = driver->connect(server, username, password);

        // Set the schema (replace with your actual schema name)
        con->setSchema("librarysystem");

        
    
        // Clean up resources
        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException& e)
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