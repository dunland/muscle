#include <SDHCI.h>
#include <Audio.h>

/* File name to play */

#define PLAY0_FILE_NAME   "samples/AGOGO1.wav"
//#define PLAY0_FILE_NAME   "Sound.wav"
#define PLAY1_FILE_NAME   "samples/CLAP.WAV"

/* Set volume[db] */

#define VOLUME_MASTER     -160
#define VOLUME_PLAY0      -160
#define VOLUME_PLAY1      -160

SDClass theSD;

// static const uint8_t pins[] = {A0, A1, A2, A3};
// static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3};
// uint8_t vals[4];
// static const uint8_t inputAmount = 3; // that makes 4 input since number 0 is counted as well

WavContainerFormatParser theParser;

/**
    @brief Repeated transfer of data read into the decoder
*/

void play_process( AudioClass *theAudio, AudioClass::PlayerId play_id, File& file)
{
  //  const int32_t sc_buffer_size = 6144;
  //  uint8_t s_buffer[sc_buffer_size];
  //  uint32_t s_remain_size = 0;
  //
  //  static const uint32_t sc_store_frames = 10;
  //  static bool is_carry_over = false;
  //  static size_t supply_size = 0;
  //  puts("play");

  /* Send new frames to decode in a loop until file ends */
  //  for (uint32_t i = 0; i < sc_store_frames; i++)
  //  {
  //    if (!is_carry_over)
  //    {
  //      supply_size = file.read(s_buffer, (s_remain_size < sizeof(s_buffer)) ? s_remain_size : sizeof(s_buffer));
  //      s_remain_size -= supply_size;
  //    }
  //    is_carry_over = false;
  //
  //    int err = theAudio->writeFrames(AudioClass::Player0, s_buffer, supply_size);
  //    Serial.print(".");
  //
  //    if (err == AUDIOLIB_ECODE_SIMPLEFIFO_ERROR)
  //    {
  //      is_carry_over = true;
  //      return;
  //    }
  //
  //    if (s_remain_size == 0)
  //    {
  //      puts("s_remain_size == 0. return!");
  //      return;
  //    }
  //  }

  while (1)
  {
    /* Send frames to be decoded */

    err_t err = theAudio->writeFrames(play_id, file);

    /*  Tell when one of player file ends */

    if (err == AUDIOLIB_ECODE_FILEEND)
    {
      printf("Player%d File End!\n", play_id);
      break;
    }
    else if (err != AUDIOLIB_ECODE_OK)
    {
      printf("Player%d error code: %d\n", play_id, err);
      break;
    }

    usleep(10000);
  }
}


/**
    @brief Setup main audio player and sub audio player

    Set output device to speaker <br>
    Open two stream files "Sound0.mp3" and "Sound1.mp3" <br>
    These should be in the root directory of the SD card. <br>
    Set main player to decode stereo mp3. Stream sample rate is set to "auto detect" <br>
    System directory "/mnt/sd0/BIN" will be searched for MP3 decoder (MP3DEC file) <br>
    This is the /BIN directory on the SD card. <br>
*/

static int player_thread(int argc, FAR char *argv[])
{

  AudioClass::PlayerId play_id;
  err_t err;
  File file;
  const char           *file_name;

  /* Get static audio instance */

  AudioClass *theAudio = AudioClass::getInstance();

  /* Get information by task */

  play_id   = (AudioClass::PlayerId)atoi(argv[1]);
  file_name = argv[2];

  printf("\"%s\" task start\n", argv[0]);

  /* Continue playing the same file. */

  //  fmt_chunk_t fmt;
  //
  //  handel_wav_parser_t *handle = (handel_wav_parser_t *)theParser.parseChunk("/mnt/sd0/Sound.wav", &fmt);
  //  if (handle == NULL)
  //  {
  //    printf("Wav parser error.\n");
  //    exit(1);
  //  }

  usleep(10);

  while (1)
  {
    int val = 260 - analogRead(A0); // TODO: make sensor read dynamic
    val = abs(val);
    val = map(val, 0, 762, 0, 255);
    Serial.print("A0 = ");
    Serial.println(analogRead(A0));
    Serial.print("val = ");
    Serial.println(val);

    if (val > 90)
    {

      //while (1)
      //{
      digitalWrite(LED_BUILTIN, HIGH);
      /*
         Set player to decode stereo mp3. Stream sample rate is set to "auto detect"
         Search for MP3 decoder in "/mnt/sd0/BIN" directory
      */
      //err = theAudio->initPlayer(play_id, AS_CODECTYPE_WAV, "/mnt/sd0/BIN", fmt.rate, fmt.bit, fmt.channel);
      err = theAudio->initPlayer(play_id, AS_CODECTYPE_WAV, "/mnt/sd0/BIN", AS_SAMPLINGRATE_48000, AS_CHANNEL_MONO);

      /* Verify player initialize */

      if (err != AUDIOLIB_ECODE_OK)
      {
        printf("Player%d initialize error\n", play_id);
        break;
      }

      printf("Open \"%s\" file\n", file_name);

      /* Open file placed on SD card */

      file = theSD.open(file_name);

      /* Verify file open */

      if (!file)
      {
        printf("Player%d file open error\n", play_id);
        break;
      }
      printf("Open! %s\n", file.name());


      /* Send first frames to be decoded */

      err = theAudio->writeFrames(play_id, file);

      if (err != AUDIOLIB_ECODE_OK && err != AUDIOLIB_ECODE_FILEEND)
      {
        printf("Player%d: File Read Error! =%d\n", play_id, err);
        file.close();
        break;
      }

      printf("Play%d!\n", play_id);

      /* Play! */

      theAudio->startPlayer(play_id);

      printf("Start player%d!\n", play_id);

      /* Running... */

      play_process(theAudio, play_id, file);

      /* Stop! */

      theAudio->stopPlayer(play_id);

      file.close();

      digitalWrite(LED_BUILTIN, LOW);
      //} // while end
    }
  }

  printf("Exit task(%d).\n", play_id);

  exit(1);

  return 0;
}


void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  /* Get static audio instance */

  AudioClass  *theAudio = AudioClass::getInstance();

  puts("Initialization Audio Library");

  /* start audio system */

  theAudio->begin();

  /* Mount SD card */

  theSD.begin();

  /* Set output device to speaker */

  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP);

  /* Set master volume, Player0 volume, Player1 volume */

  theAudio->setVolume(VOLUME_MASTER, VOLUME_PLAY0, VOLUME_PLAY1);

  /* Initialize task parameter. */

  const char *argv0[3];
  const char *argv1[3];
  char play_no0[4];
  char play_no1[4];

  snprintf(play_no0, 4, "%d", AS_PLAYER_ID_0);
  snprintf(play_no1, 4, "%d", AS_PLAYER_ID_1);

  argv0[0] = play_no0;
  argv0[1] = PLAY0_FILE_NAME;
  argv0[2] = NULL;
  argv1[0] = play_no1;
  argv1[1] = PLAY1_FILE_NAME;
  argv1[2] = NULL;

  /* Start task */

  task_create("player_thread0", 155, 2048, player_thread, (char* const*)argv0);
  //task_create("player_thread1", 155, 2048, player_thread, (char* const*)argv1);
}


void loop()
{
  /* Do nothing on main task. */

  sleep(1);
}
