
#include <SDHCI.h>
#include <Audio.h>

// ------------------------------ from rec_play
#define RecordLoopNum 800

enum
{
        PlayReady = 0,
        Playing,
        RecordReady,
        Recording
};
// ---------------------------------------------

SDClass theSD;
AudioClass *theAudio;

File kickFile, snare, hihat, tom;

WavContainerFormatParser theParser;

const int32_t sc_buffer_size = 6144;
uint8_t s_buffer[sc_buffer_size];

uint32_t s_remain_size = 0;
bool ErrEnd = false;

// ------------------------- custom contactMic variables -----------------------

const int noiseFloor = 260; // TODO: create function to assess noiseFloor upon startup
const int max_val = 762;
const int globalThreshold = 32;

static const uint8_t pins[] = {A0, A1, A2, A3};
static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3};
uint8_t vals[4];
static const uint8_t inputAmount = 3; // that makes 4 input since number 0 is counted as well

/**
   @brief Audio attention callback

   When audio internal error occurc, this function will be called back.
 */

/* -------------------------- AUDIO CALLBACK FUNCTION ----------------------- */
static void audio_attention_cb(const ErrorAttentionParam *atprm)
{
        puts("Attention!");

        if (atprm->error_code >= AS_ATTENTION_CODE_WARNING)
        {
                ErrEnd = true;
        }
}

/**
   @brief Setup audio player to play wav file

   Set clock mode to normal <br>
   Set output device to speaker <br>
   Set main player to decode stereo wav. Stream sample rate is auto detect. <br>
   System directory "/mnt/sd0/BIN" will be searched for WAV decoder (WAVDEC file)
   Open "Sound.wav" file <br>
   Set master volume to -16.0 dB
 */

static const uint32_t sc_prestore_frames = 10;

void setup() {
        Serial.begin(19200);
        theSD.begin();

        // set all required pins
        for (int i = 0; i < inputAmount; i++)
        {
                pinMode(leds[i], OUTPUT);
        }

        initializePlayer();
        // load_kickFile();

}

void initializePlayer()
{
  // Get wav file info

  fmt_chunk_t fmt;

  handel_wav_parser_t *handle = (handel_wav_parser_t *)theParser.parseChunk("/mnt/sd0/samples/Alesis_HR16A_04kick.wav", &fmt);
  if (handle == NULL)
  {
          printf("Wav parser error.\n");
          exit(1);
  }

  s_remain_size = handle->data_size + sizeof(WAVHEADER);

  theParser.resetParser((handel_wav_parser *)handle); // closes wave file and frees internal memory area

// start audio system
  theAudio = AudioClass::getInstance();

  theAudio->begin(audio_attention_cb);

  puts("initialization Audio Library");

/* Set clock mode to normal */

  theAudio->setRenderingClockMode((fmt.rate <= 48000) ? AS_CLKMODE_NORMAL : AS_CLKMODE_HIRES);

// ----------------------------- preparing playback ----------------------------
/* Set output device to speaker with first argument. If you want to change the output device to I2S, specify "AS_SETPLAYER_OUTPUTDEVICE_I2SOUTPUT" as an argument. Set speaker driver mode to LineOut with second argument. If you want to change the speaker driver mode to other, specify "AS_SP_DRV_MODE_1DRIVER" or "AS_SP_DRV_MODE_2DRIVER" or "AS_SP_DRV_MODE_4DRIVER" as an argument.

Set Audio Library Mode to Music Player. This function switches the mode of the Audio library to Music Player. This function cannot be called after transition to "Music Player mode". To return to the original state, please call setReadyMode (). In this function, setting HW necessary for music playback, and setting ES buffer configuration etc.
*/
  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP, AS_SP_DRV_MODE_LINEOUT); // sets Audio library Mode to Music Player

/*
Set main player to decode wav. Initialize parameters are taken from wav header.
Search for WAV decoder in "/mnt/sd0/BIN" directory
*/
  err_t err = theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_WAV, "/mnt/sd0/BIN", fmt.rate, fmt.bit, fmt.channel);

/* Verify player initialize */
  if (err != AUDIOLIB_ECODE_OK)
  {
          printf("Player0 initialize error\n");
          exit(1);
  }

/* Open file placed on SD card */
  kickFile = theSD.open("samples/Alesis_HR16A_04kick.wav");

/* Verify file open */
  if (!kickFile)
  {
          printf("File open error\n");
          exit(1);
  }
  printf("Open! %d\n", kickFile);

// -------------------------- write stream data to buffer
  // for (uint32_t i = 0; i < sc_prestore_frames; i++)
  // {
  //         size_t supply_size = kickFile.read(s_buffer, sizeof(s_buffer));
  //         s_remain_size -= supply_size;
  //
  //         err = theAudio->writeFrames(AudioClass::Player0, s_buffer, supply_size);
  //         if (err != AUDIOLIB_ECODE_OK)
  //         {
  //                 break;
  //         }
  //
  //         if (s_remain_size == 0)
  //         {
  //                 break;
  //         }
  // }

/* Main volume set to -16.0 dB */

  theAudio->setVolume(-160);


}

