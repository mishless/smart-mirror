#ifndef MAIN_H
#define MAIN_H

#include <opencv2\features2d\features2d.hpp>
#include <opencv2\video\video.hpp>
#include <pthread.h>
#include <string>
#include <iostream>
#include <fstream>
#include "frame.h"
#include "timer.h"
#include "recognizer.h"
#include "FrequencyDetector.h"
#include "phaseDetector.h"
#include "BPNeuralNetwork.h"

typedef pthread_t Thread;

const unsigned long long int MAX_BUFFER_SIZE = 2000;
const int REFRESH_PERIOD = 20;
const int RESPIRATION_RATE_WINDOW = 30;
const int HEARTBEAT_WINDOW = 6;
const int BLOOD_PRESSURE_WINDOW = 20;

enum State { FACE_DETECTED, NO_FACE_DETECTED };

#define FACE_W 300
#define FACE_H 300

#define EYES_W 200
#define EYES_H  40

#define HAND_W 200
#define HAND_H 200

#define RECOGNITION_TRESHOLD 10000

#define HR_LOW_FREQ 0.75 /* Hertz */
#define HR_HIGH_FREQ 4   /* Hertz */

#define RR_LOW_FREQ 0.1  /* Hertz */
#define RR_HIGH_FREQ 0.5 /* Hertz */

#define HR_WINDOW 172
#define TOTAL_DFT_WINDOW 256

#define SAMPLING_PERIOD ((double)35) /* Milliseconds*/
#define SAMPLING_FREQUENCY ((double)1000 / SAMPLING_PERIOD) /* Hertz */

#define BLOOD_PRESSURE_PERIOD 20000 /* Milliseconds */


#define DROWSINESS_PERIOD 20000 /* Milliseconds */
#define EYE_RECORDING_MAX 2000 /* Milliseconds*/

#define AWAKE_THRESHOLD  0.70   /* Percent */
#define DROWSY_THRESHOLD 0.95 /* Percent */

typedef enum { LOW_PRESSURE, NORMAL_PRESSURE, HIGH_PRESSURE } BloodPressure_t;
#endif