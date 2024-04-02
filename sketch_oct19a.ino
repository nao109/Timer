#include "sketch_oct19a.h"

void setup() {
  // Init M5Core2
  M5.begin(true, true, true, true);

  // IMUの初期化
  M5.IMU.Init();

  // スプライトの作成
  sprite.setColorDepth(8);
  sprite.setTextFont(2);
  sprite.setTextSize(2);
  sprite.setTextColor(char_color, back_color);
  sprite.createSprite(WIDTH, HEIGHT);

  // Storage wifi configuration information.
  wifiMulti.addAP(SSID, PASSPHRASE);
  sprite.printf("\nConnecting Wifi...\n");
  sprite.pushSprite(0, 0);
  wifiMulti.run();

  // Init JoyStick Unit
  Wire.begin(32, 33, 400000UL);

  // Init Env.II UNIT
  sht30.init();
  bme.begin(0x76);

  //Init Speaker or Mic
  InitI2SSpeakerOrMic(MODE_MIC);

  // Create Task
  checkSensor();

  // Init RTC
  InitRTC();
}

void loop() {
  // Init screen
  sprite.fillScreen(back_color);
  sprite.pushSprite(0, 0);

  if(abs(accX) <= sqrt(2) / 2 && abs(accY) <= sqrt(2) / 2){
    while(1){
      Top();
      if(!(abs(accX) <= sqrt(2) / 2 && abs(accY) <= sqrt(2) / 2)) break;
    }
  }
  else if(abs(accX) <= sqrt(2) / 2 && accY > sqrt(2) / 2){
    // Btnが下 / タイマー
    while(1){
      Timer();
      if(!(abs(accX) <= sqrt(2) / 2 && accY > sqrt(2) / 2)) break;
    }
  }
  else if(abs(accX) <= sqrt(2) / 2 && accY < -sqrt(2) / 2){
    // Btnが上 / ストップウォッチ
    while(1){
      Stopwatch();
      if(!(abs(accX) <= sqrt(2) / 2 && accY < -sqrt(2) / 2)) break;
    }
  }
  else if(accX < -sqrt(2) / 2 && abs(accY) <= sqrt(2) / 2){
    // Btnが左 / 気温湿度計
    while(1){
      // Read ENV data
      readEnvData();
      if(!(accX < -sqrt(2) / 2 && abs(accY) <= sqrt(2) / 2)) break;
    }
  }
}


// TOP
void Top() {
  sprite.fillScreen(back_color);
  M5.Lcd.setRotation(1);

  // Clock / Settings / Puzzle
  sprite.drawCentreString("Clock", 160, 24, 2);
  sprite.drawRoundRect(90, 20, 140, 40, 10, char_color);
  sprite.drawCentreString("Settings", 160, 104, 2);
  sprite.drawRoundRect(90, 100, 140, 40, 10, char_color);
  sprite.drawCentreString("Puzzle", 160, 184, 2);
  sprite.drawRoundRect(90, 180, 140, 40, 10, char_color);

  sprite.pushSprite(0, 0);

  TouchPoint_t pos = M5.Touch.getPressPoint();
  if(Btn1.inHotZone(pos)){
    sprite.fillRoundRect(90, 20, 140, 40, 10, char_color);
    sprite.pushSprite(0, 0);
    Clock();
  }
  else if(Btn2.inHotZone(pos)){
    sprite.fillRoundRect(90, 100, 140, 40, 10, char_color);
    sprite.pushSprite(0, 0);
    Settings();
  }
  else if(Btn3.inHotZone(pos)){
    sprite.fillRoundRect(90, 180, 140, 40, 10, char_color);
    sprite.pushSprite(0, 0);
    Puzzle();
  }
}


