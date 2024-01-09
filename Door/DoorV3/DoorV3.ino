// WIFI
#include "WiFiEsp.h"
#include <EEPROM.h> 
#include <ArduinoJson.h>

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(4, 5); // RX, TX
#endif

char ssid[] = "ARRIS-25DF";
char pass[] = "1D56D9B1FD31E39C";
int status = WL_IDLE_STATUS;

WiFiEspServer server(80);

// SERVO
#include <Servo.h>
byte servopin = 3;
Servo servo;
bool servoPos = 0;

// RFID
#include <SPI.h>
#include"rfid1.h"
RFID1 rfid;

// SD
#include <SD.h>

// BUTTON
byte btn = 2;

// LED
byte blue = 7;
byte green = 8;
byte red = 9;
unsigned long long timer;

bool enableUser = true;

bool mute = false;

void setup() {
  Serial.begin(115200);

  // SERVO SETUP
  servo.attach(servopin);
  delay(100);
  servo.write(0);
  delay(100);

  // BUTTON
  pinMode(btn, INPUT);
  digitalWrite(btn, HIGH);
  
  // LED
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  // RFID
  rfid.begin(A3, A2, A1, A0, A4, A5); // rfid.begin(IRQ_PIN, SCK_PIN, MOSI_PIN, MISO_PIN, NSS_PIN, RST_PIN)
  delay(100);
  rfid.init();

  // SD
  if (!SD.begin()) {
    Serial.println("SD not found");
    return;
  }
  
  mute = checkCard("mute");
  enableUser = checkCard("user");
  
  // BUZZER
  pinMode(6, OUTPUT);
  buzz(0);
  
  // WIFI SETUP
  Serial1.begin(57600);
  WiFi.init(&Serial1);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("No ESP found");
    while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }
  server.begin();
  buzz(0);
}

void setLED(byte op) {
  if (op == 0) {
    digitalWrite(red, LOW); digitalWrite(green, LOW); digitalWrite(blue, LOW);
    return;
  }
  if (op == 1) {
    digitalWrite(red, HIGH); digitalWrite(green, LOW); digitalWrite(blue, LOW);
  }
  if (op == 2) {
    digitalWrite(red, LOW); digitalWrite(green, HIGH); digitalWrite(blue, LOW);
  }
  if (op == 3) {
    digitalWrite(red, LOW); digitalWrite(green, LOW); digitalWrite(blue, HIGH);
  }
  if (op == 4) {
    digitalWrite(red, HIGH); digitalWrite(green, HIGH); digitalWrite(blue, HIGH);
  }
}

void playSound(int lvl) {
  if (mute) {return;}
  analogWrite(6, lvl);
}

void buzz(byte no) {
  if (no == 0) {
    playSound(600);
    delay(200);
    playSound(900);
    delay(200);
    playSound(0);
    delay(50);
    playSound(100);
    delay(200);
    playSound(0);
  }  
  if (no == 1) {
    setLED(1);
    playSound(20);
    delay(200);
    setLED(0);
    delay(200);
    setLED(1);
    delay(200);
    playSound(0);
    setLED(0);
  }   
  if (no == 2) {
    setLED(2);
    playSound(20);
    delay(75);
    setLED(0);
    playSound(0);
    delay(50);
    setLED(2);
    playSound(20);
    delay(75);
    setLED(0);
    playSound(0);
  } 
}

void operateDoor() {
  delay(50);
  if (!servoPos) { servo.write(180); servoPos = 1; setLED(3); }
  else { servo.write(120); servoPos = 0; setLED(1); }
  delay(100);
  timer = millis() + 1000;
}

bool checkCard(String card) {
  File file = SD.open(card);
    if (file) {
      Serial.println("open");
      char level = file.read();
      Serial.println(level);
      if (level == '0') {
        file.close();
        return false;
      }
      if (level == '2' || enableUser == true && level == '1') {
        file.close();
        return true;
      }
      file.close();
    }
    return false;
}

