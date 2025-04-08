#include <hidboot.h>
#include <usbhub.h>
#include <Wire.h>
#include <mouse.h>

USB Usb; /*Skapar ett usb objekt */
USBHub Hub(&Usb); /*Skapar ett usb-hub objekt med referens och utgångspunkt till usb objektet */
HIDBoot < USB_HID_PROTOCOL_KEYBOARD | USB_HID_PROTOCOL_MOUSE Hid> HidKBM(&Usb) /* Ett HIDBoot-objekt som hanterar kommunikation med USB-HID-enheter i det här fallet tangentbordet and musen*/
HIDBoot < USB_HID_PROTOCOL_MOUSE Hid> HidMouse(&Usb) /*Ett HIDBoot-obhejt som enbart hanterar kommunikation med musen */

int neg = -127
int pos = 127
int neg2 = -89
int pos2 = 89

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
