# Log folder
this folder holds logs that are recorded from contact microphone streams while playing the instruments

- **command**: `socat stdio /dev/ttyUSB0 >> file.txt` writes stream of ttyUSB0 to file.txt (`>>` appends data)  

## log overview

file 				| description
--------------------------------|----------------
snaredrum_log20200516.txt	| Snaredrum dynamisch gespielt, teilweise ganzes Set gespielt; Daten normalisiert auf abs(260 - analogRead(A0)); Spieldauer etwa 1 Min? 
