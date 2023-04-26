#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>
#include "table_generator.cpp" // Include the header file for table_generator


// Constants
#define DHT_PIN 2                // DHT22 sensor pin
#define DHT_TYPE DHT22           // DHT22 sensor type
#define RELAY_PIN 3              // Relay pin for the fan
#define HUMIDITY_THRESHOLD 65.0  // Humidity threshold for turning the fan on
#define FAN_ON_TIME 1200000      // 20 minutes on time then
#define FAN_OFF_TIME 600000      // 10 minutes off time then ^^^^^ for interval time vvvvvvvv
#define INTERVAL_6H 21600000     // 6 hours
#define INTERVAL_24H 86400000    // 24 hours

// Variables
RTC_DS3231 rtc;                 // RTC module
DHT dht(DHT_PIN, DHT_TYPE);     // DHT sensor
DHT dht2(DHT_PIN2, DHT_TYPE);  // Second DHT sensor
unsigned long lastFanOnTime = 0;// Timestamp of the last time the fan was turned on
bool fanOn = false;             // Flag to indicate if the fan should be turned on

void setup() {
    Serial.begin(9600);
    Wire.begin();
    rtc.begin();
    dht.begin();
    dht2.begin();
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Turn off the fan initially
    createTable("errors");
    createTable("humidity");
    // Uncomment the following lines to set the date and time
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
    float humidity = readHumidity();            // Read the current humidity
    unsigned long currentTime = getCurrentTimeMillis(); // Get the current time
    unsigned long elapsedTime = currentTime - lastFanOnTime; // Calculate the elapsed time since the fan was last turned on

    if (isHumidityHigh(humidity, HUMIDITY_THRESHOLD) && elapsedTime >= INTERVAL_6H) {
        fanOn = true;                            // Turn on the fan if the humidity is above the threshold and the 6-hour interval has passed
        lastFanOnTime = currentTime;
    } else if (elapsedTime >= INTERVAL_24H) {
        fanOn = true;                            // Turn on the fan if 24 hours have passed since the fan was last turned on
        lastFanOnTime = currentTime;
    }

    if (fanOn) {
        operateFan(FAN_ON_TIME, FAN_OFF_TIME, INTERVAL_6H); // Operate the fan for 6 hours with 20 minutes on and 10 minutes off
        fanOn = false;
        lastFanOnTime = getCurrentTimeMillis();
    } else {
        delay(60000); // Check humidity every minute when the fan is not active
    }
}
float readHumidity() {
    float humidity1 = dht.readHumidity();
    float humidity2 = dht2.readHumidity();
    if (isnan(humidity1) || isnan(humidity2)) {
        Serial.println("Failed to read from DHT sensors!");
        return -1;
    }
    float averageHumidity = (humidity1 + humidity2) / 2;
    Serial.print("Average humidity: ");
    Serial.print(averageHumidity);
    Serial.println("%");
    return averageHumidity;
}

unsigned long getCurrentTimeMillis() {
    DateTime now = rtc.now();                  // Get the current time from the RTC module
    return now.unixtime() * 1000;              // Convert the time to milliseconds
}

bool isHumidityHigh(float humidity, float threshold) {
    return humidity > threshold;               // Check if humidity is above the threshold
}

void operateFan(unsigned long onTime, unsigned long offTime, unsigned long duration) {
    unsigned long startTime = getCurrentTimeMillis(); // Store the starting time
    while (getCurrentTimeMillis() - startTime < duration) { // Loop until the total duration is reached
    }
