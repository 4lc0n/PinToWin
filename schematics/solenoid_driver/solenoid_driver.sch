EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_ST_STM32F4:STM32F411CEUx U1
U 1 1 61390395
P 2550 3450
F 0 "U1" H 2050 5100 50  0000 C CNN
F 1 "STM32F411CEUx" V 2600 4150 50  0000 C CNN
F 2 "Package_DFN_QFN:QFN-48-1EP_7x7mm_P0.5mm_EP5.6x5.6mm" H 1950 1950 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00115249.pdf" H 2550 3450 50  0001 C CNN
	1    2550 3450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 613A56B2
P 2750 5250
F 0 "#PWR02" H 2750 5000 50  0001 C CNN
F 1 "GND" H 2755 5077 50  0000 C CNN
F 2 "" H 2750 5250 50  0001 C CNN
F 3 "" H 2750 5250 50  0001 C CNN
	1    2750 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 5250 2750 5200
Wire Wire Line
	2350 5050 2350 5200
Wire Wire Line
	2350 5200 2450 5200
Connection ~ 2750 5200
Wire Wire Line
	2750 5200 2750 5050
Wire Wire Line
	2450 5050 2450 5200
Connection ~ 2450 5200
Wire Wire Line
	2450 5200 2550 5200
Wire Wire Line
	2550 5050 2550 5200
Connection ~ 2550 5200
Wire Wire Line
	2550 5200 2650 5200
Wire Wire Line
	2650 5050 2650 5200
Connection ~ 2650 5200
Wire Wire Line
	2650 5200 2750 5200
$Sheet
S 8100 4850 1800 850 
U 613AB37F
F0 "Sheet613AB37E" 50
F1 "solenoid_circuit.sch" 50
F2 "Sig_in" I L 8100 5150 50 
F3 "Shunt_out" I L 8100 5450 50 
$EndSheet
Wire Wire Line
	7750 5150 8100 5150
Text GLabel 7750 5150 0    50   Input ~ 0
PWM1
Text GLabel 3500 3450 2    50   Input ~ 0
PWM1
Wire Wire Line
	3500 3450 3150 3450
$Comp
L power:+3.3V #PWR01
U 1 1 613B9AE7
P 2550 1750
F 0 "#PWR01" H 2550 1600 50  0001 C CNN
F 1 "+3.3V" H 2565 1923 50  0000 C CNN
F 2 "" H 2550 1750 50  0001 C CNN
F 3 "" H 2550 1750 50  0001 C CNN
	1    2550 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 1750 2550 1850
Wire Wire Line
	2550 1850 2450 1850
Wire Wire Line
	2450 1850 2450 1950
Wire Wire Line
	2550 1850 2550 1950
Connection ~ 2550 1850
Wire Wire Line
	2650 1950 2650 1850
Wire Wire Line
	2650 1850 2550 1850
Wire Wire Line
	2750 1950 2750 1850
Wire Wire Line
	2750 1850 2650 1850
Connection ~ 2650 1850
$Comp
L power:+3.3V #PWR03
U 1 1 613BAA16
P 3550 1700
F 0 "#PWR03" H 3550 1550 50  0001 C CNN
F 1 "+3.3V" H 3565 1873 50  0000 C CNN
F 2 "" H 3550 1700 50  0001 C CNN
F 3 "" H 3550 1700 50  0001 C CNN
	1    3550 1700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 613BB08D
P 3550 2050
F 0 "#PWR04" H 3550 1800 50  0001 C CNN
F 1 "GND" H 3555 1877 50  0000 C CNN
F 2 "" H 3550 2050 50  0001 C CNN
F 3 "" H 3550 2050 50  0001 C CNN
	1    3550 2050
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C1
U 1 1 613BB72A
P 3150 1850
F 0 "C1" H 3242 1896 50  0000 L CNN
F 1 "0.1u" H 3242 1805 50  0000 L CNN
F 2 "" H 3150 1850 50  0001 C CNN
F 3 "~" H 3150 1850 50  0001 C CNN
	1    3150 1850
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C2
U 1 1 613BBE19
P 3500 1850
F 0 "C2" H 3592 1896 50  0000 L CNN
F 1 "0.1u" H 3592 1805 50  0000 L CNN
F 2 "" H 3500 1850 50  0001 C CNN
F 3 "~" H 3500 1850 50  0001 C CNN
	1    3500 1850
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C3
U 1 1 613BC17E
P 3850 1850
F 0 "C3" H 3942 1896 50  0000 L CNN
F 1 "0.1u" H 3942 1805 50  0000 L CNN
F 2 "" H 3850 1850 50  0001 C CNN
F 3 "~" H 3850 1850 50  0001 C CNN
	1    3850 1850
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C4
U 1 1 613BC767
P 4200 1850
F 0 "C4" H 4292 1896 50  0000 L CNN
F 1 "0.1u" H 4292 1805 50  0000 L CNN
F 2 "" H 4200 1850 50  0001 C CNN
F 3 "~" H 4200 1850 50  0001 C CNN
	1    4200 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 2000 3150 1950
