#include <HID-Project.h>
#include <HID-Settings.h>
#include <Rotary.h>

class Button
{
public:
  const char keymap[6] = {'s', 'd', 'k', 'l', 'c', 'm' /*6:Enter 7:ESC*/};
  const char VOL_L = 0, VOL_R = 1;
  const char VOL_LL = 'q', VOL_LR = 'w', VOL_RL = 'o', VOL_RR = 'p';

private:
  ;
};

Rotary r1 = Rotary(8, 9);
Rotary r2 = Rotary(10, 11);
volatile int posision[2] = {0, 0};
volatile int Arrayright[2] = {0, 0};
volatile int Arrayleft[2] = {0, 0};
volatile int MAX = 10;
int16_t AnalogPadX = 0;
int16_t AnalogPadY = 0;
uint16_t AnalogPadz = 0;
uint16_t AnalogPadrz = 0;

int Mode;
int ModeCount[7];
Button button;

void setup()
{

  Serial.begin(9600);
  Gamepad.begin();
  r1.begin();
  r2.begin();
  for (int i = 0; i <= 13; i++)
  { // 13pinすべてアップ
    pinMode(i, INPUT_PULLUP);
  }
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT4) | (1 << PCINT5) | (1 << PCINT7) | (1 << PCINT6);
  sei();
  Mode = 0;
}

void loop()
{
  keyFunc();
  ReduseValue();
  ModeChange();
  //  Serial.println("sizeof int :" + String(sizeof(int)));
  Serial.println("Mode:" + String(Mode));
}
void ReduseValue()
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
  if (Mode == 0) //キーボードモード
  {
    Arrayright[button.VOL_L] > 0 ? NKROKeyboard.press(button.VOL_LR) : NKROKeyboard.release(button.VOL_LR); //左のつまみが右回転
    Arrayright[button.VOL_R] > 0 ? NKROKeyboard.press(button.VOL_RR) : NKROKeyboard.release(button.VOL_RR); //右のつまみが右回転
    Arrayleft[button.VOL_L] > 0 ? NKROKeyboard.press(button.VOL_LL) : NKROKeyboard.release(button.VOL_LL);  //左のつまみが左回転
    Arrayleft[button.VOL_R] > 0 ? NKROKeyboard.press(button.VOL_RL) : NKROKeyboard.release(button.VOL_RL);  //右のつまみが左回転
  }
  else if (Mode == 1) //マウス座標モード
  {
    int pos = 30;
    Arrayright[button.VOL_L] > 0 ? Mouse.move(pos, 0) : Mouse.move(0, 0); //左のつまみが右回転
    Arrayright[button.VOL_R] > 0 ? Mouse.move(0, pos) : Mouse.move(0, 0); //右のつまみが左回転
    Arrayleft[button.VOL_L] > 0 ? Mouse.move(-pos, 0) : Mouse.move(0, 0); //左のつまみが左回転
    Arrayleft[button.VOL_R] > 0 ? Mouse.move(0, -pos) : Mouse.move(0, 0); //右のつまみが右回転
  }
  else if (Mode == 2) //アナログスティックモード
  {
    Arrayright[button.VOL_L] > 0 ? AnalogPadX += 2500 : AnalogPadX += 0; //左のつまみが右回転
    Arrayright[button.VOL_R] > 0 ? AnalogPadY += 2500 : AnalogPadY += 0; //右のつまみが左回転
    Arrayleft[button.VOL_L] > 0 ? AnalogPadX -= 2500 : AnalogPadX -= 0;  //左のつまみが左回転
    Arrayleft[button.VOL_R] > 0 ? AnalogPadY -= 2500 : AnalogPadY -= 0;  //右のつまみが右回転

    Gamepad.xAxis(AnalogPadX);
    Gamepad.yAxis(AnalogPadY);

    Gamepad.write();
  }
  else if (Mode == 3) //アナログスライダーモード
  {

    Arrayright[button.VOL_L] > 0 ? AnalogPadz += 2500 : AnalogPadz += 0;   //左のつまみが右回転
    Arrayright[button.VOL_R] > 0 ? AnalogPadrz += 2500 : AnalogPadrz += 0; //右のつまみが左回転
    Arrayleft[button.VOL_L] > 0 ? AnalogPadz -= 2500 : AnalogPadz -= 0;    //左のつまみが左回転
    Arrayleft[button.VOL_R] > 0 ? AnalogPadrz -= 2500 : AnalogPadrz -= 0;  //右のつまみが右回転

    Gamepad.zAxis(AnalogPadz);
    Gamepad.rzAxis(AnalogPadrz);

    Gamepad.write();
  }
}

ISR(PCINT0_vect)
{
  unsigned char result[2];
  result[0] = r1.process();
  result[1] = r2.process();
  for (int i = 0; i <= 2; i++)
  {
    if (result[i] == DIR_CCW)
    { //反時計回りに回った時
      Arrayright[i] -= MAX / 2;
      if (Arrayright[i] < 0)
      {
        Arrayleft[i] = MAX;
        Arrayright[i] = 0;
      }
    }
    else if (result[i] == DIR_CW)
    { //時計回りに回った時
      Arrayleft[i] -= MAX / 2;
      if (Arrayleft[i] < 0)
      {
        Arrayleft[i] = 0;
        Arrayright[i] = MAX;
      }
    }
  }
}
void ModeChange()
{
  Serial.println(ModeCount[0]);
  if (!digitalRead(6) == HIGH)
  {
    if (!digitalRead(0) == HIGH && !digitalRead(1) == LOW && !digitalRead(2) == LOW)
    {
      ModeCount[0]++;
      if (ModeCount[0] > 250)
      {

        Mode = 0;
      }
    }
    else if (!digitalRead(0) == LOW && !digitalRead(1) == HIGH && !digitalRead(2) == LOW)
    {
      ModeCount[1]++;
      if (ModeCount[1] > 250)
      {

        Mode = 1;
      }
    }
    else if (!digitalRead(0) == LOW && !digitalRead(1) == LOW && !digitalRead(2) == HIGH)
    {
    }
    else
    {
      ModeCount[0] = 0;
    }
  }
}
