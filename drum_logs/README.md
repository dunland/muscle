# Log folder
this folder holds logs that are recorded from contact microphone streams while playing the instruments

- **command**: `socat stdio /dev/ttyUSB0 >> file.txt` writes stream of ttyUSB0 to file.txt (`>>` appends data)  

## log overview

file 				| description
--------------------------------|----------------
snaredrum_log20200516.txt	| Snaredrum dynamisch gespielt, teilweise ganzes Set gespielt; Daten normalisiert auf abs(260 - analogRead(A0)); Spieldauer etwa 1 Min? 


## Record of Instrument Signatures for characteristic Hit Detection Timing

![Snaredrum mit Besen, aufgenommen mit SM58](snaredrum_besen_SM58_20200607.png)  
Snaredrum mit Besen gespielt, aufgenommen mit SM58

![Snaredrum mit Stick, aufgenommen mit SM58](snaredrum_stick_SM58_20200607.png)  
Snaredrum-Schlag mit Stick, aufgenommen mit SM58

Single Stroke	| Duration	|significant zero-crossings
----------------|---------------|----------------------------
Brush		| ~30 ms	|~8
Stick		| ~70 ms	|~20

### 1. counting amount of samples that can be taken within 1 ms:

``` C++
while (micros() < startMeasuring + 1000) // 1 ms of sampling time
{
  for (int pinNum = 0; pinNum < numInputs; pinNum++)
  {
    vals[pinNum][totalLoops] = pinValue(pinNum);
    totalMeasurements++;
  }
  totalLoops++;
}
```

(from teensyDrums00_Debug_pinReads_loop.ino)

results in:

```
96 readings taken in 1000 microseconds. Values:

39 47 84 36 58 50 3 40 16 24 34 48 

75 2 45 21 129 40 49 13 6 23 40 31 

17 35 28 44 127 18 36 40 29 19 65 48 

16 23 19 66 23 42 41 1 27 2 0 4 

1 37 22 72 46 68 4 11 6 10 65 54 

41 17 63 80 35 0 2 17 10 30 28 5 

55 95 5 13 4 19 6 56 5 13 13 37 

59 21 49 77 17 0 52 23 50 23 2 35 

```

### 2. taking samples at 1 s/ms

**count threshold crosses with contact mics at a rate of 1 sample / ms:**

Instrument	| Duration	| significant zero-crossings
----------------|---------------|----------------------------
Snare		|		|
Tom 1		|		|
Tom 2		|		|
Standtom	|		|
Hihat		|		|
Ride		|		|
Crash 1		|		|
Crash 2		|		|


### Multi-sample-taking

use 4-8 piezos at one instrument at the same time to create a multidimensional multi-variant chart to plot:  
x = time  
y = piezos 1 - 8  

**1. take sensor values every 1 ms and plot directly**  
**2. plot only crossings taken**  
and vary:  
sensor threshold -> will result in different counts of zero-crossings  
release time  

plot:  
millis	pinNum	crossings

### evaluate timing precision for multiple pins
...but how?
