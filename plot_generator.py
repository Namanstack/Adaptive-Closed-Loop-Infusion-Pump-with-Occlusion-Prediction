import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

# =========================
# USER INPUT
# =========================
input_folder = r"D:\PROJECTS\Control Systems\TEST RUNS\PWM 255\FALSE POSITIVE"     # folder with CSV files
output_folder = r"D:\PROJECTS\Control Systems\PLOTS\PWM 255 FALSE POS TEST"  # where plots will be saved

WINDOW_SIZE = 25

# =========================
# CREATE OUTPUT FOLDER
# =========================
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# =========================
# GET CSV FILES
# =========================
files = [f for f in os.listdir(input_folder) if f.endswith('.csv')]

pre_times = []
occ_times = []

# =========================
# PROCESS EACH FILE
# =========================
for idx, file in enumerate(files):

    file_path = os.path.join(input_folder, file)
    df = pd.read_csv(file_path)

    time = df['time']
    filtered = df['filtered']
    baseline = df['baseline']

    # Rolling calculations
    mean_vals = filtered.rolling(WINDOW_SIZE).mean()
    max_vals = filtered.rolling(WINDOW_SIZE).max()

    # Initialize indices
    pre_trigger_idx = None
    occ_trigger_idx = None
    pre_event_idx = None
    occ_event_idx = None
    occ_start_idx = None

    # =========================
    # FIND EVENTS
    # =========================
    for i in range(len(df)):

        if df['occlusionStart'].iloc[i] == 1 and occ_start_idx is None:
            occ_start_idx = i

        if df['preDetected'].iloc[i] == 1 and pre_trigger_idx is None:
            pre_trigger_idx = i

        if df['occDetected'].iloc[i] == 1 and occ_trigger_idx is None:
            occ_trigger_idx = i

        if df['preStreak'].iloc[i] >= 15 and pre_event_idx is None:
            pre_event_idx = i

        if df['occStreak'].iloc[i] >= 3 and occ_event_idx is None:
            occ_event_idx = i

    # =========================
    # TIME CALCULATION
    # =========================
    if occ_start_idx is not None:

        if pre_event_idx is not None:
            pre_time = time.iloc[pre_event_idx] - time.iloc[occ_start_idx]
            pre_times.append(pre_time)

        if occ_event_idx is not None:
            occ_time = time.iloc[occ_event_idx] - time.iloc[occ_start_idx]
            occ_times.append(occ_time)

    # =========================
    # PLOTTING
    # =========================
    plt.figure(figsize=(12,6))

    # Main signals
    plt.plot(time, filtered, label="Filtered Current", linewidth=2)
    plt.plot(time, baseline, linestyle='--', label="Baseline")
    plt.plot(time, mean_vals, label="Mean (Window)")
    plt.plot(time, max_vals, label="Max (Window)")

    # Occlusion start
    if occ_start_idx is not None:
        plt.scatter(time.iloc[occ_start_idx],
                    filtered.iloc[occ_start_idx],
                    marker='s', color='black', label="Occlusion Start")

    # PRE trigger
    if pre_trigger_idx is not None:
        plt.scatter(time.iloc[pre_trigger_idx],
                    filtered.iloc[pre_trigger_idx],
                    color='green', label="PRE Trigger")

    # OCC trigger
    if occ_trigger_idx is not None:
        plt.scatter(time.iloc[occ_trigger_idx],
                    filtered.iloc[occ_trigger_idx],
                    color='red', label="OCC Trigger")

    # PRE final (streak)
    if pre_event_idx is not None:
        plt.scatter(time.iloc[pre_event_idx],
                    filtered.iloc[pre_event_idx],
                    color='green', marker='x', s=100, label="PRE Final")

    # OCC final (streak)
    if occ_event_idx is not None:
        plt.scatter(time.iloc[occ_event_idx],
                    filtered.iloc[occ_event_idx],
                    color='red', marker='x', s=100, label="OCC Final")

    # Labels
    plt.title(f"Test {idx+1} - {file}")
    plt.xlabel("Time (s)")
    plt.ylabel("Current (mA)")
    plt.legend()
    plt.grid()

    # =========================
    # SAVE PLOT
    # =========================
    save_path = os.path.join(output_folder, f"plot_{idx+1}.png")
    plt.savefig(save_path, dpi=300)

    plt.show()

# =========================
# FINAL STATISTICS
# =========================
print("\n===== FINAL DETECTION STATISTICS =====")

if len(pre_times) > 0:
    print(f"PRE Detection Time: Mean = {np.mean(pre_times):.3f}s, Std = {np.std(pre_times):.3f}s")

if len(occ_times) > 0:
    print(f"OCC Detection Time: Mean = {np.mean(occ_times):.3f}s, Std = {np.std(occ_times):.3f}s")