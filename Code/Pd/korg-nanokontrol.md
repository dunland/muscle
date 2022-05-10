# KORG nanokontrol to Pd:

1. connect device
2. start `qjackctl`
    - settings:
        - driver: alsa
        - MIDI driver: none
        - Options → Execute script on startup: `/usr/bin/a2jmidid &` (must be installed via `sudo apt-get install a2jmidid` (https://unix.stackexchange.com/questions/155214/jack-not-recognizing-midi-device)
3. start Pd
    - Edit → Preferences → MIDI midi-api: alsa
    - JACK: Connection: ALSA: 24:nanoKONTROL2 → 129:Pure Data
4. open patch `korg-nanokontrol.pd` to receive data from KORG nanoKONTROL2

