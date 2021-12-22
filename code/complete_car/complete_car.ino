//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define BAUDRATE 9600
#define servo_pin 10
#define enA 6
#define in1 7
#define in2 5

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN
Servo servo;

//address through which two modules communicate.
const byte address[6] = "00001";
int raw_analog_reading_x, raw_analog_reading_y;

void setup() {
  
  Serial.begin(BAUDRATE);
  
  radio.begin();
  
  radio.openReadingPipe(0, address);
  
  radio.startListening();

  // Set all the motor control pins to outputs
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  // Attach servo pin to Servo object
  servo.attach(servo_pin);
  
  // Turn off motors - Initial state
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  
}

void receive_joystick_inputs() {

  const int inputs[2];

  if(radio.available()) {
    radio.read(&inputs, sizeof(inputs));
  }

  raw_analog_reading_x = inputs[0];
  raw_analog_reading_y = inputs[1];
  
}

void map_servo() {

  int servo_pos = map(raw_analog_reading_x, 0, 675, 0, 180);
  
  servo.write(servo_pos);
  
}

void map_motomoto() {

  int motor_speed = 100;

  if(raw_analog_reading_y > 355){
    // Forward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  else if(raw_analog_reading_y < 347){
    // Backward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  else { 
    // Stop
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
  
  analogWrite(enA, motor_speed);
  
}

void loop() {

  receive_joystick_inputs();
  
  map_servo();

  map_motomoto();

  Serial.print("(X,Y)=(");
  Serial.print(raw_analog_reading_x);
  Serial.print(",");
  Serial.print(raw_analog_reading_y);
  Serial.println(")");
   
}