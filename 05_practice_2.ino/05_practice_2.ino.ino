int c=0;
int cnt=0;
void setup() {
  pinMode(7,1);
  digitalWrite(7,c);
  delay(1000);
}

void loop() {
  while(cnt<=10){
    c=!c;
    digitalWrite(7, c);
    delay(100);
    cnt++;
  }
}
