# ThermoAndPressure

This guide will explain how to modify the calibration values in the code in order to correct any errors in the data.

How the box works:

The box works by reading in an analog value from the ADC, converting it to a voltage, and then multiplying it by a calibration value to get the actual measurement. It is important to note that the UFPM input has a 3/1 voltage divider, so it's analog output value is multiplied accordingly.

To fix errors in measurement, simply modify the correction values at the top of the program. These values are all in terms of Units per volt.
