// RTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

// MultiTask setting
#define SENSORTASK_CORE 1
#define SENSORTASK_PRI 1

// RTC
#include  <RTC.h>
const char* ntpServer = "time.cloudflare.com";
RTC rtc;
struct tm timeInfo;
RTC_TimeTypeDef timeStruct, timeold;
RTC_DateTypeDef dateStruct, dateold;
char dateStr[20], timeStr[10];

// Week Info
const char* WEEKDAY[7] = {"Sun.", "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat."};

// Counter
char counterStr[10];

// Timer
unsigned long counter_down = 0;
TaskHandle_t vcountdown;
bool countIs0 = false;

// StopWatch
unsigned long counter_up = 0;
TaskHandle_t vcountup;