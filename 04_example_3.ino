#define PIN_LED 13
unsigned int cnt,tog;
void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED,1);
  Serial.begin(115200);
  while(!Serial){
    //Serial.println("wait!!!");
    delay(100);
  }
  Serial.println("hello world");
  cnt=tog=0;
  digitalWrite(PIN_LED,tog);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(++cnt);
  tog=!tog;
  digitalWrite(PIN_LED,tog);
  delay(1000);
}