/*#include <hidboot.h> /*fler usb enheter*/
/*#include <usbhub.h> /*prata med mus*/
/*#include <SPI.h> /*prata med arduino*/
#include <Mouse.h> /*styra musen*/
#include <U8g2lib.h>
#include <Wire.h>

#define SW_PIN 16

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
/*USB Usb; /*Skapar ett usb objekt */
/*USBHub Hub(&Usb); /*Skapar ett usb-hub objekt med referens och utgångspunkt till usb objektet */
/*HIDBoot < USB_HID_PROTOCOL_KEYBOARD | USB_HID_PROTOCOL_MOUSE Hid> HidKBM(&Usb); /* Ett HIDBoot-objekt som hanterar kommunikation med USB-HID-enheter i det här fallet tangentbordet and musen*/
/*HIDBoot<USB_HID_PROTOCOL_MOUSE>HidMouse(&Usb); /*Ett HIDBoot-obhejt som enbart hanterar kommunikation med musen */

int neg = -127; /*Mouse.move supportar bara 128px per funktionsanrop*/
int pos = 127;

int SCREEN_WIDTH = 960; /*1920/2 - mitten på skärmen*/
int SCREEN_HEIGHT = 540; /*1080/2*/
int middleX = (SCREEN_WIDTH/2);
int middleY = (SCREEN_HEIGHT/2);
int targetY = 0;
int targetX = 0;
bool active = false; /*rör vi musen?*/

/*
int lmb;
int rmb;
int mmb;
*/

/*class mousereport : public MouseReportParser {
  void OnMouseMove(MOUSEINFO * mouseinfo) override { /*vi måste ersätta klassens nuvarande info därav nödvändigt med override*/
    /*if (!active)
    {
      Mouse.move(mouseinfo -> dX, mouseinfo -> dY, 0);
      middleX = constrain(middleX + mouseinfo->dX, 0, SCREEN_WIDTH);
      middleY = constrain(middleY + mouseinfo->dY, 0, SCREEN_HEIGHT);
    }
  }
} mouseParser;*/ /*parser som översätter rådata till data*/

/*
void mousereporter::OnMouseMove (mouseinfo)
{
  Mouse.move(1, 1, 0);
}

void mousereporter::OnLeftButtonUp (mouseinfo)
{
  lmb = 0;
}

void mousereporter::OnLeftButtonDown (mouseinfo)
{
  lmb = 1;
}

void mousereporter::OnRightButtonUp (mouseinfo)
{
  rmb = 0;
}

void mousereporter::OnRightButtonDown (mouseinfo)
{
  rmb = 1;
}

void mousereporter::OnMiddleButtonUp (mouseinfo)
{
  mmb = 0;
}

void mousereporter::OnMiddleButtonDown (mouseinfo)
{
  mmb = 1;
}
*/

void setup() {
  pinMode(SW_PIN, INPUT_PULLUP); // Joystickens knapp är nere
  Serial.begin(9600); /*Använd 151200 bit-rate dvs 1 ms senare i projektet när den faktiskt är användbar*/

  Mouse.begin();
  u8g2.begin();
  Serial.println("OK");
  delay(1000);

  /*while (!Serial); /*Väntar tills datorn faktiskt har öppnat porten*/
  /*if (Usb.Init() != 0){
    Serial.println("Init failed");
    while(1);
  }
  HidMouse.SetReportParser(0, &mouseParser); /*0 för första musen*/
}

void loop() {
  /*Usb.Task();*/
  const int dx = 100;
  const int dy = 100;
  if (digitalRead(SW_PIN) == LOW) { // Tryckt ner
  Mouse.move(dx, dy, 0); // Flytta muspekaren 10 pixlar till höger
  
  Serial.println("Moving");
  Serial.print("X = ");
  Serial.println(dx);
  Serial.print("Y = ");
  Serial.println(dy);

  oledWrite(10, 20, String("X = ") + String(dx));
  oledWrite(10, 35, String("Y = ") + String(dy));

  delay(200); // Liten paus så det inte spammas
}
  static char buffer[16]; /*buffer som vi samlar inputen i och skickar till processInput */
  static int index = 0; /*räknare som visar vart i bufferten nästa tecken ska hamna*/
  while (Serial.available()){ /*så länge det finns minst ett tecken att läsa från datorn*/
    char c = Serial.read(); /*plocka ut tecknet och lägg i c*/
    if (c == '\n'){ /*om det är slutet på raden*/
      buffer[index] = '\0'; /*gör om rådatan till sträng*/
      processInput(buffer); /*passerar in buffern i funktionen som utför draget*/
      index = 0; /*börja om buffern för nästa rad*/
    }
    else if (index < sizeof(buffer) - 1) { /*om buffern inte är full*/
      buffer[index] = c; /*lägg c på plats index*/
      index += 1; /*öka index*/
    }
  }
}

void oledWrite(int x, int y, String text){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(x, y, text.c_str());
  u8g2.sendBuffer();
}

void processInput(char *line) {
  int x = 0, y = 0;
  if (sscanf(line, "%d %d", &x, &y) == 2) { /*taget från google läser av texten och kollar om den hittar 2 heltal som motsvara y och x värderna */
    x = constrain(x, neg, pos); /*constrainar talet till max 127 minst -127*/
    y = constrain(y, neg, pos);
    active = true;
    Mouse.move(x,y,0); /*inget på z-axeln*/
    active = false;
    Serial.println("OK");
  } else {
    Serial.println("ERR");
  }
}
