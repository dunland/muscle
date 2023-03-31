import serial
from pythonosc.udp_client import SimpleUDPClient

IP = "127.0.0.1"
PORT = 9001
accepted_messages = ['Snare', 'Kick', 'Hihat', 'Tom1', 'Tom2', 'S_Tom1', 'S_Tom2', 'Ride', 'Crash1', 'Crash2', 'Cowbell', 'footswitch', 'Theodolit', 'Improvisation', 'Sattelstein', 'KupferUndGold']

ser = serial.Serial('/dev/ttyACM0', 9600)
# ser = serial.Serial('/dev/ttyACM0',
#                     baudrate=9600,
#                     parity=serial.PARITY_NONE,
#                     stopbits=serial.STOPBITS_ONE)

client = SimpleUDPClient(IP, PORT)


while(True):

    try:
        # read serial:
        data=ser.readline()
        print(data.decode())

        # decode serial and forward via OSC:
        for element in accepted_messages:
            if element in data.decode():
                msg = element
                client.send_message(msg, 1)
                print(msg)
    except:
        ser = serial.Serial('/dev/ttyACM0', 9600)
        client = SimpleUDPClient(IP, PORT)