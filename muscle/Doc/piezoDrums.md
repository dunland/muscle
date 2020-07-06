# piezo Drums

## Recherche

[Arduino Tutorial: Knock - Circuit](https://www.arduino.cc/en/Tutorial/Knock)

[Several Piezo contact mics with Arduino (StackOverflow)](https://arduino.stackexchange.com/questions/32793/several-piezo-contact-mics-with-arduino)


[Connecting Arduino to Processing](https://learn.sparkfun.com/tutorials/connecting-arduino-to-processing/all)

---
## Schaltung

![Piezo digital signal](PiezoDigitalSignal.png)

<iframe width="560" height="315" src="https://www.youtube.com/embed/KYewU1H2dOw" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

![big piezo signal](20200303signalBigPiezo.jpg)  
das Signal vom großen Piezo, wenn man direkt drauf rum drückt

![small piezo signal](20200303signalSmallPiezo.jpg)  
hier vom kleinen piezo. tut sich nicht viel! der unterschied ist glaube ich in den übertragenen frequenzbändern.

![small piezo values](20200303smallPiezoValues.jpg)  
sind jedenfalls genau die gleichen werte, die rauskommen. das liegt an der schaltung. klappt gut!

---

## Software

### 0. Serial Port Communication in Ubuntu
- `socat stdio /dev/ttyUSB0` liest wiederholt Serial Output vom MCU aus
- [weitere Möglichkeiten der Anzeige von Serial Streams in Ubuntu](https://www.cyberciti.biz/hardware/5-linux-unix-commands-for-connecting-to-the-serial-console/)

### 1. Verbindung von Arduino und PureData:

#### mit `[comport]`:
1. installiere comport über den externals-finder
2. 

#### mit [arduino] (hat nicht funktioniert!):
1. download [pduino](http://write.flossmanuals.net/pure-data/installing-pduino/)
2. extract to pd-path for externals: `/home/username/lib/pd/extra/`
3. create `[arduino]` object
4. problems? --> install zexy, mapping, cyclone, moocow, comport via external finder (help->find externals)

#### Puredata:
1. erstelle `[arduino]` objekt mit port Nummer des MCUs. (list serial ports by sending "devices" to `[arduino]`. Ubuntu Serial Port List: `ls /sys/class/tty`)
2. ... Errors!


## up next:
(2020-03-03)
- klangliche tests machen an echten drums (ohne voltage divider!)
- diese schaltung als trigger für irgendwas benutzen
- externe aktuatoren verwenden für xylophon usw
