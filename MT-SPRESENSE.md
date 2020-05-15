# SONY SPRESENSE log
Hier kommt der Fortschritt mit allen Hindernissen und Lösungen rein bei der Arbeit mit dem SPRESENSE Microcontroller rein.

## examples

###player_wav.ino:

"Sound.wav" auf SD-Karte  
WAVDEC von [github](https://github.com/sonydevworld/spresense-arduino-compatible/tree/master/Arduino15/packages/SPRESENSE/hardware/spresense/1.0.0/libraries/Audio/examples/dsp_installer/wav_dec_installer) geladen und in Ordner "BIN" auf SD-Karte gelegt.

oder: wav_dec_installer.ino ausführen.. :b

### connecting mics:
![Connecting Mics to JP10](img/SPRESENSE_connectingMics.png)  

Funktioniert mit Electret Mic: `SPRESENSE_audioPass.ino`  
in 3.3V Mode
Mic A:  
PIN 1: Mic + --> direct  
PIN 2: Mic + --> 2.2kOhm  
PIN 3: Mic -  


### Line In:
3.3V Mode
iPod in
PIN 1: Line + 
PIN 2: ---
PIN 3: GND

---

2019-11-13  

### audio setup

#### init Recorder

https://developer.sony.com/develop/spresense/developer-tools/api-reference/api-references-spresense-sdk/group__audioutils__audio__high__level__api.html#ga6371c7e9a02e618ed19de38534856e26

max bitrate = 510000 (wahrscheinlich kByte)  
min bitlength = 16  
--> 192000 samples/s * 16 bit/s = 3072000 bit/s = 3 Mbit/s  
--> 3Mbit/s / 8 = 384kByte/s  

RAM hat 1,5 MB --> 1,5MB/384kB = 3,906s Aufnahmezeit in FIFO

---

2019-11-20  
Audio.cpp 
+-- [AudioClass::writeFrames ](https://github.com/sonydevworld/spresense-arduino-compatible/blob/master/Arduino15/packages/SPRESENSE/hardware/spresense/1.0.0/libraries/Audio/Audio.cpp#L912)
	+-- `CMN_simpleFifoHandle *handle --> m_player0_handle`
		+-- [Struct Reference from Memory Utils](https://developer.sony.com/develop/spresense/developer-tools/api-reference/api-references-spresense-sdk/structCMN__SimpleFifoHandle.html#details)
		+-- what is `m_player0_handle` ??
		+-- 

---

2019-11-21  
hilfreiche Suchmaschineneingabe: `Improved phase vocoder time-scale modification of audio` 
		
		
