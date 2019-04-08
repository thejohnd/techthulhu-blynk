/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how to use the Menu Widget.

  App project setup:
    Menu widget attached to V1 (put 3 items in it)
 *************************************************************/

/* Comment this out to disable prints and save space */
//#define BLYNK_PRINT Serial


#include <Bridge.h>
#include <BlynkSimpleYun.h>
#include <FastLED.h>

#define LED_PIN     6
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    59

#define FRAMES_PER_SECOND 60
int BRIGHTNESS = 100;
bool gReverseDirection = 1;
CRGB leds[NUM_LEDS];
unsigned long color = CRGB::Black;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "bddc0b19be984aae876990baebb2b2d4";
CRGBPalette16 gPal = CRGBPalette16(CRGB::Grey, color, CRGB::Black, CRGB::Black);
WidgetTerminal terminal(V2);
int COOLING = 55;
int SPARKING = 120;
int faction;

BLYNK_WRITE(V3) {
  faction = param.asInt();
  FastLED.show();
}

BLYNK_WRITE(V5) {
  SPARKING = param.asInt();
  FastLED.show();
}

BLYNK_WRITE(V6) {
  COOLING = param.asInt();
  FastLED.show();
}

BLYNK_WRITE(V7) {
  BRIGHTNESS = param.asInt();
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.show();
}


BLYNK_WRITE(V1) {
  terminal.println("got item");
  switch (param.asInt())
  {
    case 1: // Item 1
      terminal.println("Item 1 selected");
      color = 0xfece5a;
      break;
    case 2: // Item 2
      terminal.println("Item 2 selected");
      color = 0xffa630;
      break;
    case 3: // Item 3
      terminal.println("Item 3 selected");
      color = 0xff7315;
      break;
    case 4: // Item 3
      terminal.println("Item 4 selected");
      color = 0xe40000;
      break;
    case 5: // Item 3
      terminal.println("Item 5 selected");
      color = 0xfd2992;
      break;
    case 6: // Item 3
      terminal.println("Item 6 selected");
      color = 0xeb26cd;
      break;
    case 7: // Item 3
      terminal.println("Item 7 selected");
      color = 0xc124e0;
      break;
    case 8: // Item 3
      terminal.println("Item 8 selected");
      color = 0x9627f4;
      break;
    case 9: // Item 3
      terminal.println("Item 8 selected");
      color = 0xFFFFFF;
      break;
    default:
      terminal.println("Unknown item selected");
  }
  terminal.flush();
  for (int i=0;i < NUM_LEDS;i++){
    leds[i] = CRGB::Black;  //initialize all to black
  }
  switch (faction){
    case 1:
      gPal = CRGBPalette16(CRGB::Grey, color, CRGB::Black, CRGB::Black);
      terminal.println("case1");
      break;
    case 2:
      gPal = CRGBPalette16(0x03DC03, color, CRGB::Black, CRGB::Black);
      terminal.println("case2");
      break;
    case 3:
      gPal = CRGBPalette16(0x0088FF, color, CRGB::Black, CRGB::Black);
      terminal.println("case3");
      break;
    default:
      gPal = CRGBPalette16(CRGB::Grey, color, CRGB::Black, CRGB::Black);
      terminal.println("default");
  }
  //gPal = CRGBPalette16(0x03DC03, color, CRGB::Black, CRGB::Black);
  FastLED.show();
  terminal.flush();
}



void setup()
{
  // Debug console
  //Serial.begin(9600);
  Blynk.begin(auth);
  //terminal.println("started");
  terminal.println(F("Blynk v" BLYNK_VERSION ": Device started"));
  terminal.println(F("-------------"));
  terminal.flush();
  pinMode(2,OUTPUT);
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( SodiumVapor );
  FastLED.setBrightness( BRIGHTNESS );
  // You can also specify server:
  //Blynk.begin(auth, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, IPAddress(192,168,1,100), 8442);
  FastLED.show();
}

void loop()
{
  Blynk.run();
  
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy( random());
  Fire2012WithPalette(); // run simulation frame, using palette colors
  leds[0] = color;
  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
//#define COOLING  65

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
//#define SPARKING 110


void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}


