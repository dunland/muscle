# this should deliver some insights on the data collected from the piezo mics

import matplotlib
# matplotlib.rcsetup.all_backends
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
import numpy
import pandas as pd

# df = pd.read_csv('drum_logs/snaredrum_log20200516.txt')
df = pd.read_csv('drum_logs/snaredrum_20200516-02_8bit.txt')

df_sorted = df.sort_values(by='0')
len(df_sorted)

# %%

fig = plt.figure()
plt.plot(range(0, len(df_sorted), 1), df)  # unsorted
plt.plot(range(0, len(df_sorted), 1), df_sorted)  # sorted
