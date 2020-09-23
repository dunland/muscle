#include <Tsunami.h>

#define LED 13

Tsunami tsunami;
char gTsunamiVersion[VERSION_STRING_LEN]; // Tsunami version string

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    pinMode(LED, OUTPUT);

    // We should wait for the Tsunami to finish reset before trying to send
    // commands.
    delay(1000);

    // Tsunami startup at 57600
    tsunami.start();
    delay(100);

    // Send a stop-all command and reset the sample-rate offset, in case we have
    //  reset while the Tsunami was already playing.
    tsunami.stopAllTracks();
    tsunami.samplerateOffset(0, 0);

    // Enable track reporting from the Tsunami
    // - this function enables (TRUE) or disables (FALSE) track reporting. When enabled, the Tsunami will send a message whenever a track starts or ends, specifying the track number. Provided you call update() periodically, the library will use these messages to maintain status of all tracks, allowing you to query if particular tracks are playing or not.
    tsunami.setReporting(true);

    // Allow time for the Tsunami to respond with the version string and
    //  number of tracks.
    delay(100);
}

void loop()
{

    // the range for t (track number) is 1 through 4096, the range for out (output number) is 1 through 4 for the stereo firmware, and 1 through 8 for the mono firmware.

    static int state = 0;

    // Call update on the Tsunami to keep the track playing status current.
    //  - this function should be called periodically when reporting is enabled. Doing so will process any incoming serial messages and keep the track status up to date.
    tsunami.update(); 

    switch (state)
    {
    case 0:
        // First retrieve and print the version and number of tracks
        if (tsunami.getVersion(gTsunamiVersion, VERSION_STRING_LEN))
        {
            Serial.print(gTsunamiVersion);
            Serial.print("\n");
            gNumTracks = tsunami.getNumTracks();
            Serial.print("Number of tracks = ");
            Serial.print(gNumTracks);
            Serial.print("\n");

        }
        else
        {
            Serial.print("WAV Trigger response not available");
            tsunami.samplerateOffset(0, 0); // Reset sample rate offset to 0
            tsunami.masterGain(0, 0);       // Reset the master gain to 0dB
            tsunami.trackGain(2, -40);            // Preset Track 2 gain to -40dB
            tsunami.trackPlayPoly(2, 0, true);    // Start Track 2
            tsunami.trackFade(2, 0, 2000, false); // Fade Track 2 to 0dB over 2 sec
        }
        state++;
        break;

    case 1:

    default:
        break;
    }
}