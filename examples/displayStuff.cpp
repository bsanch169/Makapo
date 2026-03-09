#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>

/* Constructor */
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(18, 17, U8X8_PIN_NONE);

/* u8x8.begin() is required and will sent the setup/init sequence to the display */
void setup(void)
{
  u8x8.begin();
}

void loop(void)
{
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,0,"Hello World!");
  delay(1000);
}