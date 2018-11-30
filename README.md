# Nutcracker Grandfather Clock
The following is the readme file for the code used to control the grandfather clock used for SBDC Nutcracker productions

## Usage Instructions
BT Device name =   Scott-Clock
There is a password for the device.  Password is stored on SBDC shared 1password account.

You will need to download application for android phone called
"Serial Bluetooth Terminal 1.18" available in the Google App Store/Google Play.

Once downloaded , start the application in the phone , power on the clock and then turn on BT on the phone as well. Scan for devices looking for Scott-Clock ... connect secure and use password to pair

Once Paired , reset the clock system ( Red Button on At Mega board ).

You are all set ... a Menu will appear on the phone BT application and you can start stop test etc as indicated in the menu.

## Hardware Information
Power input 12V
Regulated to 5V for the Controller ., Leds and Logic.

The 4 Leds
Green ... POWER power to board and Motor is on.
Red is StandBY and will Blink when START button is hit and it is in animation mode
Yellow is Motor1 operation blink / off when not operating
Blue is Motor2 operation. blink / off when not operating

The Motors M1 and M2 are steppers through the A4988 Allegro controllers. and take 2 GPIO each. ( Step and direction ). Driven with the AccellMotor Library in the Arduino space.

the main board brown will have the Atmega2560 ontop of itself with stand offs . The main brown board will be on standoffs to Scoitts Module.

The green boards are

2X opto boards
2X button boards with Schmidt trigger and hardware denounce. I have built only 1 at the moment. as we can use reset on the mega2560 to Home the motors as well till I make the second one later

BlueTooth trigger:  HC-SR05 board
