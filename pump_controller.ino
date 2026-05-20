#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

// Motor driver pins
int IN1 = 8;
int IN2 = 9;
int ENA = 5;
int BUTTON = 7;

int reading_no = 0;
unsigned long startTime;

// =========================
// 🔴 SET PWM HERE ONLY
// =========================
int pwmValue = 255;
int appliedPWM = pwmValue;

// Filtering
float filteredCurrent = 0;
float alpha = 0.15;

// Baseline
float baselineCurrent = 0;
String event = "";

// =========================
// 🧠 MAIN WINDOW (existing)
// =========================
#define WINDOW_SIZE 25
float buffer[WINDOW_SIZE];
int bufIndex = 0;
bool bufferFilled = false;

float meanCurrent = 0;
float maxCurrent = 0;

// =========================
// 🔴 OCCLUSION
// =========================
int occlusionCounter = 0;
bool occlusionDetected = false;
int occStreak = 0;

// =========================
// 🔵 PREDICTION (NEW)
// =========================
#define SHORT_WINDOW 10
#define LONG_WINDOW 50

float shortBuffer[SHORT_WINDOW];
float longBuffer[LONG_WINDOW];

int shortIndex = 0;
int longIndex = 0;

bool shortFilled = false;
bool longFilled = false;

float shortMean = 0;
float longMean = 0;

int preStreak = 0;
bool preOcclusionDetected = false;

// =========================
// CONTROL
// =========================
int occlusionDuration = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  startTime = millis();

  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }

  ina219.setCalibration_32V_2A();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, pwmValue);
  delay(10000);
  Serial.println("time,reading,pwm,raw,filtered,mean,max,baseline,trend,occlusionStart,preDetected,occDetected,preStreak,occStreak,event"); 

  float sum = 0;
  for(int i = 0; i < 50; i++){
    sum += ina219.getCurrent_mA();
    delay(50);
  }

  baselineCurrent = sum / 50.0;

  filteredCurrent = baselineCurrent;
  
}


void loop() {
  event = "";
  // ---- Read current ----
  float sum = 0;
  for(int i = 0; i < 20; i++){
    sum += ina219.getCurrent_mA();
    delay(2);
  }

  float current_mA = sum / 20.0;

  // ---- Filter ----
  filteredCurrent = (alpha * current_mA) + ((1.0 - alpha) * filteredCurrent);

  // =========================
  // MAIN BUFFER
  // =========================
  buffer[bufIndex++] = filteredCurrent;

  if(bufIndex >= WINDOW_SIZE){
    bufIndex = 0;
    bufferFilled = true;
  }

  // =========================
  // PREDICTION BUFFERS
  // =========================
  shortBuffer[shortIndex++] = filteredCurrent;
  if(shortIndex >= SHORT_WINDOW){
    shortIndex = 0;
    shortFilled = true;
  }

  longBuffer[longIndex++] = filteredCurrent;
  if(longIndex >= LONG_WINDOW){
    longIndex = 0;
    longFilled = true;
  }

  // =========================
  // COMPUTE MAIN FEATURES
  // =========================
  if(bufferFilled){

    float sumWindow = 0;
    maxCurrent = 0;

    for(int i = 0; i < WINDOW_SIZE; i++){
      sumWindow += buffer[i];
      if(buffer[i] > maxCurrent){
        maxCurrent = buffer[i];
      }
    }

    meanCurrent = sumWindow / WINDOW_SIZE;

    float deltaMean = meanCurrent - baselineCurrent;
    float deltaMax  = maxCurrent - baselineCurrent;

    // =========================
    // 🔵 NEW PRE DETECTION (TREND)
    // =========================
    if(shortFilled && longFilled){

      float sumShort = 0;
      for(int i=0;i<SHORT_WINDOW;i++) sumShort += shortBuffer[i];
      shortMean = sumShort / SHORT_WINDOW;

      float sumLong = 0;
      for(int i=0;i<LONG_WINDOW;i++) sumLong += longBuffer[i];
      longMean = sumLong / LONG_WINDOW;

      float trend = shortMean - longMean;

      if(trend > 2.5){
        preStreak++;
      } else {
        preStreak = 0;
      }

      preOcclusionDetected = (preStreak > 5);
    }

    // =========================
    // 🔴 OCCLUSION DETECTION
    // =========================
    bool detectCondition = (deltaMean > 6) && (deltaMax > 15);
    bool resetCondition  = (deltaMean < 2);

    if(detectCondition){
      occlusionCounter += 2;
    } 
    else if(resetCondition){
      occlusionCounter -= 3;
    } 
    else{
      occlusionCounter -= 1;
    }

    occlusionCounter = constrain(occlusionCounter, 0, 20);

    if(occlusionCounter > 5){
      occlusionDetected = true;
    }
    if(occlusionCounter < 2){
      occlusionDetected = false;
    }

    // ---- streak ----
    if(occlusionDetected){
      occStreak++;
    } else {
      occStreak = 0;
    }

    // ========================= // 🔥 FINAL DECISION // ========================= 
    if(preStreak >= 15){
    event = "PRE";
      }
    else if(occStreak >= 3){
      event = "OCC";
      }

    // =========================
    // 🟢 BASELINE UPDATE
    // =========================
    if(!occlusionDetected && !preOcclusionDetected){
      if(deltaMean < 1.5){
        baselineCurrent = 0.99 * baselineCurrent + 0.01 * meanCurrent;
      }
      else if(deltaMean < 4){
        baselineCurrent = 0.9995 * baselineCurrent + 0.0005 * meanCurrent;
      }
    }
  }


  float timeSec = (millis() - startTime) / 1000.0;
  // ========================= // BUTTON MARKER // ========================= 
  int occlusionStart = (digitalRead(BUTTON) == LOW) ? 1 : 0; 
  // ========================= // CSV OUTPUT // ========================= 
  Serial.print(timeSec); Serial.print(","); 
  Serial.print(reading_no++); Serial.print(","); 
  Serial.print(appliedPWM); Serial.print(","); 
  Serial.print(current_mA); Serial.print(","); 
  Serial.print(filteredCurrent); Serial.print(","); 
  Serial.print(meanCurrent); Serial.print(","); 
  Serial.print(maxCurrent); Serial.print(","); 
  Serial.print(baselineCurrent); Serial.print(","); 
  Serial.print(shortMean - longMean); Serial.print(","); 
  Serial.print(occlusionStart); Serial.print(","); 
  Serial.print(preOcclusionDetected); Serial.print(","); 
  Serial.print(occlusionDetected ); Serial.print(","); 
  Serial.print(preStreak); Serial.print(","); 
  Serial.print(occStreak); Serial.print(","); 
  Serial.println(event);
    delay(100); 
}