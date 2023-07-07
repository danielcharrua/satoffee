#include <WebSocketsClient.h>
#include <qrcoded.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <OneButton.h>
#include "pin_config.h"
#include "config.h"

TaskHandle_t Task1;

String payloadStr;
bool paid;

byte testState = 0;

// Buttons
OneButton leftButton(PIN_BUTTON_1, true);
OneButton rightButton(PIN_BUTTON_2, true);

// Display
TFT_eSPI tft = TFT_eSPI();
#define GFXFF 1
int x = 85;
int y = 160;
#include "display.h"

WebSocketsClient webSocket;

void setup()
{
  Serial.begin(115200);

  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  tft.init();
  tft.setRotation(0);
  
  startupScreen();

  WiFi.begin(ssid.c_str(), wifiPassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    delay(500);
  }

  Serial.println(lnbitsServer + "/api/v1/ws/" + deviceId);
  webSocket.beginSSL(lnbitsServer, 443, "/api/v1/ws/" + deviceId);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(1000);

  leftButton.attachClick([]() {
    testMode();
  });

  rightButton.attachClick([]() {
    showHelp();
  });

  xTaskCreatePinnedToCore(
    Task1code, /* Function to implement the task */
    "Task1", /* Name of the task */
    10000,  /* Stack size in words */
    NULL,  /* Task input parameter */
    0,  /* Priority of the task */
    &Task1,  /* Task handle. */
    0); /* Core where the task should run */
}

void Task1code( void * pvParameters ){

  for(;;){
    leftButton.tick();
    rightButton.tick();
  }

}

void loop() {

  while(WiFi.status() != WL_CONNECTED){
    Serial.println("Failed to connect");
    delay(500);
  }
  payloadStr = "";
  delay(2000);
  showQRScreen();
  while(paid == false){
    webSocket.loop();
    if(paid){
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
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

//////////////////WEBSOCKET///////////////////

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);

                // send message to server when Connected
                webSocket.sendTXT("Connected");
            }
            break;
        case WStype_TEXT:
            payloadStr = (char*)payload;
            paid = true;
    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
    }
}

void testMode() {
  pinMode(44, OUTPUT);
  switch(testState){
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
      digitalWrite(44, LOW);    // Relay ON
      testState++;
    break;
    case 6:
      thankYouScreen();
      digitalWrite(44, HIGH);   // Relay OFF
      testState = 0;            // Reset state
    break;               
  }
}

void showHelp() {
    stepOneScreen();
    delay(3000);
    stepTwoScreen();
    delay(3000);
    stepThreeScreen();
    delay(3000);
    showQRScreen();
}