// CLOCK
void Clock() {
  sprite.fillScreen(back_color);
  M5.Lcd.setRotation(1);

  sprite.drawRoundRect(90, 100, 140, 40, 10, char_color);
  sprite.drawCentreString("Back", 160, 104, 2);

  sprite.pushSprite(0, 0);

  while(1){
    RTC();
    sprintf(dateStr, "%04d/%02d/%02d %s", dateStruct.Year, dateStruct.Month, dateStruct.Date, WEEKDAY[dateStruct.WeekDay]);
    sprintf(timeStr, "%02d:%02d:%02d", timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds);

    sprite.fillRect(0, 10, 320, 74, back_color);

    sprite.drawCentreString(dateStr, 160, 10, 2);
    sprite.drawCentreString(timeStr, 160, 42, 2);
    sprite.pushSprite(0, 0);

    TouchPoint_t pos = M5.Touch.getPressPoint();
    if(Btn2.inHotZone(pos)){
      break;
    }
  }
}


// SETTINGS
void Settings() {
  sprite.fillScreen(back_color);
  M5.Lcd.setRotation(1);

  sprite.drawCentreString("Theme", 160, 24, 2);
  sprite.drawCentreString("Dark", 160, 104, 2);
  sprite.drawRoundRect(90, 100, 140, 40, 10, char_color);
  sprite.drawCentreString("Light", 160, 184, 2);
  sprite.drawRoundRect(90, 180, 140, 40, 10, char_color);

  sprite.pushSprite(0, 0);

  delay(100);

  while(1){
    TouchPoint_t pos = M5.Touch.getPressPoint();
    if(Btn2.inHotZone(pos)){
      sprite.fillRoundRect(90, 100, 140, 40, 10, char_color);
      sprite.pushSprite(0, 0);
      char_color = WHITE;
      back_color = BLACK;
      sprite.setTextColor(char_color, back_color);
      break;
    }
    else if(Btn3.inHotZone(pos)){
      sprite.fillRoundRect(90, 180, 140, 40, 10, char_color);
      sprite.pushSprite(0, 0);
      char_color = BLACK;
      back_color = WHITE;
      sprite.setTextColor(char_color, back_color);
      break;
    }
  }
}


// PUZZLE
void Puzzle() {
  sprite.fillScreen(back_color);
  M5.Lcd.setRotation(1);

  init_var();

  while(!gameover_flag){
    clear_field();

    if(!clear_flag){
      make_block();
      gameover();
      get_key();
      make_field();
      fix_block();
      fall_block();
    }
    else{
      clear_line();
      make_field2();
    }

    draw_field();
    delay(500);
  }

  sprite.drawCentreString("GAME OVER", 160, 120, 2);
  sprite.pushSprite(0, 0);

  while(1){
    M5.update();
    if(M5.BtnC.wasPressed()) break;
  }
}

void init_var(){
  for(i = 0; i < FIELD_HEIGHT; i++){
    for(j = 0; j < FIELD_WIDTH; j++){
      stage[i][j] = 0;
    }
  }

  for(i = 0; i < FIELD_HEIGHT; i++){
    for(j = 0; j < FIELD_WIDTH; j++){
      stage[i][0] = 9;
      stage[i][1] = 9;
      stage[i][2] = 9;
      stage[i][3] = 9;
      stage[i][4] = 9;
      stage[i][5] = 9;
      stage[i][6] = 9;
      stage[i][7] = 9;
      stage[i][8] = 9;
      stage[i][9] = 9;
      stage[20][j] = 9;
      stage[21][j] = 9;
      stage[22][j] = 9;
      stage[23][j] = 9;
      stage[i][22] = 9;
      stage[i][23] = 9;
      stage[i][24] = 9;
      stage[i][25] = 9;
      stage[i][26] = 9;
      stage[i][27] = 9;
      stage[i][28] = 9;
      stage[i][29] = 9;
      stage[i][30] = 9;
      stage[i][31] = 9;
    }
  }

  block_x = 14;
  block_y = 0;
  collision_flag = false;
  gameover_flag = false;
  make_block_flag = true;
  clear_flag = false;
  block_id = 0;
  clear_count = 0;
}

void init_var2(){
  block_x = 14;
  block_y = 0;
  make_block_flag = 1;
}

