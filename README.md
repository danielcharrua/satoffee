# Satoffee⚡️ (Satoshi + Coffee)

![Satoffee photo](./img/satoffee.jpg)

[👉 Project article (Spanish)](https://danielpcostas.dev/satoffee-lightning-coffee/)

## Install instructions

- Install [Arduino IDE](https://www.arduino.cc/en/software)
- Install ESP32 boards, using [boards manager](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-boards-manager)

![Boards manager](https://user-images.githubusercontent.com/33088785/161862832-1269a12e-16ce-427c-9a92-df3ee573a1fb.png)

- Download this repo
- Copy these [libraries](./libraries) into your Arduino install "libraries" folder
- Open [satoffee.ino](./satoffee/satoffee.ino) file in the Arduino IDE
- Change these with your own values

```c
String ssid = "your-ssid";
String wifiPassword = "your-wifi-password";
String lnbitsServer = "legend.lnbits.com";
String deviceId = "device-id";
String device_lnurlp = "lightning:LNURL...";
```

- Select the correct ESP32 board from tools>board
- Upload to device

Project based on the [bitcoinswitch](https://github.com/lnbits/bitcoinswitch) from Ben Arc.
