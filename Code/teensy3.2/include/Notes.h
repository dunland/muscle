#pragma once

enum Notes{
  Note_C1 = 0,
  Note_Db1 = 1,
  Note_D1 = 2,
  Note_Eb1 = 3,
  Note_E1 = 4,
  Note_F1 = 5,
  Note_Gb1 = 6,
  Note_G1 = 7,
  Note_Ab1 = 8,
  Note_A1 = 9,
  Note_Bb1 = 10,
  Note_B1 = 11,

  Note_C2 = 12,
  Note_Db2 = 13,
  Note_D2 = 14,
  Note_Eb2 = 15,
  Note_E2 = 16,
  Note_F2 = 17,
  Note_Gb2 = 18,
  Note_G2 = 19,
  Note_Ab2 = 20,
  Note_A2 = 21,
  Note_Bb2 = 22,
  Note_B2 = 23,

  Note_C3 = 24,
  Note_Db3 = 25,
  Note_D3 = 26,
  Note_Eb3 = 27,
  Note_E3 = 28,
  Note_F3 = 29,
  Note_Gb3 = 30,
  Note_G3 = 31,
  Note_Ab3 = 32,
  Note_A3 = 33,
  Note_Bb3 = 34,
  Note_B3 = 35,

  Note_C4 = 36,
  Note_Db4 = 37,
  Note_D4 = 38,
  Note_Eb4 = 39,
  Note_E4 = 40,
  Note_F4 = 41,
  Note_Gb4 = 42,
  Note_G4 = 43,
  Note_Ab4 = 44,
  Note_A4 = 45,
  Note_Bb4 = 46,
  Note_B4 = 47,

  Note_C5 = 48,
  Note_Db5 = 49,
  Note_D5 = 50,
  Note_Eb5 = 51,
  Note_E5 = 52,
  Note_F5 = 53,
  Note_Gb5 = 54,
  Note_G5 = 55,
  Note_Ab5 = 56,
  Note_A5 = 57,
  Note_Bb5 = 58,
  Note_B5 = 59,

  Note_C6 = 60,
  Note_Db6 = 61,
  Note_D6 = 62,
  Note_Eb6 = 63,
  Note_E6 = 64,
  Note_F6 = 65,
  Note_Gb6 = 66,
  Note_G6 = 67,
  Note_Ab6 = 68,
  Note_A6 = 69,
  Note_Bb6 = 70,
  Note_B6 = 71,

  Note_C7 = 72,
  Note_Db7 = 73,
  Note_D7 = 74,
  Note_Eb7 = 75,
  Note_E7 = 77,
  Note_F7 = 77,
  Note_Gb7 = 78,
  Note_G7 = 79,
  Note_Ab7 = 80,
  Note_A7 = 81,
  Note_Bb7 = 82,
  Note_B7 = 83,

  Note_C8 = 84,
  Note_Db8 = 85,
  Note_D8 = 86,
  Note_Eb8 = 87,
  Note_E8 = 88,
  Note_F8 = 89,
  Note_Gb8 = 90,
  Note_G8 = 91,
  Note_Ab8 = 92,
  Note_A8 = 93,
  Note_Bb8 = 94,
  Note_B8 = 95,

  Note_C9 = 96,
  Note_Db9 = 97,
  Note_D9 = 98,
  Note_Eb9 = 99,
  Note_E9 = 100,
  Note_F9 = 101,
  Note_Gb9 = 102,
  Note_G9 = 103,
  Note_Ab9 = 104,
  Note_A9 = 105,
  Note_Bb9 = 106,
  Note_B9 = 107,

  Note_C10 = 108,
  Note_Db10 = 109,
  Note_D10 = 110,
  Note_Eb10 = 111,
  Note_E10 = 112,
  Note_F10 = 113,
  Note_Gb10 = 114,
  Note_G10 = 115,
  Note_Ab10 = 116,
  Note_A10 = 117,
  Note_Bb10 = 118,
  Note_B10 = 119,

  Note_C11 = 120,
  Note_Db11 = 121,
  Note_D11 = 122,
  Note_Eb11 = 123,
  Note_E11 = 124,
  Note_F11 = 125,
  Note_Gb11 = 126,
  Note_G11 = 127
};

