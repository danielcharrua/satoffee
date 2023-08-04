#include <WiFi.h>
#include <WebSocketsClient.h>
#include <OneButton.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "FFat.h"

#include "PinConfig.h"
#include "Display.h"
#include "SerialConfig.h"

#define FORMAT_ON_FAIL true
#define PARAM_FILE "/config.json"

TaskHandle_t Task1;

String ssid;
String wifiPassword;
String switchStr;
const char *lightningPrefix = "LIGHTNING:";
const char *lnurl;
char lightning[300];

String payloadStr;
String lnbitsServer;
String deviceId;
bool paid;
byte testState = 0;
bool inConfigMode = false;
// int relayPin;

// Buttons
OneButton leftButton(PIN_BUTTON_1, true);
OneButton rightButton(PIN_BUTTON_2, true);

WebSocketsClient webSocket;

void setup()
{
  Serial.setRxBufferSize(1024);
  Serial.begin(115200);

  int timer = 0;

  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  initDisplay();
  startupScreen();

  FFat.begin(FORMAT_ON_FAIL);
  readFiles(); // get the saved details and store in global variables

  WiFi.begin(ssid.c_str(), wifiPassword.c_str());
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED && timer < 16000)
  {
    delay(1000);
    Serial.print(".");
    timer = timer + 1000;
    if (timer > 15000)
    {
      Serial.println("");
      configMode();
    }
  }
  Serial.println("");

  webSocket.beginSSL(lnbitsServer, 443, "/api/v1/ws/" + deviceId);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(1000);

  leftButton.setPressTicks(3000);
  leftButton.attachClick(testMode);
  leftButton.attachLongPressStart(configMode);
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

void readFiles()
{
  File paramFile = FFat.open(PARAM_FILE, "r");
  if (paramFile)
  {
    StaticJsonDocument<1500> doc;
    DeserializationError error = deserializeJson(doc, paramFile.readString());

    const JsonObject maRoot0 = doc[0];
    const char *maRoot0Char = maRoot0["value"];
    ssid = maRoot0Char;
    Serial.println("SSID: " + ssid);

    const JsonObject maRoot1 = doc[1];
    const char *maRoot1Char = maRoot1["value"];
    wifiPassword = maRoot1Char;
    Serial.println("Wifi pass: " + wifiPassword);

    const JsonObject maRoot2 = doc[2];
    const char *maRoot2Char = maRoot2["value"];
    switchStr = maRoot2Char;
    lnbitsServer = switchStr.substring(5, switchStr.length() - 33);
    deviceId = switchStr.substring(switchStr.length() - 22);

    Serial.println("Socket: " + switchStr);
    Serial.println("LNbits server: " + lnbitsServer);
    Serial.println("Switch device ID: " + deviceId);

    const JsonObject maRoot3 = doc[3];
    const char *maRoot3Char = maRoot3["value"];
    lnurl = maRoot3Char;

    // copy values into lightning char
    strcpy(lightning, lightningPrefix);
    strcat(lightning, lnurl);

    Serial.print("LNURL: ");
    Serial.println(lnurl);
    Serial.print("QR: ");
    Serial.println(lightning);
  }
  else
  {
    Serial.println("Config file not found");
  }
  paramFile.close();
}

//////////////////WEBSOCKET///////////////////

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  if (inConfigMode == false)
  {
    switch (type)
    {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected!");
      break;
    case WStype_CONNECTED:
    {
      Serial.printf("WebSocket connected to url: %s\n", payload);

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
}

void configMode()
{
  inConfigMode = true;
  Serial.println("Config mode triggered");
  configModeScreen();
  configOverSerialPort();
}

void testMode()
{
  // pinMode(relayPin, OUTPUT);
  switch (testState)
  {
  case 0:
    startupScreen();
    testState++;
    break;
  case 1:
    configModeScreen();
    testState++;
    break;
  case 2:
    showQRScreen();
    testState++;
    break;
  case 3:
    stepOneScreen();
    testState++;
    break;
  case 4:
    stepTwoScreen();
    testState++;
    break;
  case 5:
    stepThreeScreen();
    testState++;
    break;
  case 6:
    switchedOnScreen();
    // digitalWrite(relayPin, LOW); // Relay ON
    testState++;
    break;
  case 7:
    thankYouScreen();
    // digitalWrite(relayPin, HIGH); // Relay OFF
    testState = 0; // Reset state
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