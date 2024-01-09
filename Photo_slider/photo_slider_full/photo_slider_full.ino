// lcd //
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

// motor //
byte onpin = 10; // (en+ / orange cable)
byte mmovepin = 2; byte rmovepin = 11; // (CLK+ / green cable)
byte mdirpin = 3; byte rdirpin = 12; // (CW+ / yellow cable)

// btn and poti //
byte btn = 13; bool btnstate; bool oldbtnstate = 1;

byte poti1 = A5; int poti1state;
byte poti2 = A6; int poti2state;

// constants // 
const long len = 58000; const byte lentime = 58;

// movement vars //
bool mdir = 1; bool rdir = 1;
unsigned long distance = 0;
float dt; unsigned long originalspeed;
bool moving = false;

int stopplace = 10000; byte slowdown;

// rotation vars //
unsigned long stepsperrotation; unsigned long osprot;

// setup vars //
unsigned long totaltime = 10;
long rotation;
int repeat;

void settings() {
  while (true) {
    btnstate = digitalRead(btn);
    if (btnstate == 1 && oldbtnstate == 0) {
      dt = totaltime*1000;
      dt = dt / lentime*1. - 200;
      originalspeed = dt;

      if (rotation < 0) rdir = 1; else rdir = 0;
      stepsperrotation = len / (abs(rotation) * 8.89 ); osprot = stepsperrotation;
      digitalWrite(rdirpin, rdir);
      
      break;
    }
    poti1state = analogRead(poti1); rotation = (round((poti1state/1.42)/10)*10) - 360;
    poti2state = analogRead(poti2);

    // rotation //
    lcd.setCursor(0, 0); lcd.print("Rotation: "); lcd.print(rotation); lcd.write(byte(0));

    // time //
    lcd.setCursor(0, 1); lcd.print("Time: ");
    if (poti2state < 61*5) {
      totaltime = round(poti2state/5);
      lcd.print(totaltime); lcd.print(" seconds");
    }
    else {
      totaltime = round((poti2state-61*5)/5)*60;
      lcd.print(totaltime/60); lcd.print(" mins");
    }
    
    delay(100);
    lcd.clear();
    oldbtnstate = btnstate;
  }
  delay(500);
  while (true) {
    btnstate = digitalRead(btn);
    if (btnstate == 1 && oldbtnstate == 0) {
      dt = totaltime*1000;
      dt = dt / lentime*1.;
      originalspeed = dt;

      if (rotation < 0) rdir = 1; else rdir = 0;
      stepsperrotation = len / (abs(rotation) * 8.89 ); osprot = stepsperrotation;
      digitalWrite(rdirpin, rdir);
      
      break;
    }
    poti1state = analogRead(poti1); 
    if (poti1state < 256) slowdown = 0; if (poti1state > 256 && poti1state < 512) slowdown = 1; 
    if (poti1state > 512 && poti1state < 768) slowdown = 2; if (poti1state > 768) slowdown = 3; 
    poti2state = analogRead(poti2); repeat = poti2state / 100;

    // rotation //
    lcd.setCursor(0, 0); lcd.print("Smooth: "); lcd.print(slowdown);

    // time //
    lcd.setCursor(0, 1); lcd.print("Repeat: "); lcd.print(repeat);
    
    delay(100);
    lcd.clear();
    oldbtnstate = btnstate;
  }
  
}
  
void setup() {
  // lcd setup //
  lcd.begin(16,2); lcd.clear(); lcd.createChar(0, degreessymbol);
  
  // motor //
  pinMode(mmovepin, OUTPUT); digitalWrite(mmovepin, LOW);
  pinMode(mdirpin, OUTPUT); digitalWrite(mdirpin, mdir);
  
  pinMode(onpin, OUTPUT); digitalWrite(onpin, LOW);
  
  pinMode(rmovepin, OUTPUT); digitalWrite(rmovepin, LOW);
  pinMode(rdirpin, OUTPUT);

  // btn //
  pinMode(btn, INPUT); digitalWrite(btn, HIGH);
  
  // lcd //
  settings();
  
  moving = true;
}

void loop() {
  btnstate = digitalRead(btn);
  
  digitalWrite(mmovepin, HIGH);
  digitalWrite(mmovepin, LOW);
  delayMicroseconds(dt);
  
  if (moving) distance ++;

  if (stepsperrotation == distance) {
    digitalWrite(rmovepin, HIGH);
    digitalWrite(rmovepin, LOW);
    stepsperrotation += osprot;
  }
  
  if (distance > len) {
    distance = 0;
    digitalWrite(onpin, HIGH);
    if (repeat > 1) {
      repeat -= 1;
      if (mdir == 1) mdir = 0;
      else mdir = 1;
      if (rdir == 1) rdir = 0;
      else rdir = 1;
      digitalWrite(mdirpin, mdir);
      digitalWrite(rdirpin, rdir);
      stepsperrotation = osprot;
      dt = originalspeed;
      digitalWrite(onpin, LOW);
    }
    else {
      moving = false;
      setup();
    }
  }
  else {
    if (slowdown == 2 || slowdown == 3) {
      if (distance > len - stopplace){
        dt += dt/5000.;
      }
    }
  }
  oldbtnstate = btnstate;
}
