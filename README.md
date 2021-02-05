# Temperature-Adjustifier
Built a system which is smart enough to detect motion, and changes in temperature and lastly talk to the Google Nest to update the temperature. 

# Materials Used
Serial No     | Component Name | Specification | Units   
------------- | ------------- | ------------- | -------------
1  | Motion Sensor | HC-SR501  | 2
2  | Temperature Sensor  | LM35-DZ  | 2
3  | Microcontroller  | Atmega-168  | 1
4  | Capacitor  | 22pF Ceramic Capacitor  | 1
5  | Resistor  | Resistor  | 2
6  | Jump Wires  | Breadboard Jumper Wires  | 10-15
7  | LEDs  | Green and Red LED | 1 each
8  | Serial to USB cable  | NA  | 1
9  | Thermostat  | Nest Thermostat E  | 1
10  | Server/Laptop  | MacBook Pro  | 1
11  | Bread Boards  | 1 big and small | 2

# How to Setup:
This section only contain aspects of the project which are reproduceable apart from the firmware. 
* Setting Up Google Nest and Goolge Dev Account

Assuming the user has a NEST account registered to a google account and a registered device
access program account [here](https://console.nest.google.com/device-access).
The user needs to follow the steps from [here](https://developers.google.com/nest/device-access/get-started) , it’s pretty well documented and as an additional
resource this [link](https://www.youtube.com/watch?v=_Wex2SLmEJ8&list=WL&index=106&t=348s) should help.
Basically at the end we’ll end up with a device ID , Project ID , access token and refresh token.
The access token is only valid for 1 hour, use the refresh token to get a fresh access token. The
aforementioned link has details as to how to request for a new access token.
Then under the “Use the API” section we can find the API endpoint to list structures and devices.
We use this endpoint to get the current temperature on the thermostat.
To get the endpoint for updating the thermostat we need to head over to this documentation page
under the SetCool section we can find the endpoint for updating the thermostat.

* Setting Up the Python script and Cron Jobs
  * Python Code
     For the development of the project python 3.7 was used. Most of the libraries used for were
default libraries, we just need to install PySerial, which can be installed using pip install pyserial.
To run the python code we need to type “python serialConn.py >> LOG.txt” what it does is
writes all the print commands to a file called LOG.

  * Setup the cron job.
     To configure the cron job, you’ll need to know your path to the python environment, which can
be found by typing “where python” on the terminal. Now, to start a cron job you’ll need to do the
following things:
      * On the terminal if you’re on the directory where SerialConn.py lives you need to copy the
complete path to this location which can be found by typing “pwd” on the terminal.
      * Type crontab -e to edit a new crontab file. You can use nano or Vim editors for this. If
you’re on mac and have issues with Vim type “EDITOR=/usr/bin/vim crontab -e”. It’ll
open a new file there you need to type the command as 

```
*/5 * * * * cd /Path/To/python/file && /Path/to python evironment/python serialConn.py >> LOG.txt
```


# How to run the firmware code.

No additional library apart from UART.c was used. To flash the code we need to type `make
flash`. To open the serial terminal just type `screen <usb serial port> 9600`


# Circuit diagram

#### [Video Demonstration of the Project](https://www.youtube.com/watch?v=9yAFCZDqVdw "Named link title")

