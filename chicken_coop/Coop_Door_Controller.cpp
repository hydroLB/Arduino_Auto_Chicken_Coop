#include <RTClib.h>
#include <Servo.h>

// Constants for door opening and closing hours
const int OPEN_HOUR = 7;
const int CLOSE_HOUR = 19;

// Servo pin
const int SERVO_PIN = 9;

RTC_DS1307 rtc; // RTC instance
Servo doorServo; // Servo instance

void setup() {
  Serial.begin(9600);
  doorServo.attach(SERVO_PIN);
  createTable("exceptions");
  createTable("door_logging");
    

  if (!rtc.begin()) { // Initialize RTC
    Serial.println("Couldn't find RTC");
    while (1); // Halt execution if RTC is not found
  }

  if (!rtc.isrunning()) { // Check if RTC is running
    Serial.println("RTC is not running!");
    // Handle RTC not running exception here
  }

  controlDoor("close"); // Close the door by default
}

void loop() {
  if (isDoorOpenTime()) { // Check if it's time to open the door
    controlDoor("open"); // Open the door
    waitForCloseTime(); // Wait until the close time
  } else {
    controlDoor("close");; // Close the door
    waitForOpenTime(); // Wait until the next open time
  }
}

void controlDoor(String action) {
  DateTime now = rtc.now(); // Get the current date and time from the RTC
  int hour = now.hour(); // Get the current hour
  int minute = now.minute(); // Get the current minute

  if (action == "open") {
    doorServo.write(90); // Set the servo position to the open state (e.g., 90 degrees)
  } else if (action == "close") {
    doorServo.write(0); // Set the servo position to the closed state (e.g., 0 degrees)
  } else {
    String data = "Invalid action. Use 'open' or 'close'.";
    Serial.println(data);
    insertintoTable("exceptions", data);
    return;
  }

  String data = "Door " + action + "ed at " + String(hour) + ":" + String(minute); // Include the current time in the log message
  Serial.println(data); // Log the door event
  insertintoTable("door_logging", data);
}


bool isDoorOpenTime() {
  DateTime now = rtc.now(); // Get the current date and time from the RTC
  int hour = now.hour(); // Get the current hour

  // Return true if the current hour is within the open time range, otherwise return false
  return hour >= OPEN_HOUR && hour < CLOSE_HOUR;
}

void waitForOpenTime() {
  DateTime now = rtc.now(); // Get the current date and time from the RTC
  int hour = now.hour(); // Get the current hour
  int minute = now.minute(); // Get the current minute
  int secondsToOpen;

  // Check if the current hour is greater than or equal to the CLOSE_HOUR
  // If true, then the door will open on the next day
if (hour >= CLOSE_HOUR) {
  // Calculate the remaining seconds until the door opens the next day
  // by finding the difference between 24 hours (end of the day) and the current hour, adding the OPEN_HOUR,
  // then converting the result to seconds and subtracting the elapsed seconds in the current minute
  secondsToOpen = (24 - hour + OPEN_HOUR) * 60 * 60 - minute * 60;
} else {
  // Calculate the remaining seconds until the door opens on the same day
  // by finding the difference between OPEN_HOUR and the current hour,
  // then converting the result to seconds and subtracting the elapsed seconds in the current minute
  secondsToOpen = (OPEN_HOUR - hour) * 60 * 60 - minute * 60;
}


  // Wait until the next door open time
  delay(secondsToOpen * 1000UL);
}

void waitForCloseTime() {
  DateTime now = rtc.now(); // Get the current date and time from the RTC
  int hour = now.hour(); // Get the current hour
  int minute = now.minute(); // Get the current minute
  // Calculate the remaining seconds until the door closes
  // by finding the difference between CLOSE_HOUR and the current hour,
  // then converting the result to seconds and subtracting the elapsed seconds in the current minute
  int secondsToClose = (CLOSE_HOUR - hour) * 60 * 60 - minute * 60;


  // Wait until the door close time
  delay(secondsToClose * 1000UL);
}
