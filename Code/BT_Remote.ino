
#include <Bounce2.h>
#include <Encoder.h>
#include <SoftwareSerial.h>

#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

#define BUTTON_PIN 7
#define LED_PIN 13

int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3


byte mut[5] = {0xFD,0x03,0x03,0x40,0x00};
byte up[5] = {0xFD,0x03,0x03,0x10,0x00};
byte down[5] = {0xFD,0x03,0x03,0x20,0x00};
byte clr[5] = {0xFD,0x03,0x03,0x00,0x00};

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);
int buttonState = 0;  

// Instantiate a Bounce object
Bounce debouncer = Bounce(); 

Encoder knob(5, 6);
long oldVal  = -999;
long newVal;


void setup() {

  Serial.begin(9600);  // Begin the serial monitor at 9600bps
  bluetooth.begin(9600);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
 
  bluetooth.print("CONNECT\n");  // Start bluetooth serial at 9600

  // Setup the button with an internal pull-up :
  pinMode(BUTTON_PIN,INPUT_PULLUP);

  // After setting up the button, setup the Bounce instance :
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5); // interval in ms

  //Setup the LED :
  pinMode(LED_PIN,OUTPUT);

}

void loop() {
  newVal = knob.read();
  if (newVal != oldVal){
    if(newVal > oldVal){
      bluetooth.write(up,5);
      bluetooth.write(clr,5);
    }else {
      bluetooth.write(down,5);
      bluetooth.write(clr,5);
    }
    oldVal = newVal;
  }
  
  if(bluetooth.available())  // If the bluetooth sent any characters
  {
    // Send any characters the bluetooth prints to the serial monitor
    Serial.print((char)bluetooth.read());  
  }
  if(Serial.available())  // If stuff was typed in the serial monitor
  {
    // Send any characters the Serial monitor prints to the bluetooth
    bluetooth.print((char)Serial.read());
  }
  
  // Update the Bounce instance :
  debouncer.update();

  // Get the updated value :
  int value = debouncer.read();

  // Turn on or off the LED as determined by the state :
  if ( value == LOW ) {
    digitalWrite(LED_PIN, LOW);
    buttonState = 0;
  } 
  else {
    if (buttonState == 0){ // Do it once
      // turn LED on:
      digitalWrite(LED_PIN, HIGH);
      bluetooth.write(mut,arr_len(mut));
      bluetooth.write(clr,5);
      buttonState = 1;
    }
  }

}


