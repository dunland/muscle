EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
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
L teensy:Teensy3.2 U1
U 1 1 6108280B
P 6550 3800
F 0 "U1" H 6550 5437 60  0000 C CNN
F 1 "Teensy3.2" H 6550 5331 60  0000 C CNN
F 2 "" H 6550 3050 60  0001 C CNN
F 3 "" H 6550 3050 60  0000 C CNN
	1    6550 3800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 610D6A47
P 1400 2600
F 0 "#PWR05" H 1400 2350 50  0001 C CNN
F 1 "GND" H 1405 2427 50  0000 C CNN
F 2 "" H 1400 2600 50  0001 C CNN
F 3 "" H 1400 2600 50  0001 C CNN
	1    1400 2600
	-1   0    0    1   
$EndComp
$Comp
L Device:R R11
U 1 1 610D6A4D
P 1800 2850
F 0 "R11" H 1730 2804 50  0000 R CNN
F 1 "100K" H 1730 2895 50  0000 R CNN
F 2 "" V 1730 2850 50  0001 C CNN
F 3 "~" H 1800 2850 50  0001 C CNN
	1    1800 2850
	-1   0    0    1   
$EndComp
Wire Wire Line
	1100 2700 1400 2700
$Comp
L Device:R R19
U 1 1 610D6A54
P 2150 2700
F 0 "R19" V 2357 2700 50  0000 C CNN
F 1 "10K" V 2266 2700 50  0000 C CNN
F 2 "" V 2080 2700 50  0001 C CNN
F 3 "~" H 2150 2700 50  0001 C CNN
	1    2150 2700
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R3
U 1 1 610D6A5A
P 1400 2850
F 0 "R3" H 1330 2804 50  0000 R CNN
F 1 "100K" H 1330 2895 50  0000 R CNN
F 2 "" V 1330 2850 50  0001 C CNN
F 3 "~" H 1400 2850 50  0001 C CNN
	1    1400 2850
	-1   0    0    1   
$EndComp
Connection ~ 1400 2700
Wire Wire Line
	1400 2700 1800 2700
Wire Wire Line
	2000 2700 1800 2700
Connection ~ 1800 2700
Wire Wire Line
	1100 2600 1400 2600
$Comp
L power:GND #PWR06
U 1 1 610D6A66
P 1400 3000
F 0 "#PWR06" H 1400 2750 50  0001 C CNN
F 1 "GND" H 1405 2827 50  0000 C CNN
F 2 "" H 1400 3000 50  0001 C CNN
F 3 "" H 1400 3000 50  0001 C CNN
	1    1400 3000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 610DA25A
P 1400 3500
F 0 "#PWR07" H 1400 3250 50  0001 C CNN
F 1 "GND" H 1405 3327 50  0000 C CNN
F 2 "" H 1400 3500 50  0001 C CNN
F 3 "" H 1400 3500 50  0001 C CNN
	1    1400 3500
	-1   0    0    1   
$EndComp
$Comp
L Device:R R12
U 1 1 610DA260
P 1800 3750
F 0 "R12" H 1730 3704 50  0000 R CNN
F 1 "100K" H 1730 3795 50  0000 R CNN
F 2 "" V 1730 3750 50  0001 C CNN
F 3 "~" H 1800 3750 50  0001 C CNN
	1    1800 3750
	-1   0    0    1   
$EndComp
Wire Wire Line
	1100 3600 1400 3600
$Comp
L Device:R R20
U 1 1 610DA267
P 2150 3600
F 0 "R20" V 2357 3600 50  0000 C CNN
F 1 "10K" V 2266 3600 50  0000 C CNN
F 2 "" V 2080 3600 50  0001 C CNN
F 3 "~" H 2150 3600 50  0001 C CNN
	1    2150 3600
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R4
U 1 1 610DA26D
P 1400 3750
F 0 "R4" H 1330 3704 50  0000 R CNN
F 1 "100K" H 1330 3795 50  0000 R CNN
F 2 "" V 1330 3750 50  0001 C CNN
F 3 "~" H 1400 3750 50  0001 C CNN
	1    1400 3750
	-1   0    0    1   
$EndComp
Connection ~ 1400 3600
Wire Wire Line
	1400 3600 1800 3600
Wire Wire Line
	2000 3600 1800 3600
Connection ~ 1800 3600
Wire Wire Line
	1100 3500 1400 3500
$Comp
L power:GND #PWR09
U 1 1 610DB500
P 1400 4400
F 0 "#PWR09" H 1400 4150 50  0001 C CNN
F 1 "GND" H 1405 4227 50  0000 C CNN
F 2 "" H 1400 4400 50  0001 C CNN
F 3 "" H 1400 4400 50  0001 C CNN
	1    1400 4400
	-1   0    0    1   
