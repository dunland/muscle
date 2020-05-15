#include <MediaRecorder.h>
#include <MediaPlayer.h>
#include <OutputMixer.h>
#include <MemoryUtil.h>

MediaRecorder *theRecorder;
MediaPlayer *thePlayer;
OutputMixer *theMixer;

const int32_t sc_buffer_size = 12288;
uint8_t s_buffer[sc_buffer_size];

bool ErrEnd = false;
/*-------------------CALLBACK FUNCTIONS---------------------------------------*/
static void attention_cb(const ErrorAttentionParam *atprm)
{
  puts("Attention!");

        if (atprm->error_code > AS_ATTENTION_CODE_WARNING)
        {
                ErrEnd = true;
        }
}

static bool mediarecorder_done_callback(AsRecorderEvent event, uint32_t result, uint32_t sub_result)
{
        return true;
}

static void outputmixer_done_callback(MsgQueId requester_dtq,
                                      MsgType reply_of,
                                      AsOutputMixDoneParam *done_param)
{
        return;
}

static void outmixer_send_callback(int32_t identifier, bool is_end)
{
        AsRequestNextParam next;

        next.type = (!is_end) ? AsNextNormalRequest : AsNextStopResRequest;

        AS_RequestNextPlayerProcess(AS_PLAYER_ID_0, &next);

        return;
}

static bool mediaplayer_done_callback(AsPlayerEvent event, uint32_t result, uint32_t sub_result)
{
        /* If result of "Play", Start recording to supply captured data to player */

        if (event == AsPlayerEventPlay)
        {
          theRecorder->start();
                /**
                 * @brief Start Recording
                 *
                 * @details This function starts recoding.
                 *          Once you call this function, the media recorder will be in active state
                 *          and encoded data will be stored into internal FIFO. You shold pull out
                 *          the data as soon as possible by "read frame API".
                 *          If you do not, FIFO will overflow and data will lack.
                 *
                 *          This will continue until you call "stop API".
                 *
                 */
          puts("start recorder");
        }

        return true;
}



void mediaplayer_decode_callback(AsPcmDataParam pcm_param)
{

// audio effects processing here

        theMixer->sendData(
                OutputMixer0,   //select output mixer handle
                outmixer_send_callback,   //output device
                pcm_param);   //callback function for API results

}

/* ------------------------------SETUP----------------------------------------*/
void setup()
{
        initMemoryPools(); // returns MemoryUtil.begin()
        createStaticPools(MEM_LAYOUT_RECORDINGPLAYER);

        theRecorder = MediaRecorder::getInstance();
        thePlayer = MediaPlayer::getInstance();
        theMixer  = OutputMixer::getInstance();

        theRecorder->begin();
        thePlayer->begin();

        theMixer->activateBaseband(); // activates audio HW ?? must be called to sound

        // create objects
        thePlayer->create( // activates output mixer system
                MediaPlayer::Player0,
                attention_cb
                );
        theMixer->create(attention_cb);

        // activate objects

        theRecorder->activate(AS_SETRECDR_STS_INPUTDEVICE_MIC, mediarecorder_done_callback);
        thePlayer->activate(MediaPlayer::Player0, AS_SETPLAYER_OUTPUTDEVICE_SPHP, mediaplayer_done_callback); // theRecorder is started in this callback function
        theMixer->activate(OutputMixer0, outputmixer_done_callback);

        usleep(100 * 1000); // ?? ist das nicht sehr lang?

        // initialize recorder:
        theRecorder->init(
                AS_CODECTYPE_LPCM, /**< Select compression code. AS_CODECTYPE_MP3 or AS_CODECTYPE_WAV or AS_CODECTYPE_LPCM*/
                AS_CHANNEL_MONO,/**< Set chennel number. AS_CHANNEL_MONO or AS_CHANNEL_STEREO, 2CH, 4CH, 8CH */
                // AS_SAMPLINGRATE_192000, /**< Set sampling rate. AS_SAMPLINGRATE_XXXXX */
                AS_SAMPLINGRATE_48000,
                AS_BITLENGTH_16, /**< Set bit length. AS_BITLENGTH_16 or AS_BITLENGTH_24 */
                // AS_BITRATE_384000 /**< Set bit rate. AS_BITRATE_XXXXX */
                AS_BITRATE_24000
                // --> RAM = 1.5MByte --> audio = 3.9sec
                // TODO: write to file rather than to FIFO?
                );
        puts("recorder init");

        // initialize Player:
        thePlayer->init(
                MediaPlayer::Player0, /**< Select Player ID. */
                AS_CODECTYPE_WAV, /**< Set compression code. AS_CODECTYPE_MP3 or AS_CODECTYPE_WAV */
                AS_SAMPLINGRATE_24000, /**< Set sampling rate. AS_SAMPLINGRATE_XXXXX */
                AS_CHANNEL_MONO/**< Set channnel number. AS_CHANNEL_MONO or AS_CHANNEL_STEREO */
                );
        puts("player init");

        theMixer->setVolume(
                0, /**< Master volume. -1020(-102db) - 120(12db) */
                0, /**< Player0 volume. -1020(-102db) - 120(12db) */
                0 /**< Plyaer1 volume. -1020(-102db) - 120(12db) */
                );
}

typedef enum
{
        StateReady = 0,
        StateRun,
} State;

void loop()
{
        static State s_state = StateReady;

        if (s_state == StateReady)
        {
                memset(s_buffer, 0, sizeof(s_buffer)); // sets the first [s_buffer] bytes of the block of  s_buffer to 0

                thePlayer->writeFrames(
                        MediaPlayer::Player0, /**< Select Player ID. */
                        s_buffer, /**< Pointer to audio data which would like to sound */
                        sizeof(s_buffer) /**< Size of audio data */
                        /* TODO: use File& myfile if you want to play from file instead of FIFO!*/
                        );

                thePlayer->start( //pulls out audio data from FIF. data has to be pushed by "write frame API"
                        MediaPlayer::Player0, /**< Select Player ID. */
                        mediaplayer_decode_callback /**< Callback function for notify decode completion */
                        // implement audio effects in mediaplayer_decode_callback!
                        );

                s_state = StateRun;
        }
        else if (s_state == StateRun)
        { // TODO: understand this section. I think it's important!
          /* get recorded data and play */
                uint32_t read_size = 0;

                do
                {
                        theRecorder->readFrames(
                                s_buffer, // uint8_t* p_buffer,
                                sc_buffer_size, // uint32_t buffer_size,
                                &read_size // uint32_t* read_size
                                );

                        if (read_size > 0)
                        {
                                thePlayer->writeFrames(MediaPlayer::Player0, s_buffer, read_size);
                        }

                } while (read_size != 0);
        }
        else {}

        if (ErrEnd)
        {
                printf("Error End\n");
                goto exitRecording;
        }

        usleep(1);

        return;

exitRecording:

        thePlayer->stop(MediaPlayer::Player0);
        theRecorder->stop();

        puts("Exit.");

        exit(1);

}
