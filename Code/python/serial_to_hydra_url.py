"""
receives Serial messages from SUPERMUSCLE (teensy) and opens hydra-synth in a webbrowser with a url corresponding to the received song name.

MAKE SURE YOU HAVE A STABLE INTERNET CONNECTION WHILE STARTING THE APPLICATION!
First hydra has to be started:

```
cd path/to/github/hydra
yarn dev
```

then plug in SUPERMUSCLE & within its short startup time, do `python3 serial_to_hydra_url.py`

dunland, 2023-09
"""

import serial
import time
from selenium import webdriver

driver = webdriver.Chrome()
driver.get("http://localhost:8080/?sketch_id=intro")  # opens website


accepted_messages = ['intro', 'pogoNumberOne', 'hutschnur', 'randomVoice', 'wueste', 'besen', 'alhambra', 'mrWimbledon', 'roeskur', 'sattelstein', 'theodolit', 'kupferUndGold', 'donnerwetter']

ser = serial.Serial('/dev/ttyACM0', 9600)
# ser = serial.Serial('/dev/ttyACM0',
#                     baudrate=9600,
#                     parity=serial.PARITY_NONE,
#                     stopbits=serial.STOPBITS_ONE)


while(True):

    try:
        # read serial:
        data=ser.readline()
        print(data.decode())

        # decode serial and create link for hydra-synth
        for element in accepted_messages:
            if element in data.decode():
                msg = element
                link="http://localhost:8080/?sketch_id=" + msg
                print(link)
                driver.get(link)  # opens website
    except:
        # try:
        ser = serial.Serial('/dev/ttyACM0', 9600)
        print("could not read serial nor connect to it.")
        # except:
            # time.sleep(3)
