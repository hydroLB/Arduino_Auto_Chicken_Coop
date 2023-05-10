#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2591.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <HC05.h>
#include <DHT.h>
#include "table_generator.cpp"
#include <HX711_ADC.h>
#include <Adafruit_MotorShield.h>
#include <utility/Adafruit_MS_PWMServoDriver.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <Twilio.h>
#include <HX711.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RTClib.h> // Include the RTClib library
#include "table_generator.h"


////IDE SPECIFIC (CAN BE REMOVED)////
#pragma clang diagnostic push
#pragma ide diagnostic ignored "LoopDoesntUseConditionVariableInspection"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#pragma ide diagnostic ignored "UnusedLocalVariable"
/////////////////////////////////////

#VARIABLES (to be modified to values suited to the user):
// LIGHT CONTROLLER CONSTANTS
const int sunlightThreshold = 50;   // Define the sunlight threshold value
const int lightPin = 13;            // Assign the digital pin 13 to the light (relay or LED)
const int interval = 300000;        // 5 minutes in milliseconds for sampling interval
const long sixteenHours = 57600000; // 16 hours in milliseconds for total desired light duration
// LIGHT CONTROLLER VARIABLES
unsigned long startTime;            // initialization *dont touch*
unsigned long lightOnTime;          // initialization *dont touch*
boolean lightOn = false;            // default is false

wait_until_next_morning_loop_delay = 60000;  // Check the time every minute (60000 milliseconds)
int samples = 30; // how many light samples to average before checking it against threshold

// Objects
RTC_DS1307 rtc; // Create RTC object for real-time clock for all of them //initialization *dont touch*
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // Create light sensor object 

//DOOR CONTROLLER CONSTANTS

// Define the pin used to control the servo motor
const int SERVO_PIN = 9;    // Define the pin connected to the servo motor (pin 9)

// Define the time range during which the door should be open
const int OPEN_HOUR = 9;   // Define the hour when the door should open (9:00)
const int CLOSE_HOUR = 17; // Define the hour when the door should close (17:00)
door_open_degree = 90;     // (e.g., 90 degrees)
door_close_degree = 0;     // (e.g., 0 degrees)

// Create instances of the Servo
Servo doorServo; // Create an instance of the Servo class for controlling the door servo motor


// HUMIDITY CONTROL CONSTANTS
#define DHT_PIN 2                // DHT22 sensor pin
#define DHT_TYPE DHT22           // DHT22 sensor type
#define RELAY_PIN 3              // Relay pin for the fan
#define HUMIDITY_THRESHOLD 65.0  // Humidity threshold for turning the fan on
#define FAN_ON_TIME 1200000      // 20 minutes on time then
#define FAN_OFF_TIME 600000      // 10 minutes off time then FAN_ON_TIME for interval time 
#define INTERVAL_6H 21600000     // 6 hours
#define INTERVAL_24H 86400000    // 24 hours
DHT dht(DHT_PIN, DHT_TYPE);      // DHT sensor
DHT dht2(DHT_PIN2, DHT_TYPE);    // Second DHT sensor
unsigned long lastFanOnTime = 0; // Timestamp of the last time the fan was turned on
bool fanOn = false;              // Flag to indicate if the fan should be turned on

humidity_loop_delay = 60000;     // Check humidity every minute when the fan is not active

//FOOD/WATER DISPENSING CONSTANTS

int food_servo_start_position = 0;     // Move the servo to the starting position
int food_servo_move_delay = 1000;      // Wait for the servo to move (milliseconds)
int dispense_food_position = 90;       // Move the servo to dispense the food
int food_servo_dispense_delay = 3000;  // how long to let the food dispense
int water_pump_dispense_delay = 3000;  // how long to let the water dispense

Servo foodServo;                    // create a servo object
HX711_ADC foodWeightSensor;         // create a weight sensor object for food
HX711_ADC waterWeightSensor;        // create a weight sensor object for water
const int foodServoPin = 3;         // define the servo pin
const int foodWeightSensorDT = 2;   // define the data pin for food weight sensor
const int foodWeightSensorSCK = 4;  // define the clock pin for food weight sensor
const int waterWeightSensorDT = 5;  // define the data pin for water weight sensor
const int waterWeightSensorSCK = 6; // define the clock pin for water weight sensor
const int waterPumpPin = 7;         // define the pump pin
const float foodAmount = 100.0;     // define the amount of food to dispense in grams
const float waterAmount = 50.0;     // define the amount of water to dispense in ml
const float foodRemainingThreshold = 0.1;  // define the threshold for remaining food
const float waterRemainingThreshold = 0.1; // define the threshold for remaining water

