# ThermoAndPressure

This guide will explain how to modify the calibration values in the code in order to correct any errors in the data.

How the box works:

The box works by reading in an analog value from the ADC, converting it to a voltage, and then multiplying it by a calibration value to get the actual measurement. It is important to note that the UFPM input has a 3/1 voltage divider, so it's analog output value is multiplied accordingly.

How to use it:

1. Plug in all data collection cables into the box
2. Plug the box into your computer
3. Open Terminal.exe
4. Select the appropriate COM port from the dropdown menu and press "Connect", make sure the baud rate is set to 9600.
Note: If the COM port for the Teensy does not show up, see "Fixing COM Port Issues" below
5. Under "Receive", select "Start Log" and select an appropriate location to save the log file, also check the box "LogDateStamp".
6. When you're ready to start recording data, enter a "1" into the long dialog box located at the bottom of the window and press send.
7. The Teensy should start outputing data in the serial terminal in the middle of the window.
8. Once you're done collecting data, enter a "0" into the long dialog box and press send.
9. Import the log file into Excel as a CSV and you should be good to go!



To fix errors in measurement, simply modify the correction values at the top of the program. These values are all in terms of Units per volt.

How to reflash the code:

1. Download the file called ThermoAndPressure, and open it in Arduino.
2. Go to Tools->Board Type and select Teensy 3.6.
3. Go to Tools->USB Type and select Serial.
4. Press the left arrow at the top left corner of the window to reflash the code.
