#include <qrcoded.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "display.h"

TFT_eSPI tft = TFT_eSPI();
#define GFXFF 1
int x = 85;
int y = 160;

extern String lnurl;
char *lightningPrefix = "lightning:";

void initDisplay()
{
  tft.init();
  tft.setRotation(0);
}

// Startup
void startupScreen()
{
  tft.fillScreen(TFT_WHITE);

  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(10);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("SA", x + 5, y - 85, GFXFF);
  tft.drawString("TO", x + 5, y - 35, GFXFF);
  tft.drawString("FF", x + 5, y + 15, GFXFF);
  tft.drawString("EE", x + 5, y + 65, GFXFF);
  tft.setTextSize(2);
  tft.drawString("v1.0.0", x + 3, y + 115, GFXFF);
  tft.drawString("@danielpcostas", x + 1, y + 135, GFXFF);
}

// Config Mode Screen
void configModeScreen()
{
  tft.fillScreen(TFT_WHITE);

  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(10);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("C", x + 5, y - 70, GFXFF);

  tft.fillRect(15, 165, 140, 135, TFT_BLACK);
  tft.setTextDatum(ML_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("SERIAL", x - 55, y + 40, GFXFF);
  tft.drawString("CONFIG", x - 55, y + 70, GFXFF);
  tft.drawString("MODE", x - 55, y + 100, GFXFF);
}

// Step one
void stepOneScreen()
{
  tft.fillScreen(TFT_WHITE);

  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(10);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("1", x + 5, y - 70, GFXFF);

  tft.fillRect(15, 165, 140, 135, TFT_BLACK);
  tft.setTextDatum(ML_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("OPEN &", x - 55, y + 40, GFXFF);
  tft.drawString("INSERT", x - 55, y + 70, GFXFF);
  tft.drawString("POD", x - 55, y + 100, GFXFF);
}

// Step two
void stepTwoScreen()
{
  tft.fillScreen(TFT_WHITE);

  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(10);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("2", x + 5, y - 70, GFXFF);

  tft.fillRect(15, 165, 140, 135, TFT_BLACK);
  tft.setTextDatum(ML_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("PLACE", x - 55, y + 40, GFXFF);
  tft.drawString("CUP &", x - 55, y + 70, GFXFF);
  tft.drawString("CLOSE", x - 55, y + 100, GFXFF);
}

// Step three
void stepThreeScreen()
{
  tft.fillScreen(TFT_WHITE);

  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(10);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("3", x + 5, y - 70, GFXFF);

  tft.fillRect(15, 165, 140, 135, TFT_BLACK);
  tft.setTextDatum(ML_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("PUSH", x - 55, y + 40, GFXFF);
  tft.drawString("COFFEE", x - 55, y + 70, GFXFF);
  tft.drawString("BUTTON", x - 55, y + 100, GFXFF);
}

// Switched ON screen
void switchedOnScreen()
{
  tft.fillScreen(TFT_WHITE);

  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(10);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("SA", x + 5, y - 85, GFXFF);
  tft.drawString("TO", x + 5, y - 35, GFXFF);
  tft.drawString("FF", x + 5, y + 15, GFXFF);
  tft.drawString("EE", x + 5, y + 65, GFXFF);
  tft.setTextSize(3);
  tft.drawString("TIME", x + 3, y + 105, GFXFF);
}

// Thank you
void thankYouScreen()
{
  tft.fillScreen(TFT_WHITE);

  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(10);
  tft.setTextColor(TFT_BLACK);
  tft.drawString(";)", x + 5, y - 70, GFXFF);

  tft.fillRect(15, 165, 140, 135, TFT_BLACK);
  tft.setTextDatum(ML_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("THANK", x - 55, y + 40, GFXFF);
  tft.drawString("YOU", x - 55, y + 70, GFXFF);
  tft.drawString("HUMAN", x - 55, y + 100, GFXFF);
}

// Show QR
void showQRScreen()
{
  String qrCodeData;
  qrCodeData = strcat(lightningPrefix, lnurl.c_str());
  tft.setTextDatum(ML_DATUM);
  tft.fillScreen(TFT_WHITE);
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.fillRect(15, 165, 140, 135, TFT_BLACK);
  tft.drawString("COFFEE", x - 55, y + 40, GFXFF);
  tft.drawString("FOR", x - 55, y + 70, GFXFF);
  tft.drawString("SATS", x - 55, y + 100, GFXFF);

  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("HELP", x + 35, y + 150, GFXFF);

  qrCodeData.toUpperCase();
  const char *qrDataChar = qrCodeData.c_str();
  QRCode qrcoded;
  uint8_t qrcodeData[qrcode_getBufferSize(20)];
  qrcode_initText(&qrcoded, qrcodeData, 7, 0, qrDataChar);
  for (uint8_t y = 0; y < qrcoded.size; y++)
  {
    // Each horizontal module
    for (uint8_t x = 0; x < qrcoded.size; x++)
    {
      if (qrcode_getModule(&qrcoded, x, y))
      {
        tft.fillRect(17 + 3 * x, 15 + 3 * y, 3, 3, TFT_BLACK);
      }
      else
      {
        tft.fillRect(17 + 3 * x, 15 + 3 * y, 3, 3, TFT_WHITE);
      }
    }
  }
}