$EndComp
$Comp
L Device:R R13
U 1 1 610DB506
P 1800 4650
F 0 "R13" H 1730 4604 50  0000 R CNN
F 1 "100K" H 1730 4695 50  0000 R CNN
F 2 "" V 1730 4650 50  0001 C CNN
F 3 "~" H 1800 4650 50  0001 C CNN
	1    1800 4650
	-1   0    0    1   
$EndComp
Wire Wire Line
	1100 4500 1400 4500
$Comp
L Device:R R21
U 1 1 610DB50D
P 2150 4500
F 0 "R21" V 2357 4500 50  0000 C CNN
F 1 "10K" V 2266 4500 50  0000 C CNN
F 2 "" V 2080 4500 50  0001 C CNN
F 3 "~" H 2150 4500 50  0001 C CNN
	1    2150 4500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R5
U 1 1 610DB513
P 1400 4650
F 0 "R5" H 1330 4604 50  0000 R CNN
F 1 "100K" H 1330 4695 50  0000 R CNN
F 2 "" V 1330 4650 50  0001 C CNN
F 3 "~" H 1400 4650 50  0001 C CNN
	1    1400 4650
	-1   0    0    1   
$EndComp
Connection ~ 1400 4500
Wire Wire Line
	1400 4500 1800 4500
Wire Wire Line
	2000 4500 1800 4500
Connection ~ 1800 4500
Wire Wire Line
	1100 4400 1400 4400
$Comp
L power:GND #PWR010
U 1 1 610DB51F
P 1400 4800
F 0 "#PWR010" H 1400 4550 50  0001 C CNN
F 1 "GND" H 1405 4627 50  0000 C CNN
F 2 "" H 1400 4800 50  0001 C CNN
F 3 "" H 1400 4800 50  0001 C CNN
	1    1400 4800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 610DD293
P 1400 5300
F 0 "#PWR011" H 1400 5050 50  0001 C CNN
F 1 "GND" H 1405 5127 50  0000 C CNN
F 2 "" H 1400 5300 50  0001 C CNN
F 3 "" H 1400 5300 50  0001 C CNN
	1    1400 5300
	-1   0    0    1   
$EndComp
$Comp
L Device:R R14
U 1 1 610DD299
P 1800 5550
F 0 "R14" H 1730 5504 50  0000 R CNN
F 1 "100K" H 1730 5595 50  0000 R CNN
F 2 "" V 1730 5550 50  0001 C CNN
F 3 "~" H 1800 5550 50  0001 C CNN
	1    1800 5550
	-1   0    0    1   
$EndComp
Wire Wire Line
	1100 5400 1400 5400
$Comp
L Device:R R22
U 1 1 610DD2A0
P 2150 5400
F 0 "R22" V 2357 5400 50  0000 C CNN
F 1 "10K" V 2266 5400 50  0000 C CNN
F 2 "" V 2080 5400 50  0001 C CNN
F 3 "~" H 2150 5400 50  0001 C CNN
	1    2150 5400
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R6
U 1 1 610DD2A6
P 1400 5550
F 0 "R6" H 1330 5504 50  0000 R CNN
F 1 "100K" H 1330 5595 50  0000 R CNN
F 2 "" V 1330 5550 50  0001 C CNN
F 3 "~" H 1400 5550 50  0001 C CNN
	1    1400 5550
	-1   0    0    1   
$EndComp
Connection ~ 1400 5400
Wire Wire Line
	1400 5400 1800 5400
Wire Wire Line
	2000 5400 1800 5400
Connection ~ 1800 5400
Wire Wire Line
	1100 5300 1400 5300
$Comp
L power:GND #PWR012
U 1 1 610DD2B2
P 1400 5700
F 0 "#PWR012" H 1400 5450 50  0001 C CNN
F 1 "GND" H 1405 5527 50  0000 C CNN
F 2 "" H 1400 5700 50  0001 C CNN
F 3 "" H 1400 5700 50  0001 C CNN
	1    1400 5700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 610DF669
P 1400 6200
F 0 "#PWR013" H 1400 5950 50  0001 C CNN
F 1 "GND" H 1405 6027 50  0000 C CNN
F 2 "" H 1400 6200 50  0001 C CNN
F 3 "" H 1400 6200 50  0001 C CNN
	1    1400 6200
	-1   0    0    1   
$EndComp
$Comp
L Device:R R15
U 1 1 610DF66F
P 1800 6450
F 0 "R15" H 1730 6404 50  0000 R CNN
F 1 "100K" H 1730 6495 50  0000 R CNN
F 2 "" V 1730 6450 50  0001 C CNN
F 3 "~" H 1800 6450 50  0001 C CNN
	1    1800 6450
	-1   0    0    1   
$EndComp
Wire Wire Line
	1100 6300 1400 6300