void load_kickFile()
{
        // Get wav file info

        fmt_chunk_t fmt;

        handel_wav_parser_t *handle = (handel_wav_parser_t *)theParser.parseChunk("/mnt/sd0/samples/Alesis_HR16A_04kick.wav", &fmt);
        if (handle == NULL)
        {
                printf("Wav parser error.\n");
                exit(1);
        }

        s_remain_size = handle->data_size + sizeof(WAVHEADER);

        theParser.resetParser((handel_wav_parser *)handle); // closes wave file and frees internal memory area

// start audio system
        // theAudio = AudioClass::getInstance();
        //
        // theAudio->begin(audio_attention_cb);
        //
        // puts("initialization Audio Library");

/* Set clock mode to normal */

        // theAudio->setRenderingClockMode((fmt.rate <= 48000) ? AS_CLKMODE_NORMAL : AS_CLKMODE_HIRES);

// ----------------------------- preparing playback ----------------------------
/* Set output device to speaker with first argument. If you want to change the output device to I2S, specify "AS_SETPLAYER_OUTPUTDEVICE_I2SOUTPUT" as an argument. Set speaker driver mode to LineOut with second argument. If you want to change the speaker driver mode to other, specify "AS_SP_DRV_MODE_1DRIVER" or "AS_SP_DRV_MODE_2DRIVER" or "AS_SP_DRV_MODE_4DRIVER" as an argument.

   Set Audio Library Mode to Music Player. This function switches the mode of the Audio library to Music Player. This function cannot be called after transition to "Music Player mode". To return to the original state, please call setReadyMode (). In this function, setting HW necessary for music playback, and setting ES buffer configuration etc.
 */
        // theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP, AS_SP_DRV_MODE_LINEOUT); // sets Audio library Mode to Music Player

/*
   Set main player to decode wav. Initialize parameters are taken from wav header.
   Search for WAV decoder in "/mnt/sd0/BIN" directory
 */
      err_t err = theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_WAV, "/mnt/sd0/BIN", fmt.rate, fmt.bit, fmt.channel);

/* Verify player initialize */
        // if (err != AUDIOLIB_ECODE_OK)
        // {
        //         printf("Player0 initialize error\n");
        //         exit(1);
        // }

/* Open file placed on SD card */
        kickFile = theSD.open("samples/Alesis_HR16A_04kick.wav");

/* Verify file open */
        if (!kickFile)
        {
                printf("File open error\n");
                exit(1);
        }
        printf("Open! %d\n", kickFile);

/* Main volume set to -16.0 dB */

        theAudio->setVolume(-160);
}

/**
   @brief Play stream

   Send new frames to decode in a loop until file ends
 */

static const uint32_t sc_store_frames = 10;


/* -------------------------------------------------------------------------- */
void loop() {
  puts("looping! ");
        // Serial.println(analogRead(A0));

        /* Serial.write anstatt print, da comport nur 8-bit codiert ist!
            in PD kommt es dann zum overflow und Werte um ~10 werden ausgegeben
            Serial.print gibt Werte in 10-bit aus
         */
        /*
           Serial.write(analogRead(A0));
           Serial.write(analogRead(A1));
           Serial.write(analogRead(A2));
           Serial.write(analogRead(A3));
         */


        // --------------------------------- read all input pins
        for (int i = 0; i < 3; i++)
        {
                vals[i] = noiseFloor - analogRead(pins[i]);
                vals[i] = abs(vals[i]); // positive values only
                vals[i] = map(vals[i], 0, max_val, 0, 255); // 8-bit conversion
                //Serial.println(diff);
                //Serial.write(vals[i]);

                if (vals[i] > globalThreshold)
                //if (diff > 100)
                {
                        digitalWrite(leds[i], HIGH);
                        switch (i) {
                        case 0:
                                load_kickFile();
                                delay(10); // delay time for buffer reading...
                                play_file(kickFile);
                                break;
                                //                        case 1:
                                //                                play_file(snare);
                                //                                break;
                                //                        case 2:
                                //                                play_file(hihat);
                                //                                break;
                                //                        case 3:
                                //                                play_file(tom);
                                //                                break;
                        }
                }
        }
}

void play_file(File soundfile)
{
  // -------------------------- write stream data to buffer
          for (uint32_t i = 0; i < sc_prestore_frames; i++)
          {
                  size_t supply_size = kickFile.read(s_buffer, sizeof(s_buffer));
                  s_remain_size -= supply_size;

                  err = theAudio->writeFrames(AudioClass::Player0, s_buffer, supply_size);
                  if (err != AUDIOLIB_ECODE_OK)
                  {
                          break;
                  }

                  if (s_remain_size == 0)
                  {
                          break;
                  }
          }

  theAudio->startPlayer(AudioClass::Player0); // starts player. once called, the player will be in active state, so it can be called until stopPlayer is called. When started, it starts reading the data for the Access Unit from the stream data buffer. --> stream data must be provided to stream buffer beforehand. (writeFrames before startPlay)
  puts("Play!");
        // ----------------------------------------- Audio Playback
        static bool is_carry_over = false;
        static size_t supply_size = 0;

        /* Send new frames to decode in a loop until file ends */
        while (s_remain_size > 0)
        {

                for (uint32_t i = 0; i < sc_store_frames; i++)
                {
                        if (!is_carry_over)
                        {
                                supply_size = soundfile.read(s_buffer, (s_remain_size < sizeof(s_buffer)) ? s_remain_size : sizeof(s_buffer));
                                s_remain_size -= supply_size;
                        }
                        is_carry_over = false;

                        int err = theAudio->writeFrames(AudioClass::Player0, s_buffer, supply_size);

                        if (err == AUDIOLIB_ECODE_SIMPLEFIFO_ERROR)
                        {
                                is_carry_over = true;
                                break;
                        }

                        if (s_remain_size == 0)
                        {
                                stop_player(soundfile);
                        }
                }

                if (ErrEnd)
                {
                        printf("Error End\n");
                        stop_player(soundfile);
                }
        }

        /* This sleep is adjusted by the time to read the audio stream file.
           Please adjust in according with the processing contents
           being processed at the same time by Application.
         */

        // usleep(1000);

}

void stop_player(File file_to_stop)

{
        puts("End.");
        // theAudio->stopPlayer(AudioClass::Player0);
        file_to_stop.close();
        //exit(1);
}
