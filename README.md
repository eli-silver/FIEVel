# FIEVel
### A Fast InExpensive Velocimeter based on an optical mouse sensor

## CAD
CAD files were created in Autodesk Fusion360. Files are included in both .f3z Fusion archive format to retain parametric edibility, as well as STEP format for use with other CAD packages.

To open the files in Fusion, navigate to a project folder where you would like to add the files. From there the .f3z file can be uploaded and once processed all cad files should be available to edit. 

Model parameters for adjusting the focal length of the M12 lens and the desired magnification of the optical system can be accessed in the Design workspace. Design > Modify > Change Parameters will open a dialog box where model parameters can be edited. After modifying the magnification, it is common for a fillet operation to break (don't know why). To fix, find the broken fillet highlighted yellow on the bottom of the design workspace in the feature timeline.  Right click > Edit the fillet feature and press enter to recompile the fillet without changing any of the selected geometry or parameters.

## Firmware
Firmware in this project was adapted from the work of others. The library used to communicate with the ADNS3080 sensor was adapted from [RCmags ADNS3080 github](https://github.com/RCmags/ADNS3080) with minor modification to allow the use of a Teensy4.1 development board. RCmags' software was released under the MIT License which has been added as a header to all adapted files.

Original work on methods to increase the sample rate of the sensor is contained mostly in the /src/main.cpp file.

Flashing firmware to the Teensy has been tested using the PlatformIO and VS-Code. To flash the teensy, open VS-Code and install the PlatformIO IDE extension from the extension marketplace. From within platform IO, open an existing project and navigate to the /FIEVel/Firmware folder which contains the platformio.ini project configuration file. PlatformIO will request to install necessary dependencies to build and upload the Teensy4.1 project. 

A good tutorial for using PlatformIO with Teensy can be found at: [pjrc forum tutorial](https://forum.pjrc.com/index.php?threads/tutorial-how-to-use-platformio-visual-code-studio-for-teensy.66674/)
with the full documentation available at: [PlatformIO Teensy docs](https://docs.platformio.org/en/latest/platforms/teensy.html)

## Frame Capture
Visualization of the sensor image has been adapted from [RCmags frame capture github](https://github.com/RCmags/ADNS3080_frame_capture) with changes to allow the program to update faster. This faster frame rate is possible because of the higher communication bandwidth of both ADNS3080-Teensy SPI communication and Teensy-computer USB connection compared to the original documentation designed to work with an Arduino Uno. 

## Visualization
This folder contains python scripts to capture and log raw sensor data as well as create plots of the sensor output.

Data logging has been done using the serial_output_log.py script. To use this script, change the value of SERIAL_PORT to match the teensy port name. On MAC and Linux this will be of the form '/dev/xxxusbxxx' while on windows this will be of the form 'COM4'. Log files will be named with the value of FILE_NAME followed by a timestamp. This script will log data until it has written the number of lines specified by NUM_LINES. 

The Jupyter notebooks contained in this folder were used for early data visualizations and debugging of the sensor acquisition system. They are included as an example of how to parse the captured data in python but are likely of limited use beyond their intended application. 

## License:
FIEVel is licensed under the CC BY-SA 4.0 Creative Commons license. 