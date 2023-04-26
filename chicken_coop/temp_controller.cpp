/*
 * This sketch reads the temperature from three sensors and logs the temperature readings to a MySQL database.
 * It requires the following libraries:
 * - OneWire: https://www.pjrc.com/teensy/td_libs_OneWire.html
 * - DallasTemperature: https://github.com/milesburton/Arduino-Temperature-Control-Library
 * - MySQL Connector/Arduino: https://github.com/ChuckBell/MySQL_Connector_Arduino
 *
 * To run this sketch, you will need the following components:
 * - DS18B20 temperature sensors (3): $2.99 each
 * - 4.7k ohm resistor (3): $0.10 each
 * - Breadboard (1): $4.99
 * - Jumper wires (approx. 10): $3.99
 *
 * Total cost: $22.00
 *
 * You can buy these components from various online retailers such as:
 * - Adafruit (https://www.adafruit.com/)
 * - SparkFun (https://www.sparkfun.com/)
 * - Amazon (https://www.amazon.com/)
 *
 * Note that this sketch requires an internet connection, either through WiFi or cellular.
 *
 * If you are using WiFi, you will need to connect the Arduino to a WiFi network by updating the ssid and password variables in the sketch.
 * If you are using cellular, you will need to connect the Arduino to a cellular network by adding a compatible cellular shield or module and updating the appropriate variables in the sketch.
 *
 * Before running this sketch, you will also need to set up a MySQL database to store the temperature readings. You can do this using a hosting provider or by setting up a local server on your own computer.
 *
 * Once you have the required components and have set up the WiFi/cellular connection and MySQL database, you can upload this sketch to the Arduino and start logging temperature readings.
 */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <RTClib.h> // Include the RTClib library
#include "table_generator.h"

// Define the digital input pins for the temperature sensors
const int TEMPERATURE_SENSOR_1_PIN = 2;
const int TEMPERATURE_SENSOR_2_PIN = 3;
const int TEMPERATURE_SENSOR_3_PIN = 4;

// Define the temperature thresholds in Fahrenheit
const float MIN_TEMP_F = 50.0;
const float MAX_TEMP_F = 85.0;

// Define the number of temperature readings to take over time to average
const int NUM_READINGS = 10;

// Set up the OneWire interface and DallasTemperature library
OneWire oneWire1(TEMPERATURE_SENSOR_1_PIN);
OneWire oneWire2(TEMPERATURE_SENSOR_2_PIN);
OneWire oneWire3(TEMPERATURE_SENSOR_3_PIN);
DallasTemperature temperatureSensor1(&oneWire1);
DallasTemperature temperatureSensor2(&oneWire2);
DallasTemperature temperatureSensor3(&oneWire3);
RTC_DS1307 rtc; // Create an instance of the RTC_DS1307 class

void setup() {
    Serial.begin(9600);

    // Start the temperature sensors
    temperatureSensor1.begin();
    temperatureSensor2.begin();
    temperatureSensor3.begin();

    // Initialize the RTC module
    if (!rtc.begin()) {
        Serial.println("Error initializing RTC module");
    }

    // Create the temperature_log table
    createTable('error_log');
    createTable('temperature_log');

}

// Function to read the temperature in Fahrenheit
float getTemperatureF(int numReadings, DallasTemperature &sensor, int sensorNum) {
    float sum = 0.0;
    bool error = false;

    // Request and average multiple temperature readings
    for (int i = 0; i < numReadings; i++) {
        // Request a temperature reading from the sensor
        sensor.requestTemperatures();

        // Read the temperature in Celsius
        float temperatureC = sensor.getTempCByIndex(0);

        // Convert the temperature to Fahrenheit
        float temperatureF = temperatureC * 1.8 + 32;

        // Check if the temperature reading is valid
        if (isnan(temperatureF)) {
            Serial.println("Error reading temperature from sensor " + String(sensorNum) + "!");
            // TODO: Add error handling code here, such as resetting the Arduino or alerting the user.
            error = true;
        } else {
            // Add the temperature to the sum
            sum += temperatureF;
        }

        // Delay for a short time before taking another temperature reading
        delay(10);
    }

    // Print an error message to the database if there was an error reading the temperature
    if (error) {
        String data = "Error reading temperature from sensor " + String(sensorNum) + ".";
        insertIntoTable("error_log", data);
        return -1.0;
    }

    // Calculate and return the average
    return sum / numReadings;
}

// Function to check if the temperature is within range
void checkTemperature(float temperatureF) {
// Output the temperature to the serial monitor
    Serial.print("Temperature: ");
    Serial.print(temperatureF);
    Serial.println(" F");
    // Check if the temperature is within range
    if (temperatureF >= MIN_TEMP_F && temperatureF <= MAX_TEMP_F) {
        Serial.println("Temperature is within range.");
        // Do nothing - temperature is within the range for chicken health.
    }
// Check if the temperature is too low
    else if (temperatureF < MIN_TEMP_F) {
        Serial.println("Temperature is too low!");
        // TODO: Turn on the heater to warm up the chicken coop.
        delay(300000); // Delay for 5 minutes before turning on the heater
    }
// Check if the temperature is too high
    else if (temperatureF > MAX_TEMP_F) {
        Serial.println("Temperature is too high!");
        // TODO: Turn on the AC to cool down the chicken coop.
        delay(300000); // Delay for 5 minutes before turning on the AC
    }
}

//initialize value to keep track of how often to record time
int val = 0;

void loop() {
// Read the temperature in Fahrenheit by averaging multiple readings from each sensor
    float temperatureF1 = getTemperatureF(NUM_READINGS, temperatureSensor1);
    float temperatureF2 = getTemperatureF(NUM_READINGS, temperatureSensor2);
    float temperatureF3 = getTemperatureF(NUM_READINGS, temperatureSensor3);
    // Calculate the average temperature from the three sensors
    float averageTemperatureF = (temperatureF1 + temperatureF2 + temperatureF3) / 3;

// Check if the temperature is within range
    checkTemperature(averageTemperatureF);
    DateTime now = rtc.now();

// Get the current date and time as a string

    //logs only occassanily (delay*val equality check, (180 times of 20 second delay= 3600 seconds= 1 hour ))
    if (val == 178) {

        String tableName = 'temperature_log';
        // Get the current date and time as a string

        String data = "Temperature: ";

        data.concat(averageTemperatureF);

        insertIntoTable(tableName,data);

        //reset count for ~1 hour
        val=0;
    }
    else {
        val++;
    }
    // Delay for 20 second before taking another temperature reading
    delay(100);

}
