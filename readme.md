# Pin To Win - Working repository for electronics

## Overview

This repository represents the electronic part of the project 'Pin To Win', a project in the scope of the lecutre 'Smart Systems' at the USN Kongsberg. 

The project is a collaboration of students from electrical and mechanical engineering and computer science students. 

Members: 
* Kristian S. Iversen
* Petter Wang Tveit
* Jostein Garai Davidsen
* Jürgen Josef Markl
* Marcus Ringen Ulleberg


## Content

### Concepts

Drawings and test files used to sketch out parts of the projects

### doc

Documents regarding this project including documentation, data sheets and other application notes and reference sheets


### uC

#### solenoid_driver
STM32CubeIDE-Projects for the Microcontroller (STM32F411,  BlackPill V3 designed by WeAct), used to controll the hardware, especially the solenoids. Represents main processor in this project

#### matrix controller
STM32CubeIDE-Projects for the Microcontroller (STM32F411) to sample light dependent resistors in a matrix of 8x11 and send via USB on request

#### matrix simulation dev
STM32CubeIDE-Project for a µC to send random data to simulate the matrix, as it was not finished yet

#### pinbal detection V0
STM32CubeIDE for breadboard prototype of LDR matrix with a 4x4 matrix

### schematics

Schematics and other files related to this for the projects PCBs
#### main control
Board to controll solenoids, some light sensors and temperature sensors

#### sensor matrix
Board with Light Dependent Resistors in a matrix

#### solenoid driver
KiCAD project for first draft of the solenoid driver, a low side mosfet with a push-pull driver circuit

#### parts
custom parts and footprints for OrCAD Cadence projects

### simulations

Simulations with LTSpice regarding some components