String food_table = "food_dispensing_log";
String water_table = "water_dispensing_log";

enum WeightSensorType {
    FOOD_SENSOR,
    WATER_SENSOR
};

//TEMPERATURE CONTROL
// Define the digital input pins for the temperature sensors
const int TEMPERATURE_SENSOR_1_PIN = 2;
const int TEMPERATURE_SENSOR_2_PIN = 3;
const int TEMPERATURE_SENSOR_3_PIN = 4;
// Set up the OneWire interface and DallasTemperature library
OneWire oneWire1(TEMPERATURE_SENSOR_1_PIN);
OneWire oneWire2(TEMPERATURE_SENSOR_2_PIN);
OneWire oneWire3(TEMPERATURE_SENSOR_3_PIN);
DallasTemperature temperatureSensor1(&oneWire1);
DallasTemperature temperatureSensor2(&oneWire2);
DallasTemperature temperatureSensor3(&oneWire3);

// Define the temperature thresholds in Fahrenheit
const float MIN_TEMP_F = 50.0;
const float MAX_TEMP_F = 85.0;
int temperature_reading_delay = 20000; // Delay for 20 second before taking another temperature reading

int heater_on_time = 300000; // Delay for 5 minutes before turning on the heater
int cooler_on_time = 300000; // Delay for 5 minutes before turning on the AC

// Define the number of temperature readings to take over time to average
const int NUM_READINGS = 10;
//initialize value to keep track of how often to record time
int val = 0;

#EGG DETECT
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

