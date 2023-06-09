# Arduino-Powered Chicken Coop 🐔

Welcome to the Arduino-Powered automatic Chicken Coop project! This project is perfect for hobbyists and small-scale farmers who want to ensure their chickens receive the best care and living conditions possible while inputting minimal continued effort into raising them. Our Arduino-based system offers an affordable and efficient way to manage your chicken coop, leading to happier chickens and a more enjoyable experience for you.

The `All_in_One_Main.cpp` is a compilation of the various separate files representing the chicken coop all together as a single file. The only other files needed are `table_generator.h` and `table_generator.cpp`. This makes it easier to upload to the Arduino. 

# Items Required: 
1. Arduino Mega 2560: $38.00 (estimated, price may vary)
2. Adafruit Motor Shield v2 (1): $22.50
3. Linear actuator or a servo motor with a gear system for opening and closing the door: $30.00 
4. IoT Power Relay: $30.00 (estimated)
5. Load Cell (1): $10.00
6. Jumper Wires - Assorted: $10.00
7. LED strip or bulb $10.00 (estimated)
8. Breadboard (1): $7.00
9. HX711 Load Cell Amplifier (1): $7.00
10. Peristaltic pump (1): $7.00
11. Optical Encoder: $7.00 (estimated)
12. Break Beam Sensor (1): $5.00
13. Breadboard (1): $5.00
15. RTC DS1307 Real-time clock module (1): $5.00
16. TSL2591 light sensor (1): $5.00
17. HX711 load cell amplifier and weight sensor (2): $5.00 each
18. DHT Humidity Sensor (1): $4.00 (estimated, based on the price of RTC DS1307)
19. Jumper wires (approx. 10): $4.00
20. Jumper wires (various): $3.00 for a set of 120
21. DS18B20 temperature sensors (3): $3.00 each
22. 4.7k ohm resistor (3): $0.50 each

## Total cost = ~ $250 

## 🌈 Features

1. **Consistent Light Control**: Brightens your chickens' day by providing a consistent number of hours of light daily, no matter the weather. This promotes their well-being and encourages regular egg-laying patterns.

2. **Automated Door Control**: Automate the opening and closing of the coop door on a predefined schedule, ensuring your chickens can safely roost at night and freely explore during the day.

3. **Humidity Control**: Keep an eye on humidity levels and turns on a small fan when needed.

4. **Food and Water Dispensing**: Use a weight scale, a DC motor, and a peristaltic pump to both dispense food and water automatically. This guarantees that your chickens always have access to fresh food and clean water, keeping them happy and content.

5. **Egg Detection, Weighing, and Recording**: Utilizing a break beam sensor and a scale to detect, count, and weigh eggs as they are laid. This feature allows you to monitor egg production and spot potential issues early on.

6. **Temperature Control**: Ensures that your chickens stay comfortable and healthy by maintaining an ideal temperature inside the coop. It controls a heater and air conditioning unit through IoT power relays, making your coop always cozy, regardless of the weather outside.

## 🚀 Getting Started

To embark on your exciting journey with the Arduino-Powered Chicken Coop project, follow these steps:

1. Clone this repository to your local machine.
2. Customize the settings in each file to suit your specific coop requirements.
3. Upload the code, no folders, to your Arduino-compatible board.
4. Connect the required sensors, components, etc to your board.
5. Power on your Arduino board and enjoy!

## 💡 Contributing

We welcome and appreciate contributions to this project! If you have any bright ideas for improvements or new features, please open an issue to discuss. You can also contribute by submitting pull requests with your changes.

## 🛠️ Potential Issues:
- Humidity fan control operates independently of the temp control. 
- In a worst case, the fan may cause the ac to run longer if it blows cold air outside into a hot environment.

Solution: Integrate the humidity fan control into the temperature control so the temperature is checked before the humidity fan is on, allowing the ac to get a chance to drop the %RH before the humidity fan can turn on if it is hot, preventing them from working against each other. 

## 📜 License

This project is licensed under the MIT License.

## 🎉 Acknowledgements

Thank you for considering the Arduino-Powered Chicken Coop project for your home/farm. Happy farming! 
