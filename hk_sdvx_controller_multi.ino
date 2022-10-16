#include <HID-Project.h>
#include <HID-Settings.h>
#include <Rotary.h>

#include <LiquidCrystal_SoftI2C.h>
// Set SDA to pin A0 and SCL to pin A1
SoftwareWire *wire = new SoftwareWire(A0, A1);
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2, wire);

enum Mode
{
  keymode,       //キー入力モード
  Mousemode,     //マウス入力モード
  AnalogXYmode,  //アナログスティックXY循環モード
  AnalogZrZmode, //アナログスライダー循環モード
};
class Button
{
public:
  const char keymap[6] = {'s', 'd', 'k', 'l', 'c', 'm'};
  const char VOL_L = 0, VOL_R = 1;
  const char VOL_LL = 'q', VOL_LR = 'w', VOL_RL = 'o', VOL_RR = 'p';

private:
  ;
};
const byte ENC1_A = 8;  // D8
const byte ENC1_B = 9;  // D9
const byte ENC2_A = 10; // D10
const byte ENC2_B = 11; // D11

Rotary r1 = Rotary(ENC1_A, ENC1_B);
Rotary r2 = Rotary(ENC2_A, ENC2_B);
volatile int posision[2] = {0, 0};
volatile int Arrayright[2] = {0, 0};
volatile int Arrayleft[2] = {0, 0};
volatile const int ENCMAX = 10;

int16_t AnalogPadX = 0;
int16_t AnalogPadY = 0;
uint16_t AnalogPadz = 0;
uint16_t AnalogPadrz = 0;

byte mode = 0;
byte ModeCount[7];
const byte MODECOUNTMAX = 80; //モード切り替え時の長押し時間
Button button;
int ButtonFlag;

void setup()
{
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);

  Serial.begin(9600);
  Gamepad.begin();
  r1.begin();
  r2.begin();
  for (int i = 0; i <= 13; i++)
  { // 13pinすべてアップ
    pinMode(i, INPUT_PULLUP);
  }
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT4) | (1 << PCINT5) | (1 << PCINT6) | (1 << PCINT7);
  sei();
}

void loop()
{

  keyFunc();
  ReduseValue();
  ModeChange();
  LCDShow();
  // Serial.println("right" + String(0) + ":" + Arrayright[0]);
  // Serial.println("left" + String(0) + ":" + Arrayleft[0]);
  // Serial.println("right" + String(1) + ":" + Arrayright[1]);
  // Serial.println("left" + String(1) + ":" + Arrayleft[1]);
  // Serial.println("sizeof int :" + String(sizeof(int)));
  // Serial.println("Mode:" + String(Mode));
}
void LCDShow()
{
  lcd.setCursor(0, 0); // x,y

  switch (mode)
  {
  case keymode:
    lcd.print("keyboard");
    break;
  case Mousemode:
    lcd.print("MouseXY ");
    break;
  case AnalogXYmode:
    lcd.print("AnalogXY");
    break;
  case AnalogZrZmode:
    lcd.print("AnalogRZ");
    break;

  default:
    break;
  }
}
void ReduseValue() //右、左つまみの値を減らし続ける
{
  for (int i = 0; i < (sizeof(Arrayleft) / sizeof(int)); i++)
  {

    Arrayright[i]--;
    Arrayleft[i]--;
    if (Arrayright[i] <= 0)
    {
      Arrayright[i] = 0;
    }
    if (Arrayleft[i] <= 0)
    {
      Arrayleft[i] = 0;
    }
  }
}
void keyFunc()
{
  for (int i = 0; i < (sizeof(button.keymap) / sizeof(char)); i++)
  {

    if (!digitalRead(i) == HIGH)
    {
      NKROKeyboard.press(button.keymap[i]);
    }
    else if (!digitalRead(i) == LOW)
    {
      NKROKeyboard.release(button.keymap[i]);
    }
  }
  if (!digitalRead(6) == HIGH)
  {
    NKROKeyboard.press(KEY_ENTER);
  }
  else
  {
    NKROKeyboard.release(KEY_ENTER);
  }

  if (mode == keymode)
  {
    //キーボードモード

    Arrayright[button.VOL_L] > 0 ? NKROKeyboard.press(button.VOL_LR) : NKROKeyboard.release(button.VOL_LR); //左のつまみが右回転
    Arrayright[button.VOL_R] > 0 ? NKROKeyboard.press(button.VOL_RR) : NKROKeyboard.release(button.VOL_RR); //右のつまみが右回転
    Arrayleft[button.VOL_L] > 0 ? NKROKeyboard.press(button.VOL_LL) : NKROKeyboard.release(button.VOL_LL);  //左のつまみが左回転
    Arrayleft[button.VOL_R] > 0 ? NKROKeyboard.press(button.VOL_RL) : NKROKeyboard.release(button.VOL_RL);  //右のつまみが左回転
  }
  else if (mode == Mousemode)
  {
    //マウス座標モード
    uint8_t pos = 30;
    Arrayright[button.VOL_L] > 0 ? Mouse.move(pos, 0) : Mouse.move(0, 0); //左のつまみが右回転
    Arrayright[button.VOL_R] > 0 ? Mouse.move(0, pos) : Mouse.move(0, 0); //右のつまみが左回転
    Arrayleft[button.VOL_L] > 0 ? Mouse.move(-pos, 0) : Mouse.move(0, 0); //左のつまみが左回転
    Arrayleft[button.VOL_R] > 0 ? Mouse.move(0, -pos) : Mouse.move(0, 0); //右のつまみが右回転
  }
  else if (mode == AnalogXYmode)
  {
    //アナログスティックXYモード
    uint8_t AddXY = 2500;
    Arrayright[button.VOL_L] > 0 ? AnalogPadX += AddXY : AnalogPadX += 0; //左のつまみが右回転
    Arrayright[button.VOL_R] > 0 ? AnalogPadY += AddXY : AnalogPadY += 0; //右のつまみが左回転
    Arrayleft[button.VOL_L] > 0 ? AnalogPadX -= AddXY : AnalogPadX -= 0;  //左のつまみが左回転
    Arrayleft[button.VOL_R] > 0 ? AnalogPadY -= AddXY : AnalogPadY -= 0;  //右のつまみが右回転

    Gamepad.xAxis(AnalogPadX);
    Gamepad.yAxis(AnalogPadY);

    Gamepad.write(); //送信
  }
  else if (mode == AnalogZrZmode)
  {
    //アナログスライダーモード
    uint8_t Addrrz = 2500;
    Arrayright[button.VOL_L] > 0 ? AnalogPadz += Addrrz : AnalogPadz += 0;   //左のつまみが右回転
    Arrayright[button.VOL_R] > 0 ? AnalogPadrz += Addrrz : AnalogPadrz += 0; //右のつまみが左回転
    Arrayleft[button.VOL_L] > 0 ? AnalogPadz -= Addrrz : AnalogPadz -= 0;    //左のつまみが左回転
    Arrayleft[button.VOL_R] > 0 ? AnalogPadrz -= Addrrz : AnalogPadrz -= 0;  //右のつまみが右回転

    Gamepad.zAxis(AnalogPadz);
    Gamepad.rzAxis(AnalogPadrz);

    Gamepad.write(); //送信
  }
}

