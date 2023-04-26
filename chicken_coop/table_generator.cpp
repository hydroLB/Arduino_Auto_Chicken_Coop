//
// Created by Leon Bouramia on 3/23/23.
//

// table_generator.cpp
#include <SoftwareSerial.h>
#include <HC05.h>
//RPI CODE ^^^^
#include "table_generator.h"
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>


//RPI SETUP
SoftwareSerial BTSerial(10, 11); // RX, TX
HC05 hc05(&BTSerial);
const char *raspberryPiAddress = "XX:XX:XX:XX:XX:XX"; // Replace with your Raspberry Pi's Bluetooth address

//RPI CODE
BTSerial.begin(9600); // Set the same baud rate as the Bluetooth module

hc05.begin(38400); // HC-05 default baud rate for AT commands is 38400
hc05.setRole(HC05::Role::Master); // Set the Bluetooth module as Master
hc05.setConnectionMode(HC05::ConnectionMode::Bind, raspberryPiAddress); // Bind to the Raspberry Pi's Bluetooth address
hc05.reset();
//RPI CODE ^^^

// Set your database credentials here
char user[] = "your_username";
char password[] = "your_password";
char server[] = "your_server";
IPAddress server_ip(192, 168, 1, 100); // Set the server IP address
uint16_t server_port = 3306; // Set the server port

WiFiClient client;
MySQL_Connection conn((Client *)&client);
MySQL_Cursor cursor(&conn);

void createTable(const String &tableName) {
    // Hardcoded column definitions for the table
    String columnDefinitions = "id INT AUTO_INCREMENT PRIMARY KEY,"
                               "date_taken VARCHAR(20) NOT NULL,"
                               "content VARCHAR(255) NOT NULL";

    // Construct the SQL query to create the table
    String query = "CREATE TABLE " + tableName + " ("
                   + columnDefinitions +
                   ") ENGINE=InnoDB DEFAULT CHARSET=utf8";

    // Execute the SQL query
    if (conn.connected()) {
        Serial.println("Executing SQL query: " + query);
        cursor.execute(query);
        Serial.println(tableName + " table created.");
    } else {
        Serial.println("Database not connected.");
    }
}

String getCurrentTimestamp() {
    DateTime now = rtc.now();
    char timestamp[20];
    sprintf(timestamp, "%04d-%02d-%02d %02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute());
    return String(timestamp);
}

void insertIntoTable(const String &tableName, const String &data) {
    // Construct the SQL query to insert the data into the specified table
    String query = "INSERT INTO " + tableName + " (date_taken, content) VALUES ('" + getCurrentTimestamp + "', '" + data + "')";

    // Execute the SQL query
    if (conn.connected()) {
        // Print the SQL query to the serial monitor for debugging purposes
        Serial.println("Executing SQL query: " + query);

        // Execute the query using the MySQL Connector/C++ library
        cursor.execute(query);

        // Print a message indicating that the data was successfully inserted into the table
        Serial.println("Data inserted into " + tableName + " table.");
        // Send data to the Raspberry Pi
        BTSerial.println(query);
    } else {
        // Print an error message indicating that the database is not connected
        Serial.println("Database not connected.");
    }
}