void make_block(){
  if(make_block_flag){
    block_id = random(0, BLOCK_PATTERN);
    for(y = 0; y < BLOCK_HEIGHT; y++){
      for(x = 0; x < BLOCK_WIDTH; x++){
        block[y][x] = blocks[(block_id * BLOCK_HEIGHT) + y][x];
      }
    }
    make_block_flag = false;
  }
}

void gameover(){
  collision_center();
  
  if(collision_flag){
    gameover_flag = true;
  }
}

void get_key(){
  // Joystickの情報を取得する
  // Request 3 bytes from the slave device.
  Wire.requestFrom(JOY_ADDR, 3);
  if(Wire.available()){ // If data is received.
    x_data = Wire.read();
    y_data = Wire.read();
    button_data = Wire.read();
  }

  // 座標補正
  x_data -= X;
  y_data -= Y;

  if(x_data < -50 && y_data >= x_data && y_data < -x_data){
    collision_left();
    if(collision_flag == 0) block_x--;
  }
  else if(x_data > 50 && y_data < x_data && y_data >= -x_data){
    collision_right();
    if(collision_flag == 0) block_x++;
  }
  else if(y_data > 50 && (y_data > x_data || y_data >= -x_data)){
    collision_bottom();
	  while(!collision_flag){
		  block_y++;
		  collision_bottom();
	  }
  }
  else if(x_data <= 50 && y_data <= 50 && button_data == 1){
    turn_right();
  }
}

void turn_right(){
  turn_point++;
  for(y = 0; y < BLOCK_HEIGHT; y++){
    for(x = 0; x < BLOCK_WIDTH; x++){
      turn_block[y][x] = blocks[(block_id * BLOCK_HEIGHT) + y][(turn_point % 4 * BLOCK_WIDTH) + x];
    }
  }

  collision_turn();

  if(!collision_flag){
    for(y = 0; y < BLOCK_HEIGHT; y++){
      for(x = 0; x < BLOCK_WIDTH; x++){
        block[y][x] = turn_block[y][x];
      }
    }
  }
  else{
    turn_point--;
  }
}

void collision_left(){
  collision_flag = false;

  for(y = 0; y < BLOCK_HEIGHT; y++){
    for(x = 0; x < BLOCK_WIDTH; x++){
      if(block[y][x] != 0){
        if(stage[block_y + y][block_x + (x - 1)] != 0){
          collision_flag = true;
        }
      }
    }
  }
}

void collision_right(){
  collision_flag = false;

  for(y = 0; y < BLOCK_HEIGHT; y++){
    for(x = 0; x < BLOCK_WIDTH; x++){
      if(block[y][x] != 0){
        if(stage[block_y + y][block_x + (x + 1)] != 0){
          collision_flag = true;
        }
      }
    }
  }
}

void collision_bottom(){
  collision_flag = false;

  for(y = 0; y < BLOCK_HEIGHT; y++){
    for(x = 0; x < BLOCK_WIDTH; x++){
      if(block[y][x] != 0){
        if(stage[block_y + (y + 1)][block_x + x] != 0){
          collision_flag = true;
        }
      }
    }
  }
}

void collision_center(){
  collision_flag = false;

  for(y = 0; y < BLOCK_HEIGHT; y++){
    for(x = 0; x < BLOCK_WIDTH; x++){
      if(block[y][x] != 0){
        if(stage[block_y + y][block_x + x] != 0){
          collision_flag = true;
        }
      }
    }
  }
}

void collision_turn(){
  collision_flag = false;

  for(y = 0; y < BLOCK_HEIGHT; y++){
    for(x = 0; x < BLOCK_WIDTH; x++){
      if(turn_block[y][x] != 0){
        if(stage[block_y + y][block_x + x] != 0){
          collision_flag = true;
        }
      }
    }
  }
}

void fix_block(){
  collision_bottom();

  if(collision_flag){
    save_block();
    search_line();
    if(!clear_flag) init_var2();
  }
}

