// Width, Height of LCD
#define WIDTH M5.Lcd.width()
#define HEIGHT M5.Lcd.height()

// TFT_eSprite classのインスタンス化
TFT_eSprite sprite = TFT_eSprite(&M5.Lcd);

// Color
unsigned int char_color = WHITE;
unsigned int back_color = BLACK;

// Button
HotZone Btn1(90, 20, 230, 60); // top-center
HotZone Btn2(90, 100, 230, 140); // middle-center
HotZone Btn3(90, 180, 230, 220); // down-center
HotZone Btn4(10, 100, 150, 140); // middle-left
HotZone Btn5(170, 100, 310, 140); // middle-right
HotZone Btn6(170, 180, 310, 220); // down-right
HotZone Btn7(165, 100, 235, 140); // middle-center-small
HotZone Btn8(245, 100, 315, 140); // middle-right-small