ISR(PCINT0_vect)
{
  unsigned char result[2];
  result[0] = r1.process();
  result[1] = r2.process();

  for (int i = 0; i < (sizeof(result) / sizeof(unsigned char)); i++)
  {

    // Serial.println("right" + String(i) + ":" + Arrayright[i]);
    // Serial.println("left" + String(i) + ":" + Arrayleft[i]);
    if (result[i] == DIR_CCW)
    { //反時計回りに回った時

      Arrayright[i] -= ENCMAX / 2;
      if (Arrayright[i] < 0)
      {
        Arrayleft[i] = ENCMAX;
        Arrayright[i] = 0;
      }
    }
    else if (result[i] == DIR_CW)
    { //時計回りに回った時

      Arrayleft[i] -= ENCMAX / 2;
      if (Arrayleft[i] < 0)
      {
        Arrayleft[i] = 0;
        Arrayright[i] = ENCMAX;
      }
    }
  }
}

void ModeChange()
{
  for (int i = 0; i < (sizeof(button.keymap) / sizeof(char)); i++)
  {

    if (!digitalRead(6) == HIGH && !digitalRead(i) == HIGH)
    {
      ButtonFlag = B1000000 | !digitalRead(i) << i;
    }
  }
  if (ButtonFlag == 0)
  {
    lcd.setCursor(0, 1);
    lcd.print("0000000");
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print(ButtonFlag, BIN);
  }
  // Serial.println(ButtonFlag, BIN);

  switch (ButtonFlag)
  {

  case B1000001:
    ModeCount[keymode]++;
    break;
  case B1000010:
    ModeCount[Mousemode]++;
    break;
  case B1000100:
    ModeCount[AnalogXYmode]++;
    break;
  case B1001000:
    ModeCount[AnalogZrZmode]++;
    break;
  default:
    ModeCount[keymode] = 0;
    ModeCount[Mousemode] = 0;
    ModeCount[AnalogXYmode] = 0;
    ModeCount[AnalogZrZmode] = 0;

    break;
  }

  if (ModeCount[keymode] > MODECOUNTMAX)
  {
    mode = keymode;
  }
  else if (ModeCount[Mousemode] > MODECOUNTMAX)
  {
    mode = Mousemode;
  }
  else if (ModeCount[AnalogXYmode] > MODECOUNTMAX)
  {
    mode = AnalogXYmode;
  }
  else if (ModeCount[AnalogZrZmode] > MODECOUNTMAX)
  {
    mode = AnalogZrZmode;
  }

  ButtonFlag = B00000000;
}
