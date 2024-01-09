byte movepin = 8; byte dirpin = 9; byte onpin = 7;
long distance = 0;
bool moving = true;

int len = 29000;
bool dir = 1;

int dt = 70;

byte btn = 5; bool btnstate; bool lastbtnstate = 1;

void setup() {
  pinMode(movepin, OUTPUT); digitalWrite(movepin, LOW);
  pinMode(dirpin, OUTPUT); digitalWrite(dirpin, dir);
  pinMode(onpin, OUTPUT); digitalWrite(onpin, LOW);
  
  pinMode(btn, INPUT); digitalWrite(btn, HIGH);
  for (int i = 0; i < 500; i++) {
    digitalWrite(movepin, HIGH);
    delayMicroseconds(570 - i);
    digitalWrite(movepin, LOW);
    delayMicroseconds(570 - i);
  }
}

void loop() {
  btnstate = digitalRead(btn);
  if (btnstate == 1 && lastbtnstate == 0) {
      digitalWrite(onpin, HIGH);
      moving = false;
  }
  digitalWrite(movepin, HIGH);
  digitalWrite(movepin, LOW);
  delayMicroseconds(dt);
  lastbtnstate = btnstate;
}