void search_line(){
  for(i = 0; i < FIELD_HEIGHT - 4; i++){
    clear_line_point[i] = 0;
  }
  
  for(i = 0; i < FIELD_HEIGHT - 4; i++){
    for(j = 10; j < FIELD_WIDTH - 10; j++){
      if(stage[i][j] == 0){
        clear_line_point[i] = 1;
        break;
      }
    }
  }

  for(i = 0; i < FIELD_HEIGHT - 4; i++){
    if(clear_line_point[i] == 0){
      clear_flag = true;
      break;
    }
  }
}

void clear_line(){
  int remain_line_point[FIELD_HEIGHT - 4] = {0};
  int remain_line_index = 0;

  if(clear_count < 2){
    for(i = 0; i < FIELD_HEIGHT - 4; i++){
      if(clear_line_point[i] == 0){
        for(j = 10; j < FIELD_WIDTH - 10; j++){
          stage[i][j] = 0;
        }
      }
    }
    clear_count++;
  }
  else{
    for(i = FIELD_HEIGHT - 4; i >= 0; i--){
      if(clear_line_point[i] != 0){
        remain_line_point[remain_line_index] = i;
        remain_line_index++;
      }
    }

    remain_line_index = 0;
    for(i = FIELD_HEIGHT - 4; i >= 0; i--){
      for(j = 10; j < FIELD_WIDTH - 10; j++){
        stage[i][j] = stage[remain_line_point[remain_line_index]][j];
      }
      remain_line_index++;
    }

    clear_flag = 0;
    clear_count = 0;
    init_var2();
  }
}

void make_field(){
  for(i = 0; i < FIELD_HEIGHT; i++){
    for(j = 0; j < FIELD_WIDTH; j++){
      field[i][j] = stage[i][j];
    }
  }

  for(y = 0; y < BLOCK_HEIGHT; y++){
    for(x = 0; x < BLOCK_WIDTH; x++){
      field[block_y + y][block_x + x] += block[y][x];
    }
  }
}

void make_field2(){
  for(i = 0; i < FIELD_HEIGHT; i++){
    for(j = 0; j < FIELD_WIDTH; j++){
      field[i][j] = stage[i][j];
    }
  }
}

void save_block(){
  for(y = 0; y < BLOCK_HEIGHT; y++){
    for(x = 0; x < BLOCK_WIDTH; x++){
      stage[block_y + y][block_x + x] += block[y][x];
    }
  }
}

void draw_field(){
  sprite.fillScreen(back_color);

  for(i = 0; i < FIELD_HEIGHT; i++){
    for(j = 0; j < FIELD_WIDTH; j++){
      if(field[i][j] == 9){
        sprite.fillRect(j * 10 + 1, i * 10 + 1, 8, 8, char_color);
      }
      else if(field[i][j] == 1){
        sprite.drawRect(j * 10 + 1, i * 10 + 1, 8, 8, char_color);
      }
      else if(field[i][j] == 2){
        sprite.fillRect(j * 10 + 1, i * 10 + 1, 8, 8, char_color);
      }
    }
  }

  sprite.pushSprite(0, 0);
}

void clear_field(){
	for(i = 0; i < FIELD_HEIGHT; i++){
    for(j = 0; j < FIELD_WIDTH; j++){
			field[i][j] = 0;
		}
	}
}

void fall_block(){
  if(!make_block_flag) block_y++;
}