$Comp
L Device:R R23
U 1 1 610DF676
P 2150 6300
F 0 "R23" V 2357 6300 50  0000 C CNN
F 1 "10K" V 2266 6300 50  0000 C CNN
F 2 "" V 2080 6300 50  0001 C CNN
F 3 "~" H 2150 6300 50  0001 C CNN
	1    2150 6300
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R7
U 1 1 610DF67C
P 1400 6450
F 0 "R7" H 1330 6404 50  0000 R CNN
F 1 "100K" H 1330 6495 50  0000 R CNN
F 2 "" V 1330 6450 50  0001 C CNN
F 3 "~" H 1400 6450 50  0001 C CNN
	1    1400 6450
	-1   0    0    1   
$EndComp
Connection ~ 1400 6300
Wire Wire Line
	1400 6300 1800 6300
Wire Wire Line
	2000 6300 1800 6300
Connection ~ 1800 6300
Wire Wire Line
	1100 6200 1400 6200
$Comp
L power:GND #PWR014
U 1 1 610DF688
P 1400 6600
F 0 "#PWR014" H 1400 6350 50  0001 C CNN
F 1 "GND" H 1405 6427 50  0000 C CNN
F 2 "" H 1400 6600 50  0001 C CNN
F 3 "" H 1400 6600 50  0001 C CNN
	1    1400 6600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 610E09E4
P 1400 7050
F 0 "#PWR015" H 1400 6800 50  0001 C CNN
F 1 "GND" H 1405 6877 50  0000 C CNN
F 2 "" H 1400 7050 50  0001 C CNN
F 3 "" H 1400 7050 50  0001 C CNN
	1    1400 7050
	-1   0    0    1   
$EndComp
$Comp
L Device:R R16
U 1 1 610E09EA
P 1800 7300
F 0 "R16" H 1730 7254 50  0000 R CNN
F 1 "100K" H 1730 7345 50  0000 R CNN
F 2 "" V 1730 7300 50  0001 C CNN
F 3 "~" H 1800 7300 50  0001 C CNN
	1    1800 7300
	-1   0    0    1   
$EndComp
Wire Wire Line
	1100 7150 1400 7150
$Comp
L Device:R R24
U 1 1 610E09F1
P 2150 7150
F 0 "R24" V 2357 7150 50  0000 C CNN
F 1 "10K" V 2266 7150 50  0000 C CNN
F 2 "" V 2080 7150 50  0001 C CNN
F 3 "~" H 2150 7150 50  0001 C CNN
	1    2150 7150
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R8
U 1 1 610E09F7
P 1400 7300
F 0 "R8" H 1330 7254 50  0000 R CNN
F 1 "100K" H 1330 7345 50  0000 R CNN
F 2 "" V 1330 7300 50  0001 C CNN
F 3 "~" H 1400 7300 50  0001 C CNN
	1    1400 7300
	-1   0    0    1   
$EndComp
Connection ~ 1400 7150
Wire Wire Line
	1400 7150 1800 7150
Wire Wire Line
	2000 7150 1800 7150
Connection ~ 1800 7150
Wire Wire Line
	1100 7050 1400 7050
$Comp
L power:GND #PWR016
U 1 1 610E0A03
P 1400 7450
F 0 "#PWR016" H 1400 7200 50  0001 C CNN
F 1 "GND" H 1405 7277 50  0000 C CNN
F 2 "" H 1400 7450 50  0001 C CNN
F 3 "" H 1400 7450 50  0001 C CNN
	1    1400 7450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 610E2000
P 1400 1700
F 0 "#PWR03" H 1400 1450 50  0001 C CNN
F 1 "GND" H 1405 1527 50  0000 C CNN
F 2 "" H 1400 1700 50  0001 C CNN
F 3 "" H 1400 1700 50  0001 C CNN
	1    1400 1700
	-1   0    0    1   
$EndComp
$Comp
L Device:R R10
U 1 1 610E2006
P 1800 1950
F 0 "R10" H 1730 1904 50  0000 R CNN
F 1 "100K" H 1730 1995 50  0000 R CNN
F 2 "" V 1730 1950 50  0001 C CNN
F 3 "~" H 1800 1950 50  0001 C CNN
	1    1800 1950
	-1   0    0    1   
$EndComp
Wire Wire Line
	1100 1800 1400 1800
$Comp
L Device:R R18
U 1 1 610E200D
P 2150 1800
F 0 "R18" V 2357 1800 50  0000 C CNN
F 1 "10K" V 2266 1800 50  0000 C CNN
F 2 "" V 2080 1800 50  0001 C CNN
F 3 "~" H 2150 1800 50  0001 C CNN
	1    2150 1800
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 610E2013
P 1400 1950
F 0 "R2" H 1330 1904 50  0000 R CNN
F 1 "100K" H 1330 1995 50  0000 R CNN
F 2 "" V 1330 1950 50  0001 C CNN
F 3 "~" H 1400 1950 50  0001 C CNN
	1    1400 1950
	-1   0    0    1   
