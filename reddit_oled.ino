#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

char buf[80];
char buf2[80];

// Wifi config
const char* ssid      = "ssid";
const char* password  = "ssid_pass";

// reddit API config
const String userName = "username";
const String urlPrefix = "https://www.reddit.com/user/";
const String urlJSONFunc = "/about.json";

const String url = urlPrefix + userName + urlJSONFunc;

// Request current data every INTERVAL seconds
#define INTERVAL 60

void displayMessage(char* s, char* s2)
{
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_profont12_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(0, 16, s); // write something to the internal memory
  u8g2.setFont(u8g2_font_profont17_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(0, 32, s2); // write something to the internal memory
  u8g2.sendBuffer();         // transfer internal memory to the display
}

void get_karma() {

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

    HTTPClient http;
    http.begin(url, "4F:47:6C:62:B9:96:AA:DD:F5:D3:7B:74:6F:99:53:FC:0E:9D:B2:D9");   // Specify TLS signature
    int httpCode = http.GET();    // Send the request

    if (httpCode > 0) {           // Return less than 1 abnormal

      String payload = http.getString();  // Get the request response payload

      DynamicJsonDocument doc(5500);      // Make sure buffer is big enough, use: https://arduinojson.org/v5/assistant/

      DeserializationError error = deserializeJson(doc, payload);

      if (error)
      {
        Serial.println("Error on deserialize");

        displayMessage("Error on deserialization", "ERROR");
      }
      else
      {

        int comment_karma = doc["data"]["comment_karma"];
        int link_karma = doc["data"]["link_karma"];

        Serial.println(comment_karma);
        Serial.println(link_karma);

        int total_karma = comment_karma + link_karma;

        itoa( total_karma, buf, 10 );

        String userNameLine = userName + "'s karma";

        userNameLine.toCharArray(buf2, 40);

        u8g2.clearBuffer();          // clear the internal memory
        u8g2.setFont(u8g2_font_profont12_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
        u8g2.drawStr(0, 16, buf2); // write something to the internal memory
        u8g2.setFont(u8g2_font_logisoso30_tf);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
        u8g2.drawStr(0, 60, buf); // write something to the internal memory

        u8g2.setFont(u8g2_font_open_iconic_email_2x_t);  // mail
        u8g2.drawStr(100, 60, "@"); // write something to the internal memoryu
        
        
        u8g2.sendBuffer();         // transfer internal memory to the display

        
      }
    }

    http.end();   //Close connection

  }
  else
  {
    Serial.println("Not connected");
    displayMessage("Not connected", "WARNING");
  }
}

void setup(void) {
  u8g2.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.begin(115200);
  Serial.println("\n");

  Serial.print("Connecting to wifi ... ");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");

  String connectedMessage = WiFi.localIP().toString();
  connectedMessage.toCharArray(buf, 25);

  displayMessage(buf, "Connected");
  Serial.println(" Connected.");
  Serial.println(" IP: " + WiFi.localIP().toString());
  delay(2000); // Pause enough time to see the IP address.

  get_karma();
}

unsigned long last_millis = 0;

void loop(void) {

  if (millis() - last_millis >= INTERVAL * 1000)
  {
    get_karma();

    last_millis = millis();
  }
}
