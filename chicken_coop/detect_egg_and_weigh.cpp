// This code requires the following libraries:
// 1. Wire.h - I2C bus library: https://www.arduino.cc/en/reference/wire
// 2. Adafruit_MotorShield.h - Adafruit Motor Shield library: https://learn.adafruit.com/adafruit-motor-shield/library-reference
// 3. Adafruit_MS_PWMServoDriver.h - PWM driver utility library for the Adafruit Motor Shield: https://learn.adafruit.com/adafruit-motor-shield/library-reference
// 4. SoftwareSerial.h - software serial library: https://www.arduino.cc/en/Reference/SoftwareSerial
// 5. TinyGsmClient.h - TinyGSM library: https://github.com/vshymanskyy/TinyGSM
// 6. ArduinoHttpClient.h - HTTP client library: https://github.com/arduino-libraries/ArduinoHttpClient
// 7. Twilio.h - Twilio API library: https://www.twilio.com/docs/libraries/overview
// 8. HX711.h - HX711 load cell amplifier library: https://github.com/bogde/HX711
// 9. MySQL_Connection.h - MySQL database connection library: https://github.com/ChuckBell/MySQL_Connector_Arduino
// 10. MySQL_Cursor.h - MySQL cursor library: https://github.com/ChuckBell/MySQL_Connector_Arduino

// Required Components beside arduino:
// 2. Adafruit Motor Shield v2 - 1 (Price: $22.50)
// 3. Break Beam Sensor - 1 (Price: $4.99)
// 4. HX711 Load Cell Amplifier - 1 (Price: $6.99)
// 5. Load Cell - 1 (Price: $9.99)
// 6. Jumper Wires - Assorted (Price: $9.99)
// 7. Breadboard - 1 (Price: $6.99)

// Total Cost: $61.44

// Internet or cellular connectivity is required to use the Twilio API and
// connect to the MySQL database.
// An active Wi-Fi network is required for the Arduino Uno WiFi Rev2 to connect to the internet.

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <utility/Adafruit_MS_PWMServoDriver.h>
#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <Twilio.h>
#include <HX711.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <RTClib.h>

//special for database
#include "table_generator.h"



// Constants
#define LOADCELL_DOUT_PIN 6
#define LOADCELL_SCK_PIN 7
#define BREAK_BEAM_PIN 5
#define CONVEYOR_MOTOR_CHANNEL_1 1
#define CONVEYOR_MOTOR_CHANNEL_2 2
#define CONVEYOR_ON_TIME 5000
#define CONVEYOR_START_DELAY 2000
#define SLOW_CONVEYOR_SPEED 77

// Twilio configuration
const char* apn = "your_apn_name"; // your cellular carrier's Access Point Name (APN)
const char* gprsUser = "";
const char* gprsPass = "";
const char* server = "api.twilio.com";
const int port = 80;
const char* account_sid = "your_twilio_account_sid"; // your Twilio account SID
const char* auth_token = "your_twilio_auth_token"; // your Twilio auth token
const char* twilio_number = "your_twilio_phone_number"; // your Twilio phone number
const char* recipient_number = "recipient_phone_number"; // the recipient's phone number


//RTC config
RTC_DS1307 rtc;
rtc.begin();


// Objects
TinyGsm modem(Serial1);
HttpClient client = HttpClient(modem, server, port);
Twilio twilio(client, account_sid, auth_token);


// Variables
int egg = 0;
int egg_form_num = 0;
HX711 scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
float eggWeight = 0;

bool eggDetected() {
    int reading = digitalRead(BREAK_BEAM_PIN);

    if (reading == LOW || reading == HIGH) {
        return !reading;
    } else {
        Serial.println("Error: Invalid reading from break beam sensor.");
        return false;
    }

    }

void setup() {

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(); // Set the calibration factor (e.g., scale.set_scale(2280.0))
    scale.tare(); // Reset the scale to 0
    pinMode(BREAK_BEAM_PIN, INPUT_PULLUP);

    Serial.begin(9600);
    Serial.println("Initialization complete.");
    // Create a table
    String tableName = "egg_table";
    createTable(tableName); // Create the eggs table
}


void loop() {
    // Check if an egg is detected by the break beam sensor
    if (eggDetected()) {
        Serial.println("Egg detected! Waiting before moving conveyor.");
        // Egg counter
        egg++;
        egg_form_num++;

        // Read the weight of the egg after 1 second
        delay(1000);
        eggWeight = scale.get_units(); // Get the weight in units (e.g., grams)
        //egg weight is unusual
        if (eggWeight <= 25 and eggWeight >= 150) {
            twilio.sendSMS(twilio_number, recipient_number, "Scale value for egg unusual");
            Serial.println("Error: Invalid reading from scale, too high or low value");
        }

        DateTime eggTimestamp = rtc.now(); // Record the timestamp of the egg detection
        Serial.print("Egg weight: ");
        Serial.print(eggWeight);
        Serial.println(" grams");

        // Concatenate egg_form and eggWeight into one string
        String eggData = "#" + String(egg_form_num) + ", " + String(eggWeight) + "grams";

        // Insert egg data into the database
        insertIntoTable("Egg_Table", eggData);

    }

    // Amount of eggs before notification
    if (egg == 24) {
        // Send a text message using the Twilio API
        twilio.sendSMS(twilio_number, recipient_number, "Come get the eggs!");
        Serial.println("Text message sent.");
        egg = 0;
    }

    // Sleep for a short duration to ensure the sensor sees the egg but CPU gets a break
    delay(300);
}
