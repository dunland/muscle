# to be used with teensyDrums00_Debug_pinReads_timer.ino
# pump data into file with socat stdio /dev/ttyACM0 >> somefile.tsv

import pandas as pd
import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt

# pins = [1,2,3,4,5,6,7,8]
pins = [14, 15, 16, 17, 18, 19, 20, 21] # pins as given in MCU code; corresponds to teensy A0-A7

# plotColor = ['lightblue', 'turquoise', 'teal', 'lavender', 'pink', 'orangered', 'plum', 'purple']
plotColor = ['red', 'yellow', 'orange', 'brown', 'pink', 'orangered', 'plum', 'purple']

file = './drum_logs/01_counts_sensitivity/bassdrum_20200721-01.tsv'

fig = plt.figure()
plt.gcf()

while (1):

    df_input = pd.read_csv(file, sep='\t', header=None, names=['millis', 'pin', 'crossings', 'threshold', 'noiseFloor'])
    
    for i in range(len(pins)):
        next_df = df_input[df_input['pin'] == pins[i]]
        plt.plot(next_df['millis'], next_df['crossings'], plotColor[i])
    plt.ylabel("sensor counts (\"zero-crossings\")")
    plt.title("bassdrum, 50 ms after lastHitTime")
    # plt.legend(['threshold=10'])
    # plt.legend(['threshold=10', 'threshold=30', 'threshold=50', 'threshold=60', 'threshold=70', 'threshold=90', 'threshold=110', 'threshold=130'])
    # plt.legend(['threshold=40', 'threshold=90', 'threshold=150', 'threshold=200'])
    plt.legend(['threshold=200'])
    plt.draw()
    plt.pause(0.1)

plt.show()