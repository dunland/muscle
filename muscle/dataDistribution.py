# this should deliver some insights on the data collected from the piezo mics

import matplotlib
# matplotlib.rcsetup.all_backends
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
import numpy
import pandas as pd

# df = pd.read_csv('drum_logs/snaredrum_log20200516.txt')
df = pd.read_csv('drum_logs/hihat_20200516-02_normalized.txt')
df
df_sorted = df.sort_values(by='1')
len(df_sorted)

# %%

plt.figure()
plt.gcf()
# fig, ax = plt.subplots()
plt.plot(range(0, len(df_sorted), 1), df)  # unsorted
plt.plot(range(0, len(df_sorted), 1), df_sorted)  # sorted
plt.ylabel("normalized to 260")
plt.title("HiHat Contact Mic at TOP, levelled to 260, 2020-05-16")
plt.legend(['normalized to 260', 'sorted'])
plt.show()