$EndComp
Connection ~ 1400 1800
Wire Wire Line
	1400 1800 1800 1800
Wire Wire Line
	2000 1800 1800 1800
Connection ~ 1800 1800
Wire Wire Line
	1100 1700 1400 1700
$Comp
L power:GND #PWR04
U 1 1 610E201F
P 1400 2100
F 0 "#PWR04" H 1400 1850 50  0001 C CNN
F 1 "GND" H 1405 1927 50  0000 C CNN
F 2 "" H 1400 2100 50  0001 C CNN
F 3 "" H 1400 2100 50  0001 C CNN
	1    1400 2100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR01
U 1 1 61108487
P 1400 800
F 0 "#PWR01" H 1400 550 50  0001 C CNN
F 1 "GND" H 1405 627 50  0000 C CNN
F 2 "" H 1400 800 50  0001 C CNN
F 3 "" H 1400 800 50  0001 C CNN
	1    1400 800 
	-1   0    0    1   
$EndComp
$Comp
L Device:R R9
U 1 1 6110848D
P 1800 1050
F 0 "R9" H 1730 1004 50  0000 R CNN
F 1 "100K" H 1730 1095 50  0000 R CNN
F 2 "" V 1730 1050 50  0001 C CNN
F 3 "~" H 1800 1050 50  0001 C CNN
	1    1800 1050
	-1   0    0    1   
$EndComp
Wire Wire Line
	1100 900  1400 900 
$Comp
L Device:R R17
U 1 1 61108494
P 2150 900
F 0 "R17" V 2357 900 50  0000 C CNN
F 1 "10K" V 2266 900 50  0000 C CNN
F 2 "" V 2080 900 50  0001 C CNN
F 3 "~" H 2150 900 50  0001 C CNN
	1    2150 900 
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R1
U 1 1 6110849A
P 1400 1050
F 0 "R1" H 1330 1004 50  0000 R CNN
F 1 "100K" H 1330 1095 50  0000 R CNN
F 2 "" V 1330 1050 50  0001 C CNN
F 3 "~" H 1400 1050 50  0001 C CNN
	1    1400 1050
	-1   0    0    1   
$EndComp
Connection ~ 1400 900 
Wire Wire Line
	1400 900  1800 900 
Wire Wire Line
	2000 900  1800 900 
Connection ~ 1800 900 
Wire Wire Line
	1100 800  1400 800 
$Comp
L power:GND #PWR02
U 1 1 611084A6
P 1400 1200
F 0 "#PWR02" H 1400 950 50  0001 C CNN
F 1 "GND" H 1405 1027 50  0000 C CNN
F 2 "" H 1400 1200 50  0001 C CNN
F 3 "" H 1400 1200 50  0001 C CNN
	1    1400 1200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR038
U 1 1 611451FF
P 9050 4100
F 0 "#PWR038" H 9050 3850 50  0001 C CNN
F 1 "GND" V 9055 3972 50  0000 R CNN
F 2 "" H 9050 4100 50  0001 C CNN
F 3 "" H 9050 4100 50  0001 C CNN
	1    9050 4100
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR037
U 1 1 611459C9
P 9050 4000
F 0 "#PWR037" H 9050 3850 50  0001 C CNN
F 1 "+5V" V 9065 4128 50  0000 L CNN
F 2 "" H 9050 4000 50  0001 C CNN
F 3 "" H 9050 4000 50  0001 C CNN
	1    9050 4000
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR036
U 1 1 61146C92
P 9050 3900
F 0 "#PWR036" H 9050 3650 50  0001 C CNN
F 1 "GND" V 9055 3772 50  0000 R CNN
F 2 "" H 9050 3900 50  0001 C CNN
F 3 "" H 9050 3900 50  0001 C CNN
	1    9050 3900
	0    1    1    0   
$EndComp
Wire Wire Line
	9050 3800 8200 3800
Wire Wire Line
	8200 3800 8200 2000
Wire Wire Line
	8200 2000 5400 2000
Wire Wire Line
	5400 2000 5400 3750
Wire Wire Line
	5400 3750 5550 3750
Wire Wire Line
	9050 3100 8250 3100
Wire Wire Line
	8250 3100 8250 1950
Wire Wire Line
	8250 1950 5450 1950
Wire Wire Line
	5450 1950 5450 3350
Wire Wire Line
	5450 3350 5550 3350
Wire Wire Line
	9050 3000 8300 3000
Wire Wire Line
	8300 3000 8300 1900
Wire Wire Line
	8300 1900 5500 1900
Wire Wire Line
	5500 1900 5500 3450
Wire Wire Line
	5500 3450 5550 3450
Wire Wire Line
	9050 2900 8350 2900
Wire Wire Line
	8350 2900 8350 2100
Wire Wire Line
	8350 2100 5350 2100
