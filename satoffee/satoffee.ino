#include <WebSocketsClient.h>
#include <OneButton.h>
#include "PinConfig.h"
#include "WManager.h"
#include "Display.h"

TaskHandle_t Task1;

String payloadStr;
bool paid;
byte testState = 0;
extern char serverFull[120];
extern int relayPin;
String lnbitsServer;
String deviceId;

// Buttons
OneButton leftButton(PIN_BUTTON_1, true);
OneButton rightButton(PIN_BUTTON_2, true);

WebSocketsClient webSocket;

void setup()
{
  Serial.begin(115200);

  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  initScreen();
  startupScreen();
  init_WifiManager();

  lnbitsServer = String(serverFull).substring(5, String(serverFull).length() - 33);
  deviceId = String(serverFull).substring(String(serverFull).length() - 22);
  Serial.print("lnbitsServer: ");
  Serial.println(lnbitsServer);
  Serial.print("deviceId: ");
  Serial.println(deviceId);

  Serial.println(lnbitsServer + "/api/v1/ws/" + deviceId);
  webSocket.beginSSL(lnbitsServer, 443, "/api/v1/ws/" + deviceId);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(1000);

  leftButton.attachClick(testMode);

  rightButton.attachClick(showHelp);

  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1",   /* Name of the task */
      10000,     /* Stack size in words */
      NULL,      /* Task input parameter */
      0,         /* Priority of the task */
      &Task1,    /* Task handle. */
      0);        /* Core where the task should run */
}

void Task1code(void *pvParameters)
{
  for (;;)
  {
    leftButton.tick();
    rightButton.tick();
    wifiManagerProcess();
  }
}

void loop()
{
  payloadStr = "";
  delay(2000);
  showQRScreen();

  while (paid == false)
  {
    webSocket.loop();
    if (paid)
    {
      switchedOnScreen();
      pinMode(getValue(payloadStr, '-', 0).toInt(), OUTPUT);
      digitalWrite(getValue(payloadStr, '-', 0).toInt(), HIGH);
      delay(getValue(payloadStr, '-', 1).toInt());
      digitalWrite(getValue(payloadStr, '-', 0).toInt(), LOW);
    }
  }
  Serial.println("Paid");
  thankYouScreen();
  paid = false;
  delay(2000);
}

//////////////////HELPERS///////////////////

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

//////////////////WEBSOCKET///////////////////

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
  {
    Serial.printf("[WSc] Connected to url: %s\n", payload);

    // send message to server when Connected
    webSocket.sendTXT("Connected");
  }
  break;
  case WStype_TEXT:
    payloadStr = (char *)payload;
    paid = true;
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
    break;
  }
}

void testMode()
{
  pinMode(relayPin, OUTPUT);
  switch (testState)
  {
  case 0:
    startupScreen();
    testState++;
    break;
  case 1:
    showQRScreen();
    testState++;
    break;
  case 2:
    stepOneScreen();
    testState++;
    break;
  case 3:
    stepTwoScreen();
    testState++;
    break;
  case 4:
    stepThreeScreen();
    testState++;
    break;
  case 5:
    switchedOnScreen();
    digitalWrite(relayPin, LOW); // Relay ON
    testState++;
    break;
  case 6:
    thankYouScreen();
    digitalWrite(relayPin, HIGH); // Relay OFF
    testState = 0;                // Reset state
    break;
  }
}

void showHelp()
{
  stepOneScreen();
  delay(3000);
  stepTwoScreen();
  delay(3000);
  stepThreeScreen();
  delay(3000);
  showQRScreen();
}