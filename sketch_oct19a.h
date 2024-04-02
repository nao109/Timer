#define SILENT

#include <M5Core2.h>

// Wi-fi
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
WiFiMulti wifiMulti;
HTTPClient http;
const char* SSID = "";
const char* PASSPHRASE = "";

// Gyro Sensor
float accX, accY, accZ;

// ENV.II UNIT
#include "M5_ENV.h"
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
SHT3X sht30;
Adafruit_BMP280 bme;
float tmp = 0.0, hum = 0.0, prs = 0.0;
char tmpStr[20], humStr[20], prsStr[20];

// Joystick UNIT
// Define Joystick I2C address
#define JOY_ADDR 0x52
// Center of Joystickの補正
#define X 136
#define Y 127
static int x_data, y_data;
static uint8_t button_data;

// window
#include "window.h"

// Timer etc.
#include "timer.h"

// Puzzle
#include "puzzle.h"

// wav
#include "wav.h"
#define CONFIG_I2S_BCK_PIN      12
#define CONFIG_I2S_LRCK_PIN     0
#define CONFIG_I2S_DATA_PIN     2
#define CONFIG_I2S_DATA_IN_PIN  34
#define SPEAKER_I2S_NUMBER      I2S_NUM_0
#define MODE_MIC                0
#define MODE_SPK                1