/*

MIDI Note Number to Frequency Conversion Chart

 MIDI                   MIDI                   MIDI
 Note     Frequency      Note   Frequency       Note   Frequency
 C1  0    8.1757989156    12    16.3515978313    24    32.7031956626
 Db  1    8.6619572180    13    17.3239144361    25    34.6478288721
 D   2    9.1770239974    14    18.3540479948    26    36.7080959897
 Eb  3    9.7227182413    15    19.4454364826    27    38.8908729653
 E   4   10.3008611535    16    20.6017223071    28    41.2034446141
 F   5   10.9133822323    17    21.8267644646    29    43.6535289291
 Gb  6   11.5623257097    18    23.1246514195    30    46.2493028390
 G   7   12.2498573744    19    24.4997147489    31    48.9994294977
 Ab  8   12.9782717994    20    25.9565435987    32    51.9130871975
 A   9   13.7500000000    21    27.5000000000    33    55.0000000000
 Bb  10  14.5676175474    22    29.1352350949    34    58.2704701898
 B   11  15.4338531643    23    30.8677063285    35    61.7354126570

 C4  36  65.4063913251    48   130.8127826503    60   261.6255653006
 Db  37  69.2956577442    49   138.5913154884    61   277.1826309769
 D   38  73.4161919794    50   146.8323839587    62   293.6647679174
 Eb  39  77.7817459305    51   155.5634918610    63   311.1269837221
 E   40  82.4068892282    52   164.8137784564    64   329.6275569129
 F   41  87.3070578583    53   174.6141157165    65   349.2282314330
 Gb  42  92.4986056779    54   184.9972113558    66   369.9944227116
 G   43  97.9988589954    55   195.9977179909    67   391.9954359817
 Ab  44  103.8261743950   56   207.6523487900    68   415.3046975799
 A   45  110.0000000000   57   220.0000000000    69   440.0000000000
 Bb  46  116.5409403795   58   233.0818807590    70   466.1637615181
 B   47  123.4708253140   59   246.9416506281    71   493.8833012561

 C7  72  523.2511306012   84  1046.5022612024    96  2093.0045224048
 Db  73  554.3652619537   85  1108.7305239075    97  2217.4610478150
 D   74  587.3295358348   86  1174.6590716696    98  2349.3181433393
 Eb  75  622.2539674442   87  1244.5079348883    99  2489.0158697766
 E   76  659.2551138257   88  1318.5102276515   100  2637.0204553030
 F   77  698.4564628660   89  1396.9129257320   101  2793.8258514640
 Gb  78  739.9888454233   90  1479.9776908465   102  2959.9553816931
 G   79  783.9908719635   91  1567.9817439270   103  3135.9634878540
 Ab  80  830.6093951599   92  1661.2187903198   104  3322.4375806396
 A   81  880.0000000000   93  1760.0000000000   105  3520.0000000000
 Bb  82  932.3275230362   94  1864.6550460724   106  3729.3100921447
 B   83  987.7666025122   95  1975.5332050245   107  3951.0664100490

 C10 108 4186.0090448096  120  8372.0180896192
 Db  109 4434.9220956300  121  8869.8441912599
 D   110 4698.6362866785  122  9397.2725733570
 Eb  111 4978.0317395533  123  9956.0634791066
 E   112 5274.0409106059  124 10548.0818212118
 F   113 5587.6517029281  125 11175.3034058561
 Gb  114 5919.9107633862  126 11839.8215267723
 G   115 6271.9269757080  127 12543.8539514160
 Ab  116 6644.8751612791
 A   117 7040.0000000000
 Bb  118 7458.6201842894
 B   119 7902.1328200980

NOTES: Middle C is note #60. Frequency is in Hertz.

Here is C code to calculate an array with all of the above frequencies (ie, so that midi[0], which is midi note #0, is assigned the value of 8.1757989156). Tuning is based upon A=440.

float midi[127];
int a = 440; // a is 440 hz...
for (int x = 0; x < 127; ++x)
{
   midi[x] = (a / 32) * (2 ^ ((x - 9) / 12));
}

*/