// display //
#include <LiquidCrystal.h>
byte rs = 4; byte en = 5; byte d4 = 6;
byte d5 = 7; byte d6 = 8; byte d7 = 9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte degreessymbol[8] = {
  0b01100,
  0b10010,
  0b10010,
  0b01100,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
};

// btn and poti //
byte btn = 13; bool btnstate; bool oldbtnstate = 1;

byte poti1 = A5; int poti1state; int rotation;
byte poti2 = A6; int poti2state; int totaltime;

// motor //
byte ronpin = 10; byte rmovepin = 11; byte rdirpin = 12;
unsigned long distance = 0;

bool dir = 1;

long stepsrotation;
unsigned long dt;

// menu //
bool started = false;
long progressbar; long originalpbar;

void setup() {
  started = false;
  Serial.begin(115200);
  lcd.clear();
  lcd.createChar(0, degreessymbol);
  
  pinMode(poti1, INPUT); pinMode(poti2, INPUT);
  pinMode(btn, INPUT); digitalWrite(btn, HIGH);
  
  pinMode(rmovepin, OUTPUT); digitalWrite(rmovepin, LOW);
  pinMode(rdirpin, OUTPUT); digitalWrite(rdirpin, dir);
  pinMode(ronpin, OUTPUT); digitalWrite(ronpin, LOW);
  
  while (!started) {
    btnstate = digitalRead(btn);
    if (btnstate == 1 && oldbtnstate == 0) {
      started = true;
      break;
    }
    poti1state = analogRead(poti1); rotation = round((poti1state/1.42)/10)*10;
    poti2state = analogRead(poti2);

    if (poti2state < 61*5) {
      totaltime = round(poti2state/5);
      lcd.setCursor(8, 0); lcd.print("T:"); lcd.setCursor(11, 0); lcd.print(totaltime); lcd.print("s");
    }
    else {
      totaltime = round((poti2state-61*5)*6)+1;
      lcd.setCursor(8, 0); lcd.print("T:"); lcd.setCursor(11, 0); lcd.print(totaltime/60); lcd.print("m");
    }
    
    lcd.setCursor(0, 0); lcd.print("R:"); lcd.setCursor(3, 0); lcd.print(rotation); lcd.write(byte(0));

    delay(100);
    lcd.clear();
    oldbtnstate = btnstate;
  }
  
  stepsrotation = rotation*9;
  dt = totaltime*1000./stepsrotation;
  progressbar = stepsrotation/16; originalpbar = progressbar;
}

void loop() {
  digitalWrite(rmovepin, HIGH);
  delay(dt/2);
  digitalWrite(rmovepin, LOW);
  delay(dt/2);

  distance++;

  if (distance == progressbar) {
    lcd.print("-");
    progressbar += originalpbar;
  }
  
  if (distance == stepsrotation) {
    distance = 0;
    setup();
  }
  
}
