// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define _EMA_ALPHA 0.65    // EMA weight of new sample (range: 0 to 1)                          // Setting EMA to 1 effectively disables EMA filter.
#define N 30
float arr[N],v[N];
void shift(){
  for(int i=0;i<N-1;i++) v[i]=v[i+1];
}
void copy(){
  for(int i=0;i<N;i++) arr[i]=v[i];
}
void swap(int a,int b){
  float temp=arr[a];
  arr[a]=arr[b];
  arr[b]=temp;
}
float middle(){
  copy();
  for(int i=0;i<=N/2;i++){
    float mn=arr[i];
    int mn_id=i;
    for(int j=i+1;j<N;j++){
      if(mn>arr[j]){
        mn=arr[j];
        mn_id=j;
      }
    }
    swap(i,mn_id);
  }
  return arr[N/2];
}
// global variables
unsigned long last_sampling_time;   // unit: msec
float dist_prev = _DIST_MAX;        // Distance last-measured
float dist_ema;                     // EMA distance

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float dist_raw;
  
  // wait until next sampling time. 
  // millis() returns the number of milliseconds since the program started. 
  //    will overflow after 50 days.
  if (millis() < last_sampling_time + INTERVAL)
    return;

  // get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
      //dist_raw = _DIST_MAX + 10.0;    // Set Higher Value
      dist_raw = dist_prev;
      digitalWrite(PIN_LED, 1);       // LED OFF
  } else if (dist_raw < _DIST_MIN) {
      dist_raw = dist_prev;
      //dist_raw = _DIST_MIN - 10.0;    // Set Lower Value
      digitalWrite(PIN_LED, 1);       // LED OFF
  } else {    // In desired Range
      digitalWrite(PIN_LED, 0);       // LED ON      
  }

  // Modify the below line to implement the EMA equation
  dist_prev=dist_ema = dist_raw*_EMA_ALPHA+(1-_EMA_ALPHA)*dist_prev;
  shift();
  v[N-1]=dist_raw;
  float dist_mid=middle();
  // output the distance to the serial port
  Serial.print("Min:");   Serial.print(_DIST_MIN);
  Serial.print(",raw:");  Serial.print(dist_raw);
  Serial.print(",ema:");  Serial.print(dist_ema);
  Serial.print(",median:");  Serial.print(dist_mid);
  Serial.print(",Max:");  Serial.print(_DIST_MAX);
  Serial.println("");

  // do something here

  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - pulseIn(ECHO, HIGH, timeout) returns microseconds (음파의 왕복 시간)
  // - 편도 거리 = (pulseIn() / 1,000,000) * SND_VEL / 2 (미터 단위)
  //   mm 단위로 하려면 * 1,000이 필요 ==>  SCALE = 0.001 * 0.5 * SND_VEL
  //
  // - 예, pusseIn()이 100,000 이면 (= 0.1초, 왕복 거리 34.6m)
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346
  //        = 17,300 mm  ==> 17.3m
}
