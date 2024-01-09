byte rmovepin = 11; byte rdirpin = 12; byte ronpin = 10;
unsigned long distance = 0;

bool dir = 1;

byte times = 1;
unsigned long dtend = 800;
unsigned long totaltime = 800;
int degreesrotation = 360;

long stepsrotation = degreesrotation*4.5;
long dt = totaltime*1000/stepsrotation;

void setup() {
  
  pinMode(rmovepin, OUTPUT); digitalWrite(rmovepin, LOW);
  pinMode(rdirpin, OUTPUT); digitalWrite(rdirpin, dir);
  pinMode(ronpin, OUTPUT); digitalWrite(ronpin, LOW);
}

void loop() {
  
  digitalWrite(rmovepin, HIGH);
  digitalWrite(rmovepin, LOW);
  delayMicroseconds(dt);
  distance++;
  
  if (distance == stepsrotation) {
    delay(dtend); distance = 0;
    if (dir == 0) { dir = 1; digitalWrite(rdirpin, dir); }
    else { dir = 0; digitalWrite(rdirpin, dir); }
    times--;
    if (times == 0) digitalWrite(ronpin, HIGH);
  }
  
}