// Timer
void Timer() {
  TimerScreen();

  while(1){
    TouchPoint_t pos = M5.Touch.getPressPoint();

    if(Btn4.inHotZone(pos)){
      // Push Reset Btn

      counter_down = 0;

      // Reset fill WHITE
      sprite.fillRoundRect(10, 100, 140, 40, 10, char_color);
      sprite.pushSprite(0, 0);
    }
    else if(Btn6.inHotZone(pos)){
      if(counter_down <= 0) break;

      // Push Start Btn

      // fillRect / Start
      sprite.fillRoundRect(170, 180, 140, 40, 10, char_color);
      sprite.pushSprite(0, 0);

      // Start counting down
      sprite.fillScreen(back_color);
      sprite.drawCentreString("Timer", 160, 24, 2);
      // Btn5 / Stop
      sprite.drawCentreString("Stop", 240, 104, 2);
      sprite.drawRoundRect(170, 100, 140, 40, 10, char_color);
      sprite.pushSprite(0, 0);

      CountDown();

      // Stop fill WHITE
      sprite.fillRoundRect(170, 100, 140, 40, 10, char_color);
      sprite.pushSprite(0, 0);
      
      if(countIs0){
        countIs0 = false;
#ifdef SILENT
        M5.Axp.SetLDOEnable(3, true);
        delay(100);
        M5.Axp.SetLDOEnable(3, false);
#else
        PlayWav();
#endif
      }
    }
    else if(Btn7.inHotZone(pos)){
      // Push 1sec Btn

      counter_down = min(counter_down + 60, 86399ul);

      // 1min Btn fill WHITE
      sprite.fillRoundRect(165, 100, 70, 40, 10, char_color);
      sprite.pushSprite(0, 0);
    }
    else if(Btn8.inHotZone(pos)){
      // Push 1sec Btn

      counter_down = min(counter_down + 1, 86399ul);

      // 1sec Btn fill WHITE
      sprite.fillRoundRect(245, 100, 70, 40, 10, char_color);
      sprite.pushSprite(0, 0);
    }
    TimerScreen();

    if(!(abs(accX) <= sqrt(2) / 2 && accY > sqrt(2) / 2)){
      break;
    }

    delay(1);
  }
}

void TimerScreen() {
  sprite.fillScreen(back_color);
  M5.Lcd.setRotation(1);

  sprite.drawCentreString("Timer", 160, 24, 2);
  // Btn4 / Reset
  sprite.drawCentreString("Reset", 80, 104, 2);
  sprite.drawRoundRect(10, 100, 140, 40, 10, char_color);
  // Btn6 / Start
  sprite.drawCentreString("Start", 240, 184, 2);
  sprite.drawRoundRect(170, 180, 140, 40, 10, char_color);
  // Btn7 / 1min
  sprite.drawCentreString("1min.", 200, 104, 2);
  sprite.drawRoundRect(165, 100, 70, 40, 10, char_color);
  // Btn8 / 1sec
  sprite.drawCentreString("1sec.", 280, 104, 2);
  sprite.drawRoundRect(245, 100, 70, 40, 10, char_color);
  // Counter
  sprintf(counterStr, "%02d:%02d:%02d", counter_down / 3600, (counter_down / 60) % 60, counter_down % 60);
  sprite.drawCentreString(counterStr, 80, 184, 2);
  sprite.pushSprite(0, 0);
}

void CountDown() {
  xTaskCreatePinnedToCore(vCountDown, "vCountDown", 4096, NULL, 2, &vcountdown, 1);

  while(1){
    sprintf(counterStr, "%02d:%02d:%02d", counter_down / 3600, (counter_down / 60) % 60, counter_down % 60);
    sprite.drawCentreString(counterStr, 80, 184, 2);
    sprite.pushSprite(0, 0);

    if(counter_down == 0){
      countIs0 = true;
      vTaskDelete(vcountdown);
      break;
    }

    TouchPoint_t pos = M5.Touch.getPressPoint();
    if(Btn5.inHotZone(pos)){
      // Push Stop Btn
      vTaskDelete(vcountdown);
      break;
    }
  }
}

void vCountDown(void *pvParameters) {
  (void)pvParameters;

  while(1){
    // このタスクは周期的に実行される
    vTaskDelay(1000);

    counter_down--;

    Serial.printf("%d\n", counter_down);
  }
}


