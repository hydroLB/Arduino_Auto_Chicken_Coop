// Comment for Arduino Uno WiFi project

// Libraries required to run the code:
// - Wire.h (https://www.arduino.cc/en/reference/wire)
// - RTClib.h (https://github.com/adafruit/RTClib)
// - MySQL_Connection.h (https://github.com/ChuckBell/MySQL_Connector_Arduino)
// - MySQL_Cursor.h (https://github.com/ChuckBell/MySQL_Connector_Arduino)
// - Servo.h (https://www.arduino.cc/en/reference/servo)
// - HX711_ADC.h (https://github.com/bogde/HX711)
// - WiFiNINA.h (https://www.arduino.cc/en/Reference/WiFiNINA)

// Required components:
// - Real-time clock module (1): $4.00
// - HX711 load cell amplifier and weight sensor (2): $5.00 each
// - Servo motor (1): $5.00
// - Peristaltic pump (1): $7.00
// - Jumper wires (various): $3.00 for a set of 120

// Total cost of components: $29.00 (approximate)

// Internet connectivity is required for this project,
// either through WiFi or cellular.
// Please ensure that you have a stable internet connection before proceeding.

#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>                // include the servo library
#include <HX711_ADC.h>            // include the weight sensor library

Servo foodServo;                  // create a servo object
HX711_ADC foodWeightSensor;       // create a weight sensor object for food
HX711_ADC waterWeightSensor;      // create a weight sensor object for water
RTC_DS1307 rtc;                  // Real-time clock object

const int foodServoPin = 3;       // define the servo pin
const int foodWeightSensorDT = 2; // define the data pin for food weight sensor
const int foodWeightSensorSCK = 4;// define the clock pin for food weight sensor
const int waterWeightSensorDT = 5;// define the data pin for water weight sensor
const int waterWeightSensorSCK = 6;// define the clock pin for water weight sensor
const int waterPumpPin = 7;       // define the pump pin

const float foodAmount = 100.0;   // define the amount of food to dispense in grams
const float waterAmount = 50.0;   // define the amount of water to dispense in ml
const float foodRemainingThreshold = 0.1;  // define the threshold for remaining food
const float waterRemainingThreshold = 0.1; // define the threshold for remaining water

String food_table = 'food_dispensing_log';
String water_table = 'water_dispensing_log';

enum WeightSensorType {
    FOOD_SENSOR,
    WATER_SENSOR
};

    void setup() {
        foodServo.attach(foodServoPin);              // attach the servo to the pin
        if (!foodWeightSensor.begin(foodWeightSensorDT, foodWeightSensorSCK)) {
            Serial.println("Error: Failed to initialize food weight sensor");
        }
        if (!waterWeightSensor.begin(waterWeightSensorDT, waterWeightSensorSCK)) {
            Serial.println("Error: Failed to initialize water weight sensor");
        }
        pinMode(waterPumpPin, OUTPUT);              // set the pump pin as an output
        Serial.begin(9600);
        createTable(food_table);  // Create the food_dispensing_log table
        createTable(water_table);
    }

    void loop() {
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
float getWeight(WeightSensorType type) {
    float weight = 0.0;

    switch (type) {
        case FOOD_SENSOR:
            if (foodWeightSensor.isReady()) {  // Check if the food weight sensor is ready
                weight = foodWeightSensor.getData();  // Read the food weight
                Serial.print("Food weight: ");
            } else {
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
    }

    Serial.print(weight);  // Print the measured weight
    // Print the unit (ml for water, g for food)
    Serial.println(type == WATER_SENSOR ? " ml" : " g");

    return weight;  // Return the measured weight
}

void dispenseFood() {
    if (foodWeightSensor.isReady()) {
        foodServo.write(0);                         // Move the servo to the starting position
        delay(1000);                                // Wait for the servo to move
        foodServo.write(90);                        // Move the servo to dispense the food
        delay(1000);                                // Wait for the servo to move

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
        delay(1000);                                // Wait for the water to dispense
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
