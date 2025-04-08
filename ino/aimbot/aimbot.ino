#include <hidboot.h>
#include <usbhub.h>
#include <Wire.h>
#include <mouse.h>

USB Usb; /*Skapar ett usb objekt */
USBHub Hub(&Usb); /*Skapar ett usb-hub objekt med referens och utgångspunkt till usb objektet */
HIDBoot < USB_HID_PROTOCOL_KEYBOARD | USB_HID_PROTOCOL_MOUSE Hid> HidKBM(&Usb); /* Ett HIDBoot-objekt som hanterar kommunikation med USB-HID-enheter i det här fallet tangentbordet and musen*/
HIDBoot < USB_HID_PROTOCOL_MOUSE Hid> HidMouse(&Usb); /*Ett HIDBoot-obhejt som enbart hanterar kommunikation med musen */

int neg = -127;
int pos = 127;

int lmb;
int rmb;
int mmb;

class mousereport : public mousereporter {
  void OnMousemove(mouseinfo);
  void OnLeftButtonUp();
  void OnLeftButtonDown();
  void OnRightButtonUp();
  void OnRightButtonDown();
  void OnMiddleButtonUp();
  void OnMiddleButtonDown();
}

void mousereporter::OnMouseMove (mouseinfo)
{
  /*SÄTT MOUSE MOVEMENTS*/
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

void setup() {
  Mouse.begin();
  Serial.begin(9600);
  Usb.Init();
  HidKBM. 
  HidMouse. 
}

void loop() {
  // put your main code here, to run repeatedly:

}
