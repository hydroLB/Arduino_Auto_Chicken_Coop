#include <RTClib.h> // Include the RTClib library for real-time clock functionality
#include <Servo.h> // Include the Servo library for controlling servo motors
#include <SoftwareSerial.h>
#include <HC05.h>

// Include table_generator.h if it contains the function declarations for createTable and insertIntoTable
#include "table_generator.h"

// Define the pin used to control the servo motor
const int SERVO_PIN = 9; // Define the pin connected to the servo motor (pin 9)

// Define the time range during which the door should be open
const int OPEN_HOUR = 9; // Define the hour when the door should open (9:00)
const int CLOSE_HOUR = 17; // Define the hour when the door should close (17:00)

// Create instances of the Servo and RTClib libraries
Servo doorServo; // Create an instance of the Servo class for controlling the door servo motor
RTC_DS1307 rtc; // Create an instance of the RTC_DS1307 class for real-time clock functionality

void setup() {
    initializeServoAndRTC(); // Initialize the servo motor and RTC modules
    closeDoor(); // Call the closeDoor function to set the servo position to the closed state

    // Connect to the database and create the tables
    createTable("exceptions");
    createTable("data_logging");
}
void loop() {
    if (isRTCWorking()) { // Check if RTC is working properly
        if (isDoorOpenTime()) { // Check if it's time to open the door
            openDoorForScheduledTime(); // Open the door and wait until the scheduled close time
        } else {
            closeDoorAndWaitUntilNextOpenTime(); // Close the door and wait until the next scheduled open time
        }
    } else {
        // Handle RTC error
    }
}

void initializeServoAndRTC() {
    doorServo.attach(SERVO_PIN); // Attach the servo motor instance to the specified pin (pin 9)

    if (!rtc.begin()) {
        // Handle RTC initialization error
        while (1); // Halt the program, as the RTC is not functioning
    }
}

bool isRTCWorking() {
    return !rtc.lostPower(); // Return true if RTC has not lost power, otherwise return false
}

bool isDoorOpenTime() {
    DateTime now = rtc.now(); // Get the current date and time from the RTC
    int hour = now.hour(); // Get the current hour

    // Return true if the current hour is within the open time range, otherwise return false
    return hour >= OPEN_HOUR && hour < CLOSE_HOUR;
}

void openDoorForScheduledTime() {
    openDoor(); // Call the openDoor function to set the servo position to the open state
    unsigned long timeUntilClose = calculateTimeUntilClose(); // Calculate the time until the door close time
    delay(timeUntilClose); // Wait until the door close time
}

void closeDoorAndWaitUntilNextOpenTime() {
    closeDoor(); // Call the closeDoor function to set the servo position to the closed state
    unsigned long timeUntilOpen = calculateTimeUntilOpen(); // Calculate the time until the next door open time
    delay(timeUntilOpen); // Wait until the next door open time
}

unsigned long calculateTimeUntilClose() {
    DateTime now = rtc.now(); // Get the current date and time from the RTC
    int hour = now.hour(); // Get the current hour
    int minute = now.minute(); // Get the current minute
    // Calculate the time until the door close time in milliseconds
    return (CLOSE_HOUR - hour) * 60L * 60L * 1000L - minute * 60L * 1000L;
}

unsigned long calculateTimeUntilOpen() {
    DateTime now = rtc.now(); // Get the current date and time from the RTC
    int hour = now.hour(); // Get the current hour
    int minute = now.minute(); // Get the current minute

    // Calculate the time until the next door open time in milliseconds
    if (hour >= CLOSE_HOUR) {
        return (OPEN_HOUR + 24 - hour) * 60L * 60L * 1000L - minute * 60L * 1000L;
    } else {
        return (OPEN_HOUR - hour) * 60L * 60L * 1000L - minute * 60L * 1000L;
    }
}
void openDoor() {
    // Set the servo position to the open state (e.g., 90 degrees)
    doorServo.write(90);

    // Log the door opening event
    String data = "Door opened";
    insertIntoTable("data_logging", data);
}

void closeDoor() {
    // Set the servo position to the closed state (e.g., 0 degrees)
    doorServo.write(0);

    // Log the door closing event
    String data = "Door closed";
    insertIntoTable("data_logging", data);
}

