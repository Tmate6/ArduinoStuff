#include <LedControl.h>

int valuepin = 9;
int buzzpin = 3;

int DIN = 10;
int CS = 6;
int CLK = 13;

int j;
int i = 0;

int secondstotal = 15;
int milisecs;
int dt;

LedControl lc = LedControl(DIN, CLK, CS,0);

int save[8][8] = {B00000000,
                  B00000000,
                  B00000000,
                  B00000000,
                  B00000000,
                  B00000000,
                  B00000000,
                  B00000000};

void setup() {
  Serial.begin(115200);
  
  pinMode(9, INPUT);
  pinMode(3, OUTPUT);
  
  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
  randomSeed(analogRead(0));
  j = random(0,8);
  milisecs = secondstotal;
  milisecs = milisecs - 4 + secondstotal/100;
  dt = milisecs * 7;
  Serial.print("delaytime set to:");
  Serial.println(dt);
}


int getrandom() {
  j = random(0,8);
  if (save[0][j] == 1) {
    getrandom();
    Serial.println("retrying");
  } else {
    delay(dt);
  }
}
    
void loop() {
  delay(dt);
  for (i=0;i<8;i++) {
    if (save[i][j] == 0) {
      lc.setLed(0,j,i,true);
      delay(20);
      lc.setLed(0,j,i-1,false);
      save[i-1][j] = 0;
    }
    save[i][j] = 1;
  }
  if (save[0][0] == 1 && save[0][1] == 1 && save[0][2] == 1 && save[0][3] == 1 && save[0][4] == 1 && save[0][5] == 1 && save[0][6] == 1 && save[0][7] == 1){
      
    randomSeed(analogRead(0));

    for (int b = 0; b < 5; b++) {
      analogWrite(buzzpin, 255);
      delay(200);
      analogWrite(buzzpin, 40);
      delay(200); 
    }
    digitalWrite(buzzpin, LOW);
    lc.clearDisplay(0);
    for (int r = 0; r < 8; r++) {
      for (int k = 0; k < 8; k++) {
        save[r][k] = 0;
      }
    }
  }
  getrandom();
}
