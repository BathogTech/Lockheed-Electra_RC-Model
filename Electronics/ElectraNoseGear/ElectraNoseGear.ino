#include <FastLED.h>
#define LED_PIN     7
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    1
#define BRIGHTNESS  60
#define FRAMES_PER_SECOND 60

CRGB leds[NUM_LEDS];

#include<Servo.h>
Servo retract;
Servo doors;
Servo steer;

//Start with Gear Down
int Seq=1; // 0=gear is Up, 1=Doors opening, 2=Doors Open, 3=Gear Descending, 4=Gear is Down, 5=Lights Off, 6=Raise Gear, 7=Close doors.
int RDgearUpDn =5;
int RDsteer=4;

bool inhib=false;   // Inhibit the turning of the nose wheel from the rudder channel.
bool lightsOn=true;   // Lights On or Off.
double PWMUpDn;
double PWMSteer;

void setup()
{
//Serial.begin(9600);
  delay(2000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.clear();
  
  retract.attach(11);
  doors.attach(10);
  steer.attach(9);
  
//pinMode(RDgearUpDn,INPUT);
//pinMode(RDsteer,INPUT);
 doors.write(45); // safer to have the doors open
}

void loop()
{
//Serial.print("Seq=");
//Serial.print(Seq);
//Serial.print(" PWMSteer=");
//Serial.println(PWMSteer);
//Serial.print(" Inhib=");
//Serial.println(inhib);
  // Deal with steering
  if(!inhib)
  {
    PWMSteer=pulseIn(RDsteer,HIGH);
    if(PWMSteer>1000){
    steer.write(map(PWMSteer,1000,2000,50,130));  // 
    }
  }
  else
  {
   steer.write(90);  
  }
 
//DOWN SEQUENCE
  // If the gear IS up and the Gear Down command is detected from the Receiver then lock in the Down Sequence
  if(Seq ==0 )
  {
    PWMUpDn=pulseIn(RDgearUpDn,HIGH);
    if(PWMUpDn<1200)
    {
    Seq =1;
    }
  }

  //Set Doors Servo to Open position
  if (Seq==1)
  {
  doors.write(45);  //Set Doors Servo to Open position.
  delay(2000);
  Seq=2;
  }

  // Lower the landing gear
  if(Seq==2)
  {
  retract.write(150);  
  delay(9000);
  Seq=3;
  }

  // release the Nosewheel steering and put Landing Lights on
  if(Seq==3)
  {
  inhib=false;
  fill_solid( leds, NUM_LEDS, CRGB::White);
  FastLED.show();
  delay(1000);
  Seq=4;
  }

// UP SEQUENCE
  // If the gear IS Down and the Gear Up command is detected from the Receiver then lock in the Up Sequence
  if(Seq ==4 )
  {
    PWMUpDn=pulseIn(RDgearUpDn,HIGH);
    if(PWMUpDn>1500)
    {
    Seq =5;
    }
  }

  // Centre the Nosewheel prior to raising the gear, and turn the Landing lights off.
  if (Seq==5)
  {
  inhib=true;
  steer.write(90); 
  delay(500);
  fill_solid( leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(1000);
  Seq=6;
  }

  // actually Raise the gear.
  if (Seq==6)
  {
  retract.write(30);  //might be wrong way round
  delay(9000);
  Seq=7;
  }

  //finally Close the Gear Doors and register the Gear as UP. 3 Greens!
  if(Seq==7)
  {
  doors.write(135);  //Set Doors Servo to Close position. might be wrong way round or need adjusting
  delay(3000);
  Seq=0; // register gear Up and locked, ready for next Gear Down Sequence
  }

}
