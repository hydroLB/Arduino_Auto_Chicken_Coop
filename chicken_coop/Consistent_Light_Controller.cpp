// Comment for Arduino Uno WiFi light control code

// This code requires the following libraries to run:
// - Wire.h (included with Arduino IDE)
// - RTClib.h (https://github.com/adafruit/RTClib)
// - Adafruit_Sensor.h (https://github.com/adafruit/Adafruit_Sensor)
// - Adafruit_TSL2591.h (https://github.com/adafruit/Adafruit_TSL2591)

// The code controls a light source based on the amount of sunlight detected by a TSL2591 light sensor and maintains a consistent 16-hour light cycle for plants. The following components are required and have the following estimated prices (from highest to lowest):
// List of components beside arduino:
// - RTC DS1307: $5
// - TSL2591 light sensor: $5
// - Relay or LED: $1
// - Jumper wires: $0.1

// Total estimated cost of components: $11.1

// Internet or cellular connectivity is not required to run this code.


// Include necessary libraries for RTC and light sensor
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>

// Constants
const int sunlightThreshold = 50;        // Define the sunlight threshold value
const int lightPin = 13;                 // Assign the digital pin 13 to the light (relay or LED)
const int interval = 300000;             // 5 minutes in milliseconds for sampling interval
const long total_light_duration = 57600000;      // 16 hours in milliseconds for total desired light duration

// Variables
unsigned long startTime;
unsigned long lightOnTime;
boolean lightOn = false;

// Objects
RTC_DS1307 rtc;                          // Create RTC object for real-time clock
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // Create light sensor object

// Function prototypes
float readLightSensorAverage();          // Function to read light sensor average value
void turnLightOn(unsigned long duration);// Function to turn the light on for a specific duration
void waitForNextMorning();               // Function to wait until 5:00am
void setupRTC();                         // Function to initialize the RTC
void setupTSL2591();                     // Function to initialize the TSL2591 light sensor

// Setup function runs once at the beginning
void setup() {
    Serial.begin(9600);                    // Initialize serial communication at 9600 baud rate
    pinMode(lightPin, OUTPUT);             // Set the light pin as output to control the light

    setupRTC();                            // Call setupRTC function to initialize the RTC
    setupTSL2591();                        // Call setupTSL2591 function to initialize the TSL2591 light sensor
}

// Main loop function runs repeatedly
void loop() {
    waitForNextMorning();                  // Call waitForNextMorning function to wait until 5:00am

    // Wait for the light sensor average value to exceed the sunlight threshold
    while (readLightSensorAverage() <= sunlightThreshold) {
        delay(interval);                     // Wait for the interval duration (5 minutes) between checks
    }
    //ensures the sun is out from 5:00am to start the counter
    DateTime sunriseTime = rtc.now();      // Record the sunrise time using the RTC

    // Wait for the light sensor average value to fall below the sunlight threshold
    while (readLightSensorAverage() >= sunlightThreshold) {
        delay(interval);                     // Wait for the interval duration (5 minutes) between checks
    }
    DateTime sunsetTime = rtc.now();       // Record the sunset time using the RTC

    // Calculate the duration of daylight in milliseconds
    unsigned long daylightDuration = sunsetTime.unixtime() - sunriseTime.unixtime();
    // Calculate the required light duration in milliseconds to complete 16 hours
    unsigned long lightDuration = total_light_duration - daylightDuration;

    // If the light duration is greater than zero, turn the
    // light on for the remaining duration to finish the 16 hours
    if (lightDuration > 0) {
        turnLightOn(lightDuration);
    }
}

// Function to calculate the average light sensor reading over 5 minutes
float readLightSensorAverage() {
    float sum = 0;
    int samples = 30;

    // Take multiple light sensor readings and add them to the sum variable
    for (int i = 0; i < samples; i++) {
        sensors_event_t event;
        tsl.getEvent(&event);
        sum += event.light; // Use TSL2591 light sensor reading instead of analogRead
        delay(100);
    }

    return sum / samples;
}

// Function to turn the light on for a specified duration
void turnLightOn(unsigned long duration) {
    digitalWrite(lightPin, HIGH);          // Set the light pin to HIGH, turning the light on
    delay(duration);                       // Wait for the specified duration
    digitalWrite(lightPin, LOW);           // Set the light pin to LOW, turning the light off
}

// Function to wait until it's 5:00am
void waitForNextMorning() {
    while (true) {
        DateTime now = rtc.now();            // Get the current time from the RTC
        if (now.hour() == 5) {
            break;                             // If it's 5:00am, exit the loop
        }
        delay(60000);                        // Check the time every minute (60000 milliseconds)
    }
}

// Function to initialize the RTC
void setupRTC() {
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC"); // If the RTC is not found, print an error message
        while (1);                           // Stop the program with an infinite loop
    }

    if (!rtc.isrunning()) {
        Serial.println("RTC is NOT running!"); // If the RTC is not running, print an error message
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set the RTC to the current date and time
    }
}

// Function to initialize the TSL2591 light sensor
void setupTSL2591() {
    if (tsl.begin()) {
        Serial.println("Found a TSL2591 sensor");
    } else {
        Serial.println("No TSL2591 sensor found ... check your connections");
        while (1);
    }

    tsl.setGain(TSL2591_GAIN_LOW);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
}
