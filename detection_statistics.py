import pandas as pd
import numpy as np
import os

# =========================
# USER INPUT
# =========================
input_folder = r"D:\PROJECTS\Control Systems\TEST RUNS\PWM 200\FALSE POSITIVE"

files = [f for f in os.listdir(input_folder) if f.endswith('.csv')]

pre_times = []
occ_times = []

print("\n===== FILE-WISE RESULTS =====\n")

# =========================
# PROCESS EACH FILE
# =========================
for idx, file in enumerate(files):

    file_path = os.path.join(input_folder, file)
    df = pd.read_csv(file_path)

    time = df['time']

    pre_event_idx = None
    occ_event_idx = None
    occ_start_idx = None

    # =========================
    # FIND EVENTS
    # =========================
    for i in range(len(df)):

        if df['occlusionStart'].iloc[i] == 1 and occ_start_idx is None:
            occ_start_idx = i

        if df['preStreak'].iloc[i] >= 15 and pre_event_idx is None:
            pre_event_idx = i

        if df['occStreak'].iloc[i] >= 3 and occ_event_idx is None:
            occ_event_idx = i

    # =========================
    # TIME CALCULATION
    # =========================
    pre_time = None
    occ_time = None

    if occ_start_idx is not None:

        if pre_event_idx is not None:
            pre_time = time.iloc[pre_event_idx] - time.iloc[occ_start_idx]
            pre_times.append(pre_time)

        if occ_event_idx is not None:
            occ_time = time.iloc[occ_event_idx] - time.iloc[occ_start_idx]
            occ_times.append(occ_time)

    # =========================
    # PRINT PER FILE
    # =========================
    print(f"File {idx+1}: {file}")
    print(f"  PRE Time: {pre_time if pre_time is not None else 'Not Detected'}")
    print(f"  OCC Time: {occ_time if occ_time is not None else 'Not Detected'}")
    print("")

# =========================
# FINAL STATISTICS
# =========================
print("===== FINAL DETECTION STATISTICS =====")

if len(pre_times) > 0:
    print(f"PRE Detection Time: Mean = {np.mean(pre_times):.3f}s, Std = {np.std(pre_times):.3f}s")
else:
    print("PRE Detection: No valid detections")

if len(occ_times) > 0:
    print(f"OCC Detection Time: Mean = {np.mean(occ_times):.3f}s, Std = {np.std(occ_times):.3f}s")
else:
    print("OCC Detection: No valid detections")