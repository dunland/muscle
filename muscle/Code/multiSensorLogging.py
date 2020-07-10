import pandas as pd
import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt

# pins = [1,2,3,4,5,6,7,8]
pins = [14, 15, 16, 17, 18, 19, 20, 21] # pins as given in MCU code; corresponds to teensy A0-A7

plotColor = ['lightblue', 'turquoise', 'teal', 'lavender', 'pink', 'orangered', 'plum', 'purple']

fig = plt.figure()
plt.gcf()

while (1):

    df_input = pd.read_csv('./drum_logs/crossings_sensitivity_20200709/snare_log_multiPins04.tsv', sep='\t', header=None, names=['millis', 'pin', 'crossings', 'threshold'])
    
    for i in range(len(pins)):
        next_df = df_input[df_input['pin'] == pins[i]]
        plt.plot(next_df['millis'], next_df['crossings'], plotColor[i])
    plt.ylabel("zero-crossings")
    plt.title("snaredrum ungedämpft, 50 ms after lastHitTime")
    # plt.legend(['threshold=10'])
    # plt.legend(['threshold=10', 'threshold=30', 'threshold=50', 'threshold=60', 'threshold=70', 'threshold=90', 'threshold=110', 'threshold=130'])
    plt.legend(['threshold=30', 'threshold=60', 'threshold=90', 'threshold=120'])
    plt.draw()
    plt.pause(0.1)

plt.show()