#include <M5StickCPlus.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "MHZ19_uart.h"
#include "config.h"

const int rx_pin = 36;
const int tx_pin = 26;

MHZ19_uart mhz19;

void setup(){
  M5.begin();

  pinMode(36, INPUT);
  gpio_pulldown_dis(GPIO_NUM_25);
  gpio_pullup_dis(GPIO_NUM_25);

  Serial.begin(9600);
  mhz19.begin(rx_pin, tx_pin);

  M5.Lcd.setRotation(3);
  M5.Lcd.setTextSize(4);

  M5.Lcd.println("Connecting to the network...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  M5.Lcd.println(WiFi.localIP());
  delay(3000);
}

int fetch(int *co2ppm, int *hashrate) {
  int _co2ppm = mhz19.getCO2PPM();
  if (_co2ppm < 0) {
    return -1;
  }
  *co2ppm = _co2ppm;

#if 0
  HTTPClient http;
  http.begin(NICEHASH_SERVER);
  http.addHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
  http.addHeader("Accept-Language", "en-US");
  http.addHeader("Accept-Encoding", "gzip, deflate");
  http.useHTTP10(true);
  int httpCode = http.GET();
  Serial.println(httpCode);
  if (httpCode <= 0) {
    return -1;
  }
  if (httpCode != HTTP_CODE_OK) {
    Serial.println(http.errorToString(httpCode));
    Serial.println(http.getString());
    return -1;
  }
  String hashrateStr = http.getString();
  *hashrate = hashrateStr.toInt();
#endif

  return 0;
}

void loop() {
  M5.update();

  M5.Lcd.fillScreen(BLACK);

  int co2ppm = 0, hashrate = 0;
  if (fetch(&co2ppm, &hashrate) == 0) {
    
    //M5.Lcd.setTextFont(7);
    //M5.Lcd.setTextSize(2);

    M5.Lcd.drawString(String(co2ppm), 0, 0);
    M5.Lcd.drawString(String(hashrate), 0, 100);
  } else {
    M5.Lcd.drawString("Fetch error!", 0, 0);
  }

  delay(3000);
}
