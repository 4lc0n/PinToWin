# Pin To Win - Working repository for electronics


<p align="center">
  <img src="images/IMG_3024.JPG" width="500px">
</p>



## IMPORTANT

This repository has long file names, for windows user: enable long file names with this command

git config --system core.longpaths true



## Overview

This repository represents the electronic part of the project 'Pin To Win', a project in the scope of the lecutre 'Smart Systems' at the USN Kongsberg. 

The project is a collaboration of students from electrical and mechanical engineering and computer science students. 

Members: 
* Kristian S. Iversen
* Petter Wang Tveit
* Jostein Garai Davidsen
* Jürgen Josef Markl
* Marcus Ringen Ulleberg

For more information on the project, see the blog at http://dronesonen.usn.no/?author=84

## Content

### PinballSim-main/Pinball 
Unity Simulation by Marcus

### Concepts

Drawings and test files used to sketch out parts of the projects

### doc
---
Documents regarding this project including documentation, data sheets and other application notes and reference sheets

### schematics
---

Schematics and other files related to this for the projects PCBs

* main control:  
Board to controll solenoids, some light sensors and temperature sensors

* sensor matrix:  
Board with Light Dependent Resistors in a matrix

* solenoid driver:  
KiCAD project for first draft of the solenoid driver, a low side mosfet with a push-pull driver circuit

* parts:  
custom parts and footprints for OrCAD Cadence projects

### simulations
---
Simulations with LTSpice regarding some components like 
* Solenoid Driver (Mosfet Driver)
* Current Calculation on Solenoids


### uC
---
* solenoid_driver:  
STM32CubeIDE-Projects for the Microcontroller (STM32F411,  BlackPill V3 designed by WeAct), used to controll the hardware, especially the solenoids. Represents main processor in this project

* matrix controller:  
STM32CubeIDE-Projects for the Microcontroller (STM32F411) to sample light dependent resistors in a matrix of 8x11 and send via USB on request

* matrix simulation dev:  
STM32CubeIDE-Project for a µC to send random data to simulate the matrix, as it was not finished yet

* pinbal detection V0:  
STM32CubeIDE for breadboard prototype of LDR matrix with a 4x4 matrix

* AVR:  
contains FreeRTOS implementation on ATMega2560 (Arduino Mega). Main project lies in PlatformIO workspace, the Arduino_FreeRTOS port is used, as the native didn't feel like running

Multi-Tasking firmware with custom drivers for ADC and UART for effective data throughput
reads pins for sensors, targets, Raspberry Pi output, button input, etc. 





## Note for further development

To get this thing up and running, 2 Raspberry were used, but only 1 is also sufficient. 

All matrices were connected via USB-C cables to the Raspberry, appearing as ACM0 and ACM1 (numbering can be different), the ATMega2560 was connected via a FTDI USB to Serial adapter, as this will show up as USB0, so can be easily identified. 

A strong 5V power supply is needed to prevent low power warnings on the raspberry and the Arduino to brown-out!

Having a common ground between the Raspberry with the control output and the ATMega2560 with the input for the flipper pins is also important, and is given when connecting either a jumper wire between the two, or having the ATMega2560 be powered from the Raspberry Pi. 

Getting the score board script to automaticly open the port did not work in our case, so it has to be started manually. Also the c++-programm for autonomous playing needs to be started manually.
