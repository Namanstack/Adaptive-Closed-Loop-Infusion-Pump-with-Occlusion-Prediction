# Adaptive Closed-Loop Infusion Pump with Occlusion Prediction

## Overview

This project presents an Adaptive Closed-Loop Infusion Pump system designed to detect and predict occlusions in a peristaltic infusion pump using real-time motor current analysis.
Traditional infusion systems detect blockage only after pressure crosses a threshold. This system introduces predictive behavior by monitoring current trends and dynamically responding before a complete blockage occurs.
The system combines signal processing, trend analysis, threshold logic, and adaptive control to improve safety and reduce delayed detection.

---

##  Key Features

- Real-time motor current monitoring using INA219
- Threshold-based occlusion detection
- Trend-based occlusion prediction
- Exponential moving average filtering
- Sliding window analysis
- False-positive reduction logic
- Adaptive closed-loop response mechanism
- PWM-controlled pump operation
- Real-time experimental validation

---

##  Working Principle

The peristaltic pump produces a pulsatile current waveform because rotating rollers periodically compress tubing.

Normal behavior:
- Periodic current oscillations
- Stable baseline

Occlusion behavior:
- Increased motor load
- Increased torque requirement
- Gradual or sudden rise in current

Two detection mechanisms are implemented:

### Method 1:
Threshold-based occlusion detection

Detects rapid current increase.

### Method 2:
Trend-based prediction

Predicts gradual blockage using moving-window analysis.

---

##  Hardware Used

- Arduino Uno
- INA219 Current Sensor
- Kamoer 12V Peristaltic Pump
- L298N Motor Driver
- Power Supply
- Tubing setup

---

##  Mathematical Model

### Exponential Moving Average Filter

FilteredCurrent =
α(Current)+(1−α)(PreviousFiltered)

where:

α = 0.15

---

### Sliding Window Analysis

Window Size = 25 samples

Computed values:

- Mean current
- Maximum current
- Baseline deviation

---

##  Project Structure

```text
.
├── Arduino_Code.ino
├── data_analysis.py
├── graph_generator.py
├── datasets/
├── plots/
├── documentation/
├── README.md
```

---

##  Experimental Results

### PWM = 200

PRE Detection Mean:
4.806 s

OCC Detection Mean:
4.023 s

Detection Rate:
100%

---

### PWM = 255

PRE Detection Mean:
3.693 s

OCC Detection Mean:
3.243 s

Detection Rate:
100%

Higher PWM significantly improved detection speed and consistency.

---

##  Results Summary

- Full occlusions detected successfully
- Lower false positives
- OCC detection faster than PRE detection
- Prediction mechanism detects early trends
- Robust behavior under experimental testing

---

## 👨‍💻 Technologies Used

- Arduino
- Embedded C
- Python
- Pandas
- NumPy
- Matplotlib

---

##  Contributors

### Naman Samadhiya
- Hardware setup
- Arduino implementation
- Data analysis
- Testing and validation
- Result generation

### Roumak Saha
- Control system modeling
- Literature survey
- Documentation

### Siddhant Pandey
- Research support
- Testing assistance

---

## 🔬 Future Scope

- ML-based occlusion prediction
- Cloud monitoring dashboard
- Mobile app integration
- FPGA implementation
- Hospital-grade deployment