Wire Wire Line
	5350 2100 5350 2950
Wire Wire Line
	5350 2950 5550 2950
Wire Wire Line
	9050 2800 8400 2800
Wire Wire Line
	8400 2800 8400 2150
Wire Wire Line
	8400 2150 5250 2150
Wire Wire Line
	5250 2150 5250 3050
Wire Wire Line
	5250 3050 5550 3050
$Comp
L power:+5V #PWR035
U 1 1 61158359
P 9050 2700
F 0 "#PWR035" H 9050 2550 50  0001 C CNN
F 1 "+5V" V 9065 2828 50  0000 L CNN
F 2 "" H 9050 2700 50  0001 C CNN
F 3 "" H 9050 2700 50  0001 C CNN
	1    9050 2700
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR034
U 1 1 61158769
P 9050 2600
F 0 "#PWR034" H 9050 2350 50  0001 C CNN
F 1 "GND" V 9055 2472 50  0000 R CNN
F 2 "" H 9050 2600 50  0001 C CNN
F 3 "" H 9050 2600 50  0001 C CNN
	1    9050 2600
	0    1    1    0   
$EndComp
Text Notes 8750 2500 0    50   ~ 0
ANAG VISION AV1623 LCD
$Comp
L Device:Rotary_Encoder_Switch SW1
U 1 1 6115FE32
P 5900 1050
F 0 "SW1" H 5900 1417 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 5900 1326 50  0000 C CNN
F 2 "" H 5750 1210 50  0001 C CNN
F 3 "~" H 5900 1310 50  0001 C CNN
	1    5900 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 950  5000 950 
Wire Wire Line
	5000 2850 5550 2850
Wire Wire Line
	5000 950  5000 2850
Wire Wire Line
	5550 3250 4900 3250
Wire Wire Line
	4750 1150 5600 1150
$Comp
L power:GND #PWR025
U 1 1 6118ADDA
P 5500 1350
F 0 "#PWR025" H 5500 1100 50  0001 C CNN
F 1 "GND" H 5505 1177 50  0000 C CNN
F 2 "" H 5500 1350 50  0001 C CNN
F 3 "" H 5500 1350 50  0001 C CNN
	1    5500 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 1350 5500 1050
Wire Wire Line
	5500 1050 5600 1050
$Comp
L power:GND #PWR031
U 1 1 6118DAE1
P 6350 1350
F 0 "#PWR031" H 6350 1100 50  0001 C CNN
F 1 "GND" H 6355 1177 50  0000 C CNN
F 2 "" H 6350 1350 50  0001 C CNN
F 3 "" H 6350 1350 50  0001 C CNN
	1    6350 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 1350 6350 1150
Wire Wire Line
	6350 1150 6200 1150
Wire Wire Line
	6200 950  6250 950 
Wire Wire Line
	6250 950  6250 1650
Wire Wire Line
	6250 1650 4900 1650
Wire Wire Line
	4900 1650 4900 3250
Wire Wire Line
	4750 1150 4750 3150
Wire Wire Line
	4750 3150 5550 3150
$Comp
L power:GND #PWR026
U 1 1 61195754
P 5550 2450
F 0 "#PWR026" H 5550 2200 50  0001 C CNN
F 1 "GND" V 5555 2322 50  0000 R CNN
F 2 "" H 5550 2450 50  0001 C CNN
F 3 "" H 5550 2450 50  0001 C CNN
	1    5550 2450
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR028
U 1 1 611978A2
P 5550 4050
F 0 "#PWR028" H 5550 3800 50  0001 C CNN
F 1 "GND" V 5555 3922 50  0000 R CNN
F 2 "" H 5550 4050 50  0001 C CNN
F 3 "" H 5550 4050 50  0001 C CNN
	1    5550 4050
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR032
U 1 1 61197C40
P 7550 3950
F 0 "#PWR032" H 7550 3700 50  0001 C CNN
F 1 "GND" V 7555 3822 50  0000 R CNN
F 2 "" H 7550 3950 50  0001 C CNN
F 3 "" H 7550 3950 50  0001 C CNN
	1    7550 3950
	0    -1   -1   0   
