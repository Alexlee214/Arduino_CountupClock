# Description
The original intentional of the project is to make a simple pocket device for my mom as her 2018 Mother's Day gift. The device houses a joy stick, 2 buttons and an LCD display. it has several modes to choose from, including keeping track of the time that has lapsed since a certain date (All of the calculations are done onboard). 
Required components are as follows:
 * ATMega328P-PU
 * DIP-28 socket
 * 1602 LCD display
 * Joystick
 * Pushbuttons x 2
 


## Current functionalities
* May 5, 2018, Version 1.0.0
 * Does not require an RTC module
 * Date and time setup when first powered on, uses the joystick by the display for intuitive fine tuning
 * Push button to switch between modes
 * Uses another push button to turn the LCD backlight on and off to save energy
 * EEPROM to store the date and time since the last time the device was on
 * Ability to calculate the number of years, months, weeks, days, and hours since a certain date
 * Determine leap years and and number of days in each months
 * Displays the mode number in the bottom left corner
 * Buttons use external interrupts so that they are not affected by other tasks currently running
 * Buttons exploits the internal pullup resistor
 * Mode1: Message display
 * Mode2: clock with date and time
 * Mode3: display the number of years since a certain date specified(stored in EEPROM)
 * Mode4: display the number of months since a certain date specified(stored in EEPROM)
 * Mode5: display the number of weeks since a certain date specified(stored in EEPROM)
 * Mode6: display the number of days since a certain date specified(stored in EEPROM)
 * Mode7: display the number of hours since a certain date specified(stored in EEPROM)


## Future improvements
* Add mode: timer - uses an piezo buzzer
* Set high fuse bits so that the EEPROM is not erased everytime on the standalone ATMega328 chip


## Appendix
* Set up:
 - Switch off all the light sources in the room
 - Plug in the LED strip
 - Wait for 1 second and the LED strip should turn on for 1 second
 - After the LED strip turns off once again, the setup process is complete
 
* Pinouts on ATTiny85
 - Pin #1 (RESET)
 - Pin #2 (pin 3 or pin A1) : LED light to indicate in auto mode
 - Pin #3 (pin 4 or pin A2) : Photoresistor 
 - Pin #4 (GROUND) : all grounds must be connected together
 - Pin #5 (pin 0 PWM) : output signal to the 5v relay
 - Pin #6 (pin 1 PWM) : push button 1 for manual mode
 - Pin #7 (pin 2 or pin A1) : push button 2 for auto mode
 - Pin #4 (VCC) : 5V coming out from the step down converter
