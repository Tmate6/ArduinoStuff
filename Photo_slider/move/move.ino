byte movepin = 2; byte dirpin = 3; byte onpin = 10;
byte btn = 13; bool btnstate; bool lastbtnstate = 1;

long distance = 0;
bool moving = true;

long len = 58000;
bool dir = 0;

long totaltime = 10;
unsigned long dt = totaltime * 1000000;

byte rmovepin = 11; byte rdirpin = 12; byte ronpin = 10;
unsigned long rdistance = 0;
bool rdir = 1;

int degreesrotation = 360;
long stepsrotation = degreesrotation*9;
long stepsbetweenrot = len / stepsrotation;
long timessteppedbetweenrot = 0;

int originalspeed;
int stopplace = 3500; int breakspeed = 0; int slowdown = 6;

void setup() {
  dt = dt / len;
  originalspeed = dt;
  pinMode(rmovepin, OUTPUT); digitalWrite(rmovepin, LOW);
  pinMode(rdirpin, OUTPUT); digitalWrite(rdirpin, rdir);
  pinMode(ronpin, OUTPUT); digitalWrite(ronpin, LOW);
  
  pinMode(movepin, OUTPUT); digitalWrite(movepin, LOW);
  pinMode(dirpin, OUTPUT); digitalWrite(dirpin, dir);
  pinMode(onpin, OUTPUT); digitalWrite(onpin, LOW);
  
  pinMode(btn, INPUT); digitalWrite(btn, HIGH);
  Serial.begin(1000000);
  Serial.println(dt);
}

void loop() {
  btnstate = digitalRead(btn);
  
  if (btnstate == 1 && lastbtnstate == 0) {
    if (moving) {
      digitalWrite(onpin, HIGH);
      digitalWrite(ronpin, HIGH);
      moving = false;
      }
    else {
      digitalWrite(onpin, LOW);
      digitalWrite(ronpin, LOW);
      moving = true;
      dt = originalspeed;
     }
   }
  if (timessteppedbetweenrot == stepsbetweenrot) {
    digitalWrite(rmovepin, HIGH);
    digitalWrite(rmovepin, LOW);
    timessteppedbetweenrot = 0;
  }
  
  if (distance < 0 || distance >= len) {
    if (dir == 1) { digitalWrite(dirpin, 0); dir = 0; distance--; }
    else          { digitalWrite(dirpin, 1); dir = 1; distance++; }
    if (rdir == 0){ rdir = 1; digitalWrite(rdirpin, rdir); }
    else          { rdir = 0; digitalWrite(rdirpin, rdir); }
    moving = false;
    digitalWrite(ronpin, HIGH);
    digitalWrite(onpin, HIGH);
    dt = originalspeed;
  }
  else {
    if (distance > len - 350){
      dt = dt + 2;
    }
    else {
      if (distance>len-350-stopplace){
        breakspeed++;
        if (breakspeed > slowdown) {
          dt++;
          breakspeed = 0;
        }
      }
    }
  }
  
  digitalWrite(movepin, HIGH);
  digitalWrite(movepin, LOW);
  delayMicroseconds(dt);
  if (moving) {
    timessteppedbetweenrot++;
    if (dir) distance++;
    else distance--;
  }

  lastbtnstate = btnstate;
}