String readCard(uchar str[MAX_LEN]) {
  setLED(4);
  byte cycles = 0;
  while (cycles < 254) {
    status = rfid.request(PICC_REQIDL, str);
    if (status == MI_OK) {
      status = rfid.anticoll(str);
      if (status == MI_OK) {
        String currCard;
        for(int i=0; i<5; i++){
          currCard += (0x0F & (str[i]>>4));
        }
        return currCard;
      }
    }
    cycles++;
    delay(50);
  }
  return "";
}

void loop() {
  if (digitalRead(btn) == 0) {
    operateDoor();
    delay(200);
  }
  
  if (timer) {
    if (timer >= millis()) {
      setLED(0);
      timer = 0;
    }
  }
  
  // rfid
  uchar status;
  uchar str[MAX_LEN];
  status = rfid.request(PICC_REQIDL, str);
  
  if (status == MI_OK) {
    status = rfid.anticoll(str);
    if (status == MI_OK) {
      String currCard;
      for(int i=0; i<5; i++){
          currCard += (0x0F & (str[i]>>4));
      }
      Serial.println(currCard);
      if (checkCard(currCard)) {
        buzz(2);
        operateDoor();
      } else { buzz(1); }
      
      rfid.halt();
      delay(200);
    }
  }
  
  // wifi
  WiFiEspClient client = server.available();  // listen for clients

  if (client) {
    Serial.println("[server] new client");
    
    String input;
    while (client.connected()) {
      if (client.available()) {
        
        // read data
        char c = client.read();
        input.concat(c);
        
        // response end
        if (input.endsWith("HTTP/1.1")) {
          // get data
          String substr = input.substring(input.indexOf(" /")+2, input.indexOf(" HTTP/1.1"));

          Serial.println("\n[server] revieved: " + substr);
          
          if (substr.indexOf("icon") != -1) {
            Serial.println("\n[server] exit - icon");
          }

          else if (substr == "") {
            sendHttpResponse(client, String(servoPos) + "," + String(enableUser) + "," + String(mute));
          }
          
          else if (substr == "o") {
            sendHttpResponse(client, String(!servoPos));
            operateDoor();
          }

          else if (substr == "r") {
            sendHttpResponse(client, readCard(str));
          }

          else if (substr == "a") {
            String card = readCard(str);
            if (!checkCard(card)) {
              File file = SD.open(card, FILE_WRITE);
              file.write("1");
              file.close();
              sendHttpResponse(client, "1");
            } else {
              sendHttpResponse(client, "0");
            }
          }
          
          else if (substr == "d") {
            String card = readCard(str);
            if (card != "802515") {
              SD.remove(card);
              sendHttpResponse(client, "1");
            } else {
              sendHttpResponse(client, "0");
            }
          }

          else if (substr == "ue") {
            SD.remove("user");
            File file = SD.open("user", FILE_WRITE);
            file.write("2");
            file.close();
            enableUser = true;
            sendHttpResponse(client, "1");
          }

          else if (substr == "ud") {
            SD.remove("user");
            File file = SD.open("user", FILE_WRITE);
            file.write("0");
            file.close();
            enableUser = false;
            sendHttpResponse(client, "1");
          }

          else if (substr == "me") {
            SD.remove("mute");
            File file = SD.open("mute", FILE_WRITE);
            file.write("2");
            file.close();
            mute = true;
            sendHttpResponse(client, "1");
          }

          else if (substr == "md") {
            SD.remove("mute");
            File file = SD.open("mute", FILE_WRITE);
            file.write("0");
            file.close();
            mute = false;
            sendHttpResponse(client, "1");
          }
          
          Serial.println("\n[server] exit - response sent");
          break;
        }
      }
    }
  }
}


// wifi voids
void sendHttpResponse(WiFiEspClient client, String contents) {
  // header
  client.print("HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "\r\n");
  
  client.print(contents);
  // end
  client.println();
  delay(100);
  client.stop();
}
