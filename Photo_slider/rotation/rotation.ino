byte rmovepin = 11; byte rdirpin = 12; byte ronpin = 10;
unsigned long distance = 0;

bool dir = 1;
byte times = 1;                // Amount of times to rotate
unsigned long dtend = 0;       // Delay between each rotation (in miliseconds)
unsigned long totaltime = 600;   // Total rotation time (in seconds, minimum 2 per 360 degrees)
int degreesrotation = 360;     // Degrees to rotate each time

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
  delay(dt);
  distance++;
  
  if (distance == stepsrotation) {
    delay(dtend); distance = 0;
    if (dir == 0) { dir = 1; digitalWrite(rdirpin, dir); }
    else { dir = 0; digitalWrite(rdirpin, dir); }
    times--;
    if (times == 0) digitalWrite(ronpin, HIGH);
  }
  
}
