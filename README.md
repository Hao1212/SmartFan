# SmartFan
A smart fan project, controls fan actions according to sensors and user-defined data.

Board: ESP32   
Connection: WiFi   
Temperature sensor: TMP36

Cooling model:   
&nbsp;&nbsp;Input: temperature sensor   
&nbsp;&nbsp;Function: to determine what temperature would be desired outcome then power attached fan. Fan speed is controllable, according to temperature thresholds, or manual input.

Cooling small electronic device:   
&nbsp;&nbsp;Input: temperature sensor.   
&nbsp;&nbsp;Function: to cool a Arduino device down

Manual mode   
&nbsp;&nbsp;Input: buttons, potentiometer   
&nbsp;&nbsp;Function: to manually adjust fan power, change default temperature, etc

Extension: LCD display   
&nbsp;&nbsp;Input: TBD   
&nbsp;&nbsp;Function: sensor data display.

Extension: weather data:   
&nbsp;&nbsp;Function: dynamically set appropriate target temperature, given the absence of a user-defined default   

The temperature sensor inputs will be used alongside the settings in temperature sensing mode to determine what temperature regulation action needs to be taken