$EndComp
$Comp
L power:+3.3V #PWR017
U 1 1 6119A8E5
P 1800 1200
F 0 "#PWR017" H 1800 1050 50  0001 C CNN
F 1 "+3.3V" H 1815 1373 50  0000 C CNN
F 2 "" H 1800 1200 50  0001 C CNN
F 3 "" H 1800 1200 50  0001 C CNN
	1    1800 1200
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR018
U 1 1 6119CDB1
P 1800 2100
F 0 "#PWR018" H 1800 1950 50  0001 C CNN
F 1 "+3.3V" H 1815 2273 50  0000 C CNN
F 2 "" H 1800 2100 50  0001 C CNN
F 3 "" H 1800 2100 50  0001 C CNN
	1    1800 2100
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR019
U 1 1 611A6014
P 1800 3000
F 0 "#PWR019" H 1800 2850 50  0001 C CNN
F 1 "+3.3V" H 1815 3173 50  0000 C CNN
F 2 "" H 1800 3000 50  0001 C CNN
F 3 "" H 1800 3000 50  0001 C CNN
	1    1800 3000
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR020
U 1 1 611A64B5
P 1800 3900
F 0 "#PWR020" H 1800 3750 50  0001 C CNN
F 1 "+3.3V" H 1815 4073 50  0000 C CNN
F 2 "" H 1800 3900 50  0001 C CNN
F 3 "" H 1800 3900 50  0001 C CNN
	1    1800 3900
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR021
U 1 1 611A7641
P 1800 4800
F 0 "#PWR021" H 1800 4650 50  0001 C CNN
F 1 "+3.3V" H 1815 4973 50  0000 C CNN
F 2 "" H 1800 4800 50  0001 C CNN
F 3 "" H 1800 4800 50  0001 C CNN
	1    1800 4800
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR022
U 1 1 611A7A19
P 1800 5700
F 0 "#PWR022" H 1800 5550 50  0001 C CNN
F 1 "+3.3V" H 1815 5873 50  0000 C CNN
F 2 "" H 1800 5700 50  0001 C CNN
F 3 "" H 1800 5700 50  0001 C CNN
	1    1800 5700
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR023
U 1 1 611A97CB
P 1800 6600
F 0 "#PWR023" H 1800 6450 50  0001 C CNN
F 1 "+3.3V" H 1815 6773 50  0000 C CNN
F 2 "" H 1800 6600 50  0001 C CNN
F 3 "" H 1800 6600 50  0001 C CNN
	1    1800 6600
	-1   0    0    1   
$EndComp
$Comp
L power:+3.3V #PWR024
U 1 1 611A9CB2
P 1800 7450
F 0 "#PWR024" H 1800 7300 50  0001 C CNN
F 1 "+3.3V" H 1815 7623 50  0000 C CNN
F 2 "" H 1800 7450 50  0001 C CNN
F 3 "" H 1800 7450 50  0001 C CNN
	1    1800 7450
	-1   0    0    1   
$EndComp
$Comp
L Switch:SW_DIP_x01 SW2
U 1 1 611BDD56
P 7350 1050
F 0 "SW2" H 7350 1317 50  0000 C CNN
F 1 "SW_DIP_x01" H 7350 1226 50  0000 C CNN
F 2 "" H 7350 1050 50  0001 C CNN
F 3 "~" H 7350 1050 50  0001 C CNN
	1    7350 1050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR033
U 1 1 611C4425
P 7850 1050
F 0 "#PWR033" H 7850 800 50  0001 C CNN
F 1 "GND" H 7855 877 50  0000 C CNN
F 2 "" H 7850 1050 50  0001 C CNN
F 3 "" H 7850 1050 50  0001 C CNN
	1    7850 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 1050 7650 1050
Wire Wire Line
	7050 1050 7050 1800
Wire Wire Line
	7050 1800 5100 1800
Wire Wire Line
	5100 1800 5100 2750
Wire Wire Line
	5100 2750 5550 2750
Text Notes 7100 700  0    50   ~ 0
FOOTSWITCH
$Comp
L Connector_Generic:Conn_01x05 J9
U 1 1 611D6F81
P 6050 6200
F 0 "J9" H 6130 6242 50  0000 L CNN
F 1 "Conn_01x05" H 6130 6151 50  0000 L CNN
F 2 "" H 6050 6200 50  0001 C CNN
F 3 "~" H 6050 6200 50  0001 C CNN
	1    6050 6200
	1    0    0    -1  
$EndComp
Text Notes 5900 5850 0    50   ~ 0
MIDI out
$Comp
L power:GND #PWR029
U 1 1 6121110F
P 5550 6100
F 0 "#PWR029" H 5550 5850 50  0001 C CNN
F 1 "GND" V 5555 5972 50  0000 R CNN
F 2 "" H 5550 6100 50  0001 C CNN
F 3 "" H 5550 6100 50  0001 C CNN
	1    5550 6100
	0    1    1    0   
$EndComp
Wire Wire Line
	5550 6100 5850 6100
$Comp
L Device:R R25
U 1 1 6121712C
P 5700 6300
F 0 "R25" V 5493 6300 50  0000 C CNN
F 1 "220" V 5584 6300 50  0000 C CNN
F 2 "" V 5630 6300 50  0001 C CNN
F 3 "~" H 5700 6300 50  0001 C CNN
	1    5700 6300
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR030
U 1 1 612179EC
P 5550 6300
F 0 "#PWR030" H 5550 6150 50  0001 C CNN
F 1 "+5V" V 5565 6428 50  0000 L CNN
F 2 "" H 5550 6300 50  0001 C CNN
F 3 "" H 5550 6300 50  0001 C CNN
	1    5550 6300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2300 7150 4500 7150
