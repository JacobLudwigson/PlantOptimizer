#include <Servo.h>
#include <dht_nonblocking.h>
#include <TimeLib.h>

#define DHT_SENSOR_TYPE DHT_TYPE_11
#define LEFT_BLIND_SERVO_PIN 9
#define RIGHT_BLIND_SERVO_PIN 8
#define DHT11_SENSOR_PIN 2
#define TOP_BUTTON_ANALOG_PIN A0
#define BOTTOM_BUTTON_ANALOG_PIN A1
#define False 0
#define True 1
#define Open 0
#define Closed 1
static bool blindsToOpen;
static bool blindsCurrentState;
Servo myservoLeft, myservoRight;

static const int DHT_SENSOR_PIN = DHT11_SENSOR_PIN;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }

  return( false );
}
void printTime(){
  Serial.print("Time: ");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.println(second());
}
void setup() {
  Serial.begin(9600);
  setTime(19, 00, 0, 9, 9, 2025);
  blindsToOpen = False;
  blindsCurrentState= Closed;
}
void openBlinds(int delay1, Servo& myservo, Servo& myservo2){
  for (int i = 0; i < delay1; i++){
      myservo.write(180);
      myservo2.write(180);
      delay(5);
  }
}
void closeBlinds(int delay1, Servo& myservo, Servo& myservo2){
  for (int i = 0; i < delay1; i++){
      myservo.write(0);
      myservo2.write(0);
      delay(5);
  }
}
void loop() {
  // myservoLeft.write(85);
  float temperature;
  float humidity;

  /* Measure temperature and humidity.  If the functions returns
     true, then a measurement is available. */
  if( measure_environment( &temperature, &humidity ) == true )
  {
    int curHour = hour();
    bool isMidDay = (curHour > 10) && (curHour < 19);
    if (temperature > 27.0 || !isMidDay){
      Serial.print("Setting Blinds to Auto Close!\n");
      blindsToOpen = False;
    }
    else {
      Serial.print("Setting Blinds to Auto Open!\n");
      blindsToOpen = True;
    }
    printTime();
    Serial.print( "T = " );
    Serial.print( temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );
  }
  int topButtonVal = analogRead(TOP_BUTTON_ANALOG_PIN);
  if(!topButtonVal) blindsToOpen = false;
  int bottomButtonVal = analogRead(BOTTOM_BUTTON_ANALOG_PIN);
  if(!bottomButtonVal) blindsToOpen = True; 

  if ((blindsToOpen == false && blindsCurrentState == Open)){
    Serial.print("Closing the blinds!\n");
    myservoRight.attach(RIGHT_BLIND_SERVO_PIN);
    myservoLeft.attach(LEFT_BLIND_SERVO_PIN);
    myservoRight.write(90);
    myservoLeft.write(90);
    closeBlinds(600, myservoRight, myservoLeft);
    myservoLeft.write(90);
    delay(200*5);
    myservoRight.write(90);
    myservoRight.detach();
    myservoLeft.detach();
    blindsCurrentState = Closed;
  }
  else if ((blindsToOpen == True && blindsCurrentState == Closed)){
    Serial.print("Opening the blinds!\n");
    myservoRight.attach(RIGHT_BLIND_SERVO_PIN);
    myservoLeft.attach(LEFT_BLIND_SERVO_PIN);
    myservoRight.write(90);
    myservoLeft.write(90);
    openBlinds(800, myservoRight, myservoLeft);
    myservoLeft.write(90);
    delay(700*5);
    myservoRight.write(90);
    myservoRight.detach();
    myservoLeft.detach();
    blindsCurrentState = Open;
  }
  // runServo();
}