// STOPWATCH
void Stopwatch() {
  StopwatchScreen();

  while(1){
    TouchPoint_t pos = M5.Touch.getPressPoint();

    if(Btn4.inHotZone(pos)){
      // Push Reset Btn

      counter_up = 0;

      // Reset fill WHITE
      sprite.fillRoundRect(10, 100, 140, 40, 10, char_color);
      sprite.pushSprite(0, 0);
    }
    else if(Btn6.inHotZone(pos)){
      // Push Start Btn

      // fillRect / Start
      sprite.fillRoundRect(170, 180, 140, 40, 10, char_color);
      sprite.pushSprite(0, 0);

      // Start counting up
      sprite.fillScreen(back_color);
      sprite.drawCentreString("Stopwatch", 160, 24, 2);
      // Btn5 / Stop
      sprite.drawCentreString("Stop", 240, 104, 2);
      sprite.drawRoundRect(170, 100, 140, 40, 10, char_color);
      sprite.pushSprite(0, 0);

      CountUp();

      // Stop fill WHITE
      sprite.fillRoundRect(170, 100, 140, 40, 10, char_color);
      sprite.pushSprite(0, 0);
    }
    StopwatchScreen();

    if(!(abs(accX) <= sqrt(2) / 2 && accY < -sqrt(2) / 2)){
      break;
    }

    delay(1);
  }
}

void StopwatchScreen() {
  sprite.fillScreen(back_color);
  M5.Lcd.setRotation(3);

  sprite.drawCentreString("Stopwatch", 160, 24, 2);
  // Btn4 / Reset
  sprite.drawCentreString("Reset", 80, 104, 2);
  sprite.drawRoundRect(10, 100, 140, 40, 10, char_color);
  // Btn6 / Start
  sprite.drawCentreString("Start", 240, 184, 2);
  sprite.drawRoundRect(170, 180, 140, 40, 10, char_color);
  // Counter
  sprintf(counterStr, "%02d:%02d:%02d", counter_up / 3600, (counter_up / 60) % 60, counter_up % 60);
  sprite.drawCentreString(counterStr, 80, 184, 2);
  sprite.pushSprite(0, 0);
}

void CountUp() {
  TaskHandle_t vcountup;

  xTaskCreatePinnedToCore(vCountUp, "vCountUp", 4096, NULL, 2, &vcountup, 1);

  while(1){
    sprintf(counterStr, "%02d:%02d:%02d", counter_up / 3600, (counter_up / 60) % 60, counter_up % 60);
    sprite.drawCentreString(counterStr, 80, 184, 2);
    sprite.pushSprite(0, 0);

    if(counter_up >= 86400){
      counter_up = 0;
      vTaskDelete(vcountup);
      break;
    }

    TouchPoint_t pos = M5.Touch.getPressPoint();
    if(Btn5.inHotZone(pos)){
      // Push Stop Btn
      vTaskDelete(vcountup);
      break;
    }
  }
}

void vCountUp(void *pvParameters) {
  (void)pvParameters;

  while(1){
    // このタスクは周期的に実行される
    vTaskDelay(1000);
    counter_up++;
  }
}


// RTC
void InitRTC() {
  if(wifiMulti.run() != WL_CONNECTED){
    Serial.print("connect failed\n");
    return;
  }

  // Set ntp time to local
  configTime(9 * 3600, 0, ntpServer);

  // Init RTC
  rtc.begin();

  // Get local time
  if(getLocalTime(&timeInfo)){
    // Set RTC time
    timeStruct.Hours   = timeInfo.tm_hour;
    timeStruct.Minutes = timeInfo.tm_min;
    timeStruct.Seconds = timeInfo.tm_sec;
    rtc.SetTime(&timeStruct);

    // Set RTC Date
    dateStruct.WeekDay = timeInfo.tm_wday;
    dateStruct.Month   = timeInfo.tm_mon + 1;
    dateStruct.Date    = timeInfo.tm_mday;
    dateStruct.Year    = timeInfo.tm_year + 1900;
    rtc.SetDate(&dateStruct);
  }
}