Wire Wire Line
	4500 7150 4500 5150
Wire Wire Line
	4500 5150 5550 5150
Wire Wire Line
	2300 6300 4400 6300
Wire Wire Line
	4400 6300 4400 5050
Wire Wire Line
	4400 5050 5550 5050
Wire Wire Line
	2300 5400 4300 5400
Wire Wire Line
	4300 5400 4300 4950
Wire Wire Line
	4300 4950 5550 4950
Wire Wire Line
	2300 4500 4300 4500
Wire Wire Line
	4300 4500 4300 4850
Wire Wire Line
	4300 4850 5550 4850
Wire Wire Line
	2300 3600 4400 3600
Wire Wire Line
	4400 3600 4400 4750
Wire Wire Line
	4400 4750 5550 4750
Wire Wire Line
	2300 2700 4500 2700
Wire Wire Line
	4500 2700 4500 4650
Wire Wire Line
	4500 4650 5550 4650
Wire Wire Line
	2300 1800 4600 1800
Wire Wire Line
	4600 1800 4600 4550
Wire Wire Line
	4600 4550 5550 4550
Wire Wire Line
	2300 900  4700 900 
Wire Wire Line
	4700 900  4700 4450
Wire Wire Line
	4700 4450 5550 4450
Wire Wire Line
	5850 6400 4850 6400
Wire Wire Line
	4850 6400 4850 3550
Wire Wire Line
	4850 3550 5550 3550
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 61245A01
P 10400 6150
F 0 "#FLG0101" H 10400 6225 50  0001 C CNN
F 1 "PWR_FLAG" V 10400 6278 50  0000 L CNN
F 2 "" H 10400 6150 50  0001 C CNN
F 3 "~" H 10400 6150 50  0001 C CNN
	1    10400 6150
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR0101
U 1 1 61248128
P 10400 6150
F 0 "#PWR0101" H 10400 6000 50  0001 C CNN
F 1 "+5V" V 10415 6278 50  0000 L CNN
F 2 "" H 10400 6150 50  0001 C CNN
F 3 "" H 10400 6150 50  0001 C CNN
	1    10400 6150
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x16_Male J10
U 1 1 61141D0E
P 9250 3400
F 0 "J10" H 9222 3282 50  0000 R CNN
F 1 "Conn_01x16_Male" H 9222 3373 50  0000 R CNN
F 2 "" H 9250 3400 50  0001 C CNN
F 3 "~" H 9250 3400 50  0001 C CNN
	1    9250 3400
	-1   0    0    1   
$EndComp
NoConn ~ 9050 3200
NoConn ~ 9050 3300
NoConn ~ 9050 3400
NoConn ~ 9050 3500
NoConn ~ 9050 3600
NoConn ~ 9050 3700
NoConn ~ 5850 6000
NoConn ~ 5850 6200
NoConn ~ 7550 3050
NoConn ~ 7550 3150
NoConn ~ 7550 3250
NoConn ~ 7550 3350
NoConn ~ 7550 3450
NoConn ~ 7550 3550
NoConn ~ 7550 3650
NoConn ~ 7550 3750
NoConn ~ 7550 3850
NoConn ~ 7550 4050
NoConn ~ 7550 4350
NoConn ~ 7550 4450
NoConn ~ 7550 4550
NoConn ~ 7550 4650
NoConn ~ 7550 4850
NoConn ~ 7550 5050
NoConn ~ 7550 5150
NoConn ~ 7550 2950
NoConn ~ 7550 2750
NoConn ~ 7550 2650
NoConn ~ 7550 2550
NoConn ~ 7550 2450
NoConn ~ 5550 2650
NoConn ~ 5550 2550
NoConn ~ 5550 3650
NoConn ~ 5550 3850
NoConn ~ 5550 4150
NoConn ~ 5550 4250
NoConn ~ 5550 4350
NoConn ~ 7550 4950
$Comp
L power:+5V #PWR0102
U 1 1 612F9114
P 7550 4750
F 0 "#PWR0102" H 7550 4600 50  0001 C CNN
F 1 "+5V" V 7565 4878 50  0000 L CNN
F 2 "" H 7550 4750 50  0001 C CNN
F 3 "" H 7550 4750 50  0001 C CNN
	1    7550 4750
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR08
U 1 1 610DA279
P 1400 3900
F 0 "#PWR08" H 1400 3650 50  0001 C CNN
F 1 "GND" H 1405 3727 50  0000 C CNN
F 2 "" H 1400 3900 50  0001 C CNN
F 3 "" H 1400 3900 50  0001 C CNN
	1    1400 3900
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 610A00BC
P 10400 6300
F 0 "#FLG0102" H 10400 6375 50  0001 C CNN
F 1 "PWR_FLAG" V 10400 6428 50  0000 L CNN
F 2 "" H 10400 6300 50  0001 C CNN
F 3 "~" H 10400 6300 50  0001 C CNN
	1    10400 6300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 610A036E
