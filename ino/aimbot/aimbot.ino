/*#include <hidboot.h> /*fler usb enheter*/
/*#include <usbhub.h> /*prata med mus*/
/*#include <SPI.h> /*prata med arduino*/
#include <Mouse.h> /*styra musen*/
#include <U8g2lib.h> /*oled-grafiken*/
#include <Wire.h> /*kommunicera med oled-skärmen*/
#define SW_PIN 16 /*joystickens sw pin till raspberrys GP16*/
#define X_PIN        A0
#define Y_PIN        A1
#define RAW_DEADZONE 30 /*alla rörelser som är mer eller mindre 30 från joystickens mittpunkt kommer att ignoreras (def: (0-1023))*/
#define SENSITIVITY  0.5
int centerRawX, centerRawY; /*medelvärde för x och y i joystickens viloläge*/

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE); /*initerar skärmen*/
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

void setup() {
  pinMode(SW_PIN, INPUT_PULLUP); // Joystickens knapp är nere
  Serial.begin(115200); /*Använd 151200 bit-rate dvs 1 ms senare i projektet när den faktiskt är användbar*/

  Mouse.begin();
  u8g2.begin();

  int sumX = 0, sumY = 0; /*summan för alla x och y i joystickens viloläge*/
  for (int i = 0; i < 50; i++) { /*50 gånger räcker*/
    sumX += analogRead(X_PIN);
    sumY += analogRead(Y_PIN);
    delay(5);
  }
  centerRawX = sumX / 50;
  centerRawY = sumY / 50;

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
  /*const int dx = 500;
  const int dy = 0;
  if (digitalRead(SW_PIN) == LOW) { // Tryckt ner
  /*Mouse.move(dx, dy, 0); // Flytta muspekaren x pixlar till höger/vänster och y pixlar upp/ner*/
  
  /*Serial.println("Moving");
  Serial.print("X = ");
  Serial.println(dx);
  Serial.print("Y = ");
  Serial.println(dy);

  oledWrite(10, 25, String("X = ") + String(dx), 10, 45, String("Y = ") + String(dy));

  delay(200); // Liten paus så det inte spammas
}*/

  if (digitalRead(SW_PIN) == HIGH) {
    int rawX = analogRead(X_PIN) - centerRawX;
    int rawY = analogRead(Y_PIN) - centerRawY;

    if (abs(rawX) < RAW_DEADZONE) rawX = 0;
    if (abs(rawY) < RAW_DEADZONE) rawY = 0;

    if (rawX != 0 || rawY != 0) {
      int joyX = map(rawX, -centerRawX, 1023 - centerRawX, neg, pos); /*x inmin inmax outmin outmax*/
      int joyY = map(rawY, -centerRawY, 1023 - centerRawY, neg, pos);

      joyX = int(joyX * SENSITIVITY);
      joyY = int(joyY * SENSITIVITY);

      Mouse.move(joyX, -joyY, 0);
      oledWrite(10, 30, "Moving", 0, 0, "");
    }
  }

  if (digitalRead(SW_PIN) == LOW) {
    oledWrite(10, 30, "AIMBOT", 0, 0, "");
    Serial.println("Moving");
  }

  static char buffer[16]; /*buffer som vi samlar inputen i och skickar till processInput */
  static int index = 0; /*räknare som visar vart i bufferten nästa tecken ska hamna*/
  while (Serial.available()){ /*så länge det finns minst ett tecken att läsa från python skriptet*/
    char c = Serial.read(); /*plocka ut tecknet och lägg i c (behöver inte index här eftersom Serial.read har inbyggd tail)*/
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

void oledWrite(int x1, int y1, String text1, int x2, int y2, String text2){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(x1, y1, text1.c_str());
  u8g2.drawStr(x2, y2, text2.c_str());
  u8g2.sendBuffer();
}

void processInput(char line[]) { /*line kommer från processInput(buffer); i loop*/
  int x = 0, y = 0;
  if (sscanf(line, "%d %d", &x, &y) == 2) { /*taget från google läser av texten och kollar om den hittar 2 heltal som motsvarar y och x värderna */
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
