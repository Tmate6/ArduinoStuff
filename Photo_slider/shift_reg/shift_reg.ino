int latchpin = 5;
int clockpin = 3;
int datapin = 6;

byte LEDs = 0XF0;

char led[9] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

void setup() {
  pinMode(latchpin, OUTPUT); 
  pinMode(clockpin, OUTPUT);
  pinMode(datapin, OUTPUT);
}

void loop() {
  digitalWrite(latchpin, LOW);
  shiftOut(datapin, clockpin, LSBFIRST, 0xFF);
  digitalWrite(latchpin, HIGH);
  delay(100);
  digitalWrite(latchpin, LOW);
  shiftOut(datapin, clockpin, LSBFIRST, 0x00);
  digitalWrite(latchpin, HIGH);
  delay(100);
}