Wire Wire Line
	4200 2000 4200 1950
Wire Wire Line
	3150 2000 3500 2000
Wire Wire Line
	4200 1750 4200 1700
Wire Wire Line
	4200 1700 3850 1700
Wire Wire Line
	3150 1700 3150 1750
Connection ~ 3550 1700
Wire Wire Line
	3550 1700 3500 1700
Wire Wire Line
	3500 1750 3500 1700
Connection ~ 3500 1700
Wire Wire Line
	3500 1700 3150 1700
Wire Wire Line
	3850 1750 3850 1700
Connection ~ 3850 1700
Wire Wire Line
	3850 1700 3550 1700
Wire Wire Line
	3500 1950 3500 2000
Connection ~ 3500 2000
Wire Wire Line
	3500 2000 3550 2000
Wire Wire Line
	3850 1950 3850 2000
Connection ~ 3850 2000
Wire Wire Line
	3850 2000 4200 2000
Wire Wire Line
	3550 2050 3550 2000
Connection ~ 3550 2000
Wire Wire Line
	3550 2000 3850 2000
$Comp
L Amplifier_Operational:LM324 U?
U 1 1 616CD86C
P 8050 1650
F 0 "U?" H 8050 2017 50  0000 C CNN
F 1 "LM324" H 8050 1926 50  0000 C CNN
F 2 "" H 8000 1750 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm2902-n.pdf" H 8100 1850 50  0001 C CNN
	1    8050 1650
	1    0    0    -1  
$EndComp
Text GLabel 7750 5450 0    50   Input ~ 0
Shunt1_out
Wire Wire Line
	7750 5450 8100 5450
Text GLabel 7250 1550 0    50   Input ~ 0
Shunt1_out
Wire Wire Line
	7250 1550 7750 1550
$Comp
L Device:R R?
U 1 1 616D0DDE
P 8550 1900
F 0 "R?" H 8620 1946 50  0000 L CNN
F 1 "66k" H 8620 1855 50  0000 L CNN
F 2 "" V 8480 1900 50  0001 C CNN
F 3 "~" H 8550 1900 50  0001 C CNN
	1    8550 1900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 616D1600
P 8550 2300
F 0 "R?" H 8620 2346 50  0000 L CNN
F 1 "10k" H 8620 2255 50  0000 L CNN
F 2 "" V 8480 2300 50  0001 C CNN
F 3 "~" H 8550 2300 50  0001 C CNN
	1    8550 2300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 616D1B4E
P 8550 2550
F 0 "#PWR?" H 8550 2300 50  0001 C CNN
F 1 "GND" H 8555 2377 50  0000 C CNN
F 2 "" H 8550 2550 50  0001 C CNN
F 3 "" H 8550 2550 50  0001 C CNN
	1    8550 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 2550 8550 2450
Wire Wire Line
	8550 2150 8550 2100
Wire Wire Line
	8550 1750 8550 1650
Wire Wire Line
	8550 1650 8350 1650
Wire Wire Line
	7750 1750 7550 1750
Wire Wire Line
	7550 1750 7550 2100
Wire Wire Line
	7550 2100 8550 2100
Connection ~ 8550 2100
Wire Wire Line
	8550 2100 8550 2050
Text GLabel 8800 1650 2    50   Input ~ 0
Sol1_Current_Sense
Wire Wire Line
	8800 1650 8550 1650
Connection ~ 8550 1650
Text Notes 7350 1200 0    50   ~ 0
ampl. factor: 7.5
Text GLabel 3500 3550 2    50   Input ~ 0
Sol1_Current_Sense
$EndSCHEMATC