P 10400 6300
F 0 "#PWR0103" H 10400 6050 50  0001 C CNN
F 1 "GND" V 10405 6172 50  0000 R CNN
F 2 "" H 10400 6300 50  0001 C CNN
F 3 "" H 10400 6300 50  0001 C CNN
	1    10400 6300
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR0105
U 1 1 610A2B97
P 5550 3950
F 0 "#PWR0105" H 5550 3800 50  0001 C CNN
F 1 "+3.3V" V 5565 4078 50  0000 L CNN
F 2 "" H 5550 3950 50  0001 C CNN
F 3 "" H 5550 3950 50  0001 C CNN
	1    5550 3950
	0    -1   -1   0   
$EndComp
NoConn ~ 7550 2850
$Comp
L Connector:Conn_01x02_Male J1
U 1 1 610BABC8
P 900 800
F 0 "J1" H 1008 981 50  0000 C CNN
F 1 "Conn_01x02_Male" H 1008 890 50  0000 C CNN
F 2 "" H 900 800 50  0001 C CNN
F 3 "~" H 900 800 50  0001 C CNN
	1    900  800 
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J2
U 1 1 610BAF58
P 900 1700
F 0 "J2" H 1008 1881 50  0000 C CNN
F 1 "Conn_01x02_Male" H 1008 1790 50  0000 C CNN
F 2 "" H 900 1700 50  0001 C CNN
F 3 "~" H 900 1700 50  0001 C CNN
	1    900  1700
	1    0    0    -1  
$EndComp
Text Notes 600  1850 1    50   ~ 0
PIN CONNECTORS TO PIEZO/JACK
$Comp
L Connector:Conn_01x02_Male J3
U 1 1 610BE2A0
P 900 2600
F 0 "J3" H 1008 2781 50  0000 C CNN
F 1 "Conn_01x02_Male" H 1008 2690 50  0000 C CNN
F 2 "" H 900 2600 50  0001 C CNN
F 3 "~" H 900 2600 50  0001 C CNN
	1    900  2600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J4
U 1 1 610BE8AE
P 900 3500
F 0 "J4" H 1008 3681 50  0000 C CNN
F 1 "Conn_01x02_Male" H 1008 3590 50  0000 C CNN
F 2 "" H 900 3500 50  0001 C CNN
F 3 "~" H 900 3500 50  0001 C CNN
	1    900  3500
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J5
U 1 1 610BF007
P 900 4400
F 0 "J5" H 1008 4581 50  0000 C CNN
F 1 "Conn_01x02_Male" H 1008 4490 50  0000 C CNN
F 2 "" H 900 4400 50  0001 C CNN
F 3 "~" H 900 4400 50  0001 C CNN
	1    900  4400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J6
U 1 1 610C063D
P 900 5300
F 0 "J6" H 1008 5481 50  0000 C CNN
F 1 "Conn_01x02_Male" H 1008 5390 50  0000 C CNN
F 2 "" H 900 5300 50  0001 C CNN
F 3 "~" H 900 5300 50  0001 C CNN
	1    900  5300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J7
U 1 1 610C0D4C
P 900 6200
F 0 "J7" H 1008 6381 50  0000 C CNN
F 1 "Conn_01x02_Male" H 1008 6290 50  0000 C CNN
F 2 "" H 900 6200 50  0001 C CNN
F 3 "~" H 900 6200 50  0001 C CNN
	1    900  6200
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J8
U 1 1 610C1B14
P 900 7050
F 0 "J8" H 1008 7231 50  0000 C CNN
F 1 "Conn_01x02_Male" H 1008 7140 50  0000 C CNN
F 2 "" H 900 7050 50  0001 C CNN
F 3 "~" H 900 7050 50  0001 C CNN
	1    900  7050
	1    0    0    -1  
$EndComp
Text Notes 650  7150 0    50   ~ 0
Sleeve\nTip
Text Notes 650  6300 0    50   ~ 0
Sleeve\nTip
Text Notes 650  5400 0    50   ~ 0
Sleeve\nTip
Text Notes 650  4500 0    50   ~ 0
Sleeve\nTip
Text Notes 650  3600 0    50   ~ 0
Sleeve\nTip
Text Notes 650  2700 0    50   ~ 0
Sleeve\nTip
Text Notes 650  1800 0    50   ~ 0
Sleeve\nTip
Text Notes 650  900  0    50   ~ 0
Sleeve\nTip
Text Notes 7350 7500 0    50   ~ 0
SUPERMUSCLE
Text Notes 8150 7650 0    50   ~ 0
2021-08-02
$EndSCHEMATC
