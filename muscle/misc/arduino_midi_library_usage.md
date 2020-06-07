# Arduino_MIDI_library usage

``` C
/*! Enumeration of MIDI types */
enum MidiType
{
    InvalidType           = 0x00,    ///< For notifying errors
    NoteOff               = 0x80,    ///< Note Off
    NoteOn                = 0x90,    ///< Note On
    AfterTouchPoly        = 0xA0,    ///< Polyphonic AfterTouch
    ControlChange         = 0xB0,    ///< Control Change / Channel Mode
    ProgramChange         = 0xC0,    ///< Program Change
    AfterTouchChannel     = 0xD0,    ///< Channel (monophonic) AfterTouch
    PitchBend             = 0xE0,    ///< Pitch Bend
    SystemExclusive       = 0xF0,    ///< System Exclusive
    TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
    SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
    SongSelect            = 0xF3,    ///< System Common - Song Select
    TuneRequest           = 0xF6,    ///< System Common - Tune Request
    Clock                 = 0xF8,    ///< System Real Time - Timing Clock
    Start                 = 0xFA,    ///< System Real Time - Start
    Continue              = 0xFB,    ///< System Real Time - Continue
    Stop                  = 0xFC,    ///< System Real Time - Stop
    ActiveSensing         = 0xFE,    ///< System Real Time - Active Sensing
    SystemReset           = 0xFF,    ///< System Real Time - System Reset
};
```


``` C
/*! \brief Create an instance of the library attached to a serial port.
 You can use HardwareSerial or SoftwareSerial for the serial port.
 Example: MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi2);
 Then call midi2.begin(), midi2.read() etc..
 */
#define MIDI_CREATE_INSTANCE(Type, SerialPort, Name)  \
    MIDI_NAMESPACE::SerialMIDI<Type> serial##Name(SerialPort);\
    MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<Type>> Name((MIDI_NAMESPACE::SerialMIDI<Type>&)serial##Name);

#if defined(ARDUINO_SAM_DUE) || defined(USBCON) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
    // Leonardo, Due and other USB boards use Serial1 by default.
    #define MIDI_CREATE_DEFAULT_INSTANCE()                                      \
        MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#else
    /*! \brief Create an instance of the library with default name, serial port
    and settings, for compatibility with sketches written with pre-v4.2 MIDI Lib,
    or if you don't bother using custom names, serial port or settings.
    */
    #define MIDI_CREATE_DEFAULT_INSTANCE()                                      \
        MIDI_CREATE_INSTANCE(HardwareSerial, Serial,  MIDI);
#endif
```