void RTC() {
  rtc.GetDate(&dateStruct);
  rtc.GetTime(&timeStruct);
  dateold = dateStruct;
  timeold = timeStruct;

  // Get RTC
  rtc.GetDate(&dateStruct);
  rtc.GetTime(&timeStruct);

  delay(1);
}


// READ_ENV_DATA
void readEnvData() {
  sprite.fillScreen(back_color);
  M5.Lcd.setRotation(0);

  if(sht30.get() == 0){           // Obtain the data of shT30.
    tmp = sht30.cTemp;            // Store the temperature obtained from shT30.
    hum = sht30.humidity;         // Store the humidity obtained from shT30.
  }
  else{
    tmp = nan("");
    hum = nan("");
  }

  if(bme.begin(0x76)){
    prs = bme.readPressure() / 100;
  }
  else{
    prs = nan("");
  }

  sprintf(tmpStr, "Temp(dC): %.1f\n", tmp);
  sprintf(humStr, "Hum(pct.): %.1f\n", hum);
  sprintf(prsStr, "Prs(hPa): %.1f\n", prs);

  sprite.drawCentreString(tmpStr, 120, 0, 2);
  sprite.drawCentreString(humStr, 120, 40, 2);
  sprite.drawCentreString(prsStr, 120, 80, 2);

  sprite.pushSprite(0, 0);
}


// RTOS
void checkSensor() {
  // Multi task start
  xTaskCreatePinnedToCore(GetSensorTask, "GetSensorTask", 4096, NULL, SENSORTASK_PRI, NULL, SENSORTASK_CORE);
}

// センサー&ボタンの情報を取得する
void GetSensorTask(void *pvParameters) {
  (void)pvParameters;

  while(1){
    // gyroX, gyroYの取得
    M5.IMU.getAccelData(&accX, &accY, &accZ);

    M5.update();
    if(M5.BtnB.isPressed()){
      M5.Axp.PowerOff();
    }

    // このタスクは周期的に実行される
    vTaskDelay(500);
  }
}


// I2SSpeakerOrMic
void InitI2SSpeakerOrMic(int mode) {
  esp_err_t err = ESP_OK;

  i2s_driver_uninstall(SPEAKER_I2S_NUMBER);
  i2s_config_t i2s_config = {
    .mode                 = (i2s_mode_t)(I2S_MODE_MASTER),
    .sample_rate          = 16000,
    .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format       = I2S_CHANNEL_FMT_ALL_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count        = 6,
    .dma_buf_len          = 60,
    .use_apll             = false,
    .tx_desc_auto_clear   = true,
    .fixed_mclk           = 0
  };
  if (mode == MODE_MIC) {
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
  } else {
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  }

  err += i2s_driver_install(SPEAKER_I2S_NUMBER, &i2s_config, 0, NULL);

  i2s_pin_config_t tx_pin_config = {
    .bck_io_num           = CONFIG_I2S_BCK_PIN,
    .ws_io_num            = CONFIG_I2S_LRCK_PIN,
    .data_out_num         = CONFIG_I2S_DATA_PIN,
    .data_in_num          = CONFIG_I2S_DATA_IN_PIN,
  };
  err += i2s_set_pin(SPEAKER_I2S_NUMBER, &tx_pin_config);

  if (mode != MODE_MIC) {
    err += i2s_set_clk(SPEAKER_I2S_NUMBER, 16000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
  }

  i2s_zero_dma_buffer(SPEAKER_I2S_NUMBER);
}

void PlayWav() {
  size_t bytes_written;
  M5.Axp.SetSpkEnable(true);
  InitI2SSpeakerOrMic(MODE_SPK);
  // Play wav1
  // Write Speaker
  i2s_write(SPEAKER_I2S_NUMBER, wav, sizeof(wav), &bytes_written, portMAX_DELAY);
  i2s_zero_dma_buffer(SPEAKER_I2S_NUMBER);
  // Set Mic Mode
  InitI2SSpeakerOrMic(MODE_MIC);
  M5.Axp.SetSpkEnable(false);
}