int egg = 0;
int egg_form_num = 0;
int certain_egg_num = 24; // how many eggs to count before a text is sent and counter is reset
int beam_interval_sleep = 300; // how many milliseconds the cpu will wait before checking the break beam again
HX711 scale(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
float eggWeight = 0; // initialization *dont touch*
int weigh_egg_after_detect_delay = 1000; // Delay before reading the weight of the egg
    
// Objects
TinyGsm modem(Serial1);
HttpClient client = HttpClient(modem, server, port);
Twilio twilio(client, account_sid, auth_token);



////////////////////////////////////// CONSISTENT LIGHT CONTROLLER START //////////////////////////
// Function to calculate the average light sensor reading over 5 minutes
float readLightSensorAverage() {
    float sum = 0;

    // Take multiple light sensor readings and add them to the sum variable
    for (int i = 0; i < samples; i++) {
        sensors_event_t event;
        tsl.getEvent(&event);
        sum += event.light; // Use TSL2591 light sensor reading instead of analogRead
        delay(200);
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
        DateTime local_now = rtc.now();            // Get the current time from the RTC
        if (local_now.hour() == 5) {
            break;                             // If it's 5:00am, exit the loop
        }
        delay(wait_until_next_morning_loop_delay); 
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

void Consistent_Light_Controller_loop() {
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
    unsigned long lightDuration = sixteenHours - daylightDuration;

    // If the light duration is greater than zero, turn the
    // light on for the remaining duration to finish the 16 hours
    if (lightDuration > 0) {
        turnLightOn(lightDuration);
    }
}
////////////////////////////////////// CONSISTENT LIGHT CONTROLLER END //////////////////////////

////////////////////////////////////// COOP DOOR CONTROLLER START //////////////////////////
void initializeServo() {
    doorServo.attach(SERVO_PIN); // Attach the servo motor instance to the specified pin (pin 9)
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
    return (CLOSE_HOUR - hour) * 60 * 60 * 1000 - minute * 60 * 1000;
}

unsigned long calculateTimeUntilOpen() {
    DateTime now = rtc.now(); // Get the current date and time from the RTC
    int hour = now.hour(); // Get the current hour
    int minute = now.minute(); // Get the current minute

    // Calculate the time until the next door open time in milliseconds
    if (hour >= CLOSE_HOUR) {
        return (OPEN_HOUR + 24 - hour) * 60 * 60 * 1000 - minute * 60 * 1000;
    } else {
        return (OPEN_HOUR - hour) * 60 * 60 * 1000 - minute * 60 * 1000;
    
}
void openDoor() {
    // Set the servo position to the open state 
    doorServo.write(door_open_degree);

    // Log the door opening event
    String data = "Door opened";
    insertIntoTable("data_logging", data);
}

void closeDoor() {
    // Set the servo position to the closed state 
    doorServo.write(door_close_degree);

    // Log the door closing event
    String data = "Door closed";
    insertIntoTable("data_logging", data);
}

void Coop_Door_Controller_setup() {
    initializeServoAndRTC(); // Initialize the servo motor and RTC modules
    closeDoor(); // Call the closeDoor function to set the servo position to the closed state

    // Connect to the database and create the tables
    createTable("exceptions");
    createTable("data_logging");
}

void Coop_Door_Controller_loop() {
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
    
////////////////////////////////////// COOP DOOR CONTROLLER END //////////////////////////

////////////////////////////////////// COOP HUMIDITY START //////////////////////////
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
}

void Coop_Humidity_Control_setup() {
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
        delay(humidity_loop_delay); 
    }
}

void Coop_Humidity_Control_loop() {
    Wire.begin();
    dht.begin();
    dht2.begin();
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Turn off the fan initially
    createTable("errors");
    createTable("humidity");
    // Uncomment the following lines to set the date and time
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}
////////////////////////////////////// COOP HUMIDITY END //////////////////////////

////////////////////////////////////// DISPENSE FOOD/WATER START //////////////////////////   
float getWeight(WeightSensorType type) {
    float weight = 0.0;

    switch (type) {
        case FOOD_SENSOR:
            if (foodWeightSensor.isReady()) {  // Check if the food weight sensor is ready
                weight = foodWeightSensor.getData();  // Read the food weight
                if (!weight > 0) {
                    Serial.print("Food weight: ");
                }
                else {
                    Serial.println("Error: Food weight sensor not ready");  // Error message
                    break;
                } 
                else {
                Serial.println("Error: Food weight sensor not ready");  // Error message
                break;
            }
            break;
        case WATER_SENSOR:
            if (waterWeightSensor.isReady()) {  // Check if the water weight sensor is ready
                weight = waterWeightSensor.getData();  // Read the water weight
                Serial.print("Water weight: ");
            } else {
                Serial.println("Error: Water weight sensor not ready");  // Error message
                break;
            }
            break;
        default:
            Serial.println("Error: Invalid weight sensor type");
            break;
    }

    Serial.print(weight);  // Print the measured weight
    // Print the unit (ml for water, g for food)
    Serial.println(type == WATER_SENSOR ? " ml" : " g");

    return weight;  // Return the measured weight
}
 
    
void dispenseFood() {
    if (foodWeightSensor.isReady()) {
        foodServo.write(food_servo_start_position);                    
        delay(food_servo_move_delay);                                
        foodServo.write(dispense_food_position);                       
        delay(food_servo_dispense_delay);     
        foodServo.write(food_servo_start_position);                    

        float foodDispensed = foodWeightSensor.getWeight();
        String food_dispense_string= "Dispensed: ";
        food_dispense_string.concat(foodDispensed);// Get the amount of food dispensed

        insertIntoTable(food_table, food_dispense_string); // Call the new function to insert data

    } else {
        // Print an error message if the food weight sensor is not ready
        Serial.println("Error: Food weight sensor not ready");
    }
}
    
void dispenseWater() {
    if (waterWeightSensor.isReady()) {
        digitalWrite(waterPumpPin, HIGH);           // Turn on the water pump
        delay(water_pump_dispense_delay);           // Wait for the water to dispense
        digitalWrite(waterPumpPin, LOW);            // Turn off the water pump

        float waterDispensed = waterWeightSensor.getWeight();  // Get the amount of water dispensed from the water weight sensor

        String water_dispense_string= "Dispensed: ";
        water_dispense_string.concat(waterDispensed);

        insertIntoTable(water_table, water_dispense_string); // Call the new function to insert data

    } else {
        // Print an error message if the water weight sensor is not ready
        Serial.println("Error: Water weight sensor not ready");
    }
}

void Dispense_Food_Water_setup() {
        foodServo.attach(foodServoPin);              // attach the servo to the pin
        if (!foodWeightSensor.begin(foodWeightSensorDT, foodWeightSensorSCK)) {
            Serial.println("Error: Failed to initialize food weight sensor");
        }
        if (!waterWeightSensor.begin(waterWeightSensorDT, waterWeightSensorSCK)) {
            Serial.println("Error: Failed to initialize water weight sensor");
        }
        pinMode(waterPumpPin, OUTPUT);              // set the pump pin as an output
        createTable(food_table);  // Create the food_dispensing_log table
        createTable(water_table);
    }

void Dispense_Food_Water_loop()  {
    float foodWeight = getWeight(FOOD_SENSOR);   // get the current weight of the food
        float waterWeight = getWeight(WATER_SENSOR); // get the current weight of the water

        if (foodWeight == 0.0) {
            Serial.println("Error: Failed to read food weight");
        } else if (foodWeight < foodAmount * foodRemainingThreshold) {  // if there's less than 10% food remaining
            dispenseFood();                           // dispense food
        }

        if (waterWeight == 0.0) {
            Serial.println("Error: Failed to read water weight");
        } else if (waterWeight < waterAmount * waterRemainingThreshold) {  // if there's less than 10% water remaining
            dispenseWater();                          // dispense water
        }
    }

////////////////////////////////////// DISPENSE FOOD END //////////////////////////

////////////////////////////////////// EGG DETECT START //////////////////////////
bool eggDetected() {
    int reading = digitalRead(BREAK_BEAM_PIN);

    if (reading == LOW || reading == HIGH) {
        return !reading;
    } else {
        Serial.println("Error: Invalid reading from break beam sensor.");
        return false;
    }
 }

void Egg_Detect_Weigh_Record_setup() {

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(); // Set the calibration factor (e.g., scale.set_scale(2280.0))
    scale.tare(); // Reset the scale to 0
    pinMode(BREAK_BEAM_PIN, INPUT_PULLUP);

    Serial.println("Initialization complete.");
    // Create a table
    String tableName = "egg_table";
    createTable(tableName); // Create the eggs table
}

void Egg_Detect_Weigh_Record_loop() {
    // Check if an egg is detected by the break beam sensor
    if (eggDetected()) {
        Serial.println("Egg detected!");
        //Local egg counter that will reset when eggs reach certain_egg_num
        egg++;
        //Global egg counter to ensure total number of eggs in form is accurate
        egg_form_num++;
        // read the weight after a delay 
        delay(weigh_egg_after_detect_delay);
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
        
        // Amount of eggs before notification
        if (egg == certain_egg_num) {
            // Send a text message using the Twilio API
            twilio.sendSMS(twilio_number, recipient_number, "Come get the eggs!");
            Serial.println("Text message sent.");
            egg = 0;
            }

    }
    // Sleep for a short duration to ensure the sensor sees the egg but CPU gets a break
    delay(beam_interval_sleep);
}
////////////////////////////////////// EGG DETECT END //////////////////////////
    
////////////////////////////////////// HEAT/COOL TEMP CONTROLLER START ////////////////////////////////////
void temperature_setup() {
    // Start the temperature sensors
    temperatureSensor1.begin();
    temperatureSensor2.begin();
    temperatureSensor3.begin();

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
        delay(50);
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
        delay(heater_on_time); 
    }
// Check if the temperature is too high
    else if (temperatureF > MAX_TEMP_F) {
        Serial.println("Temperature is too high!");
        // TODO: Turn on the AC to cool down the chicken coop.
        delay(cooler_on_time); // Delay for 5 minutes before turning on the AC
        
    }
}

void temperature_loop() {
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

    //logs only occasionally (delay*val equality check, (180 times of 20-second delay= 3600 seconds= 1 hour ))
    if (val == 180) {

        String tableName = "temperature_log";
        // Get the current date and time as a string

        String data = "Temperature: ";

        data += String(averageTemperatureF);

        insertIntoTable(tableName,data);

        //reset count every ~1 hour
        val= 0;
    }
    else {
        val++;
    }
    delay(temperature_reading_delay); 
}

//////////////////////////////////////////  HEAT/COOL TEMP CONTROLLER END ////////////////////////////////////


// Call the setup functions for all 4 components
void setup() {
    Serial.begin(9600);
    Consistent_Light_Controller_setup(); //includes RTC setup
    Coop_Door_Controller_setup();
    Dispense_Food_Water_setup();
    Coop_Humidity_Control_setup();
    Egg_Detect_Weigh_Record_setup();
    temperature_setup();
    
}

// Call the loop functions for all 4 components
void loop() {
    Consistent_Light_Controller_loop();
    Coop_Door_Controller_loop();
    Dispense_Food_Water_loop();
    Coop_Humidity_Control_loop();
    Egg_Detect_Weigh_Record_loop();
    temperature_loop();
}
