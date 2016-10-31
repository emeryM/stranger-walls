#include <Arduino.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

#include "FastLED.h"


/*=========================================================================
    BLUEFRUIT SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
    #define BUFSIZE                      128
/*=========================================================================*/

// Create the software serial bluefruit object
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}


/*=========================================================================
    FASTLED SETTINGS
    -----------------------------------------------------------------------*/
    
    // number of leds in chain
    #define NUM_LEDS 26
    // data pin that led data will be written out over
    #define DATA_PIN 6
    // set to 100 to experience blindness
    #define MASTER_BRIGHTNESS 15
    
    // position of each letter in led chain
    #define LET_A 25
    #define LET_B 24
    #define LET_C 23
    #define LET_D 22
    #define LET_E 21
    #define LET_F 20
    #define LET_G 19
    #define LET_H 18
    #define LET_I 9
    #define LET_J 10
    #define LET_K 11
    #define LET_L 12
    #define LET_M 13
    #define LET_N 14
    #define LET_O 15
    #define LET_P 16
    #define LET_Q 17
    #define LET_R 8
    #define LET_S 7
    #define LET_T 6
    #define LET_U 5
    #define LET_V 4
    #define LET_W 3
    #define LET_X 2
    #define LET_Y 1
    #define LET_Z 0
    
    // position of RGB values in color arrays
    #define RED 0
    #define BLU 1
    #define GRN 2
    
    // animations
    #define OFF 0
    #define RAINBOW_CHASE 1
    #define RAINBOW_CYCLE 2
    #define COLOR_CHASE 3
    #define BLINKY 4
    #define DEF 5
    #define FIERY 6

    #define COOLING  55
    #define SPARKING 120
    #define FRAMES_PER_SECOND 60
    
    bool gReverseDirection = false;

/*=========================================================================*/

// set default idle animation
int default_animation = DEF;
int idle_animation = default_animation;

CRGB leds[NUM_LEDS];

// joyce's christmas light bulb colors
int rgb_red[3] = {255,0,80};
int rgb_blue[3] = {0,0,255};
int rgb_yellow[3] = {255,200,0};
int rgb_white[3] = {255,255,200};
int rgb_teal[3] = {0,130,130};

// the color bulb that corresponds to each letter
int *alpha_colors[26];

// the led in the chain that corresponds to each letter 
int alpha_map[123];

int previous;

/**************************************************************************/
/*!
    @brief  Sets up the HW, LEDS, and the BLE module (this function is
            called automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  FastLED.delay(2000);
  randomSeed(analogRead(0));

  // set up leds
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(MASTER_BRIGHTNESS);
  alpha_colors[LET_A] = &rgb_white[0];
  alpha_colors[LET_B] = &rgb_blue[0];
  alpha_colors[LET_C] = &rgb_red[0];
  alpha_colors[LET_D] = &rgb_teal[0];
  alpha_colors[LET_E] = &rgb_blue[0];
  alpha_colors[LET_F] = &rgb_yellow[0];
  alpha_colors[LET_G] = &rgb_red[0];
  alpha_colors[LET_H] = &rgb_teal[0];
  alpha_colors[LET_I] = &rgb_teal[0];
  alpha_colors[LET_J] = &rgb_red[0];
  alpha_colors[LET_K] = &rgb_blue[0];
  alpha_colors[LET_L] = &rgb_teal[0];
  alpha_colors[LET_M] = &rgb_yellow[0];
  alpha_colors[LET_N] = &rgb_red[0];
  alpha_colors[LET_O] = &rgb_red[0];
  alpha_colors[LET_P] = &rgb_teal[0];
  alpha_colors[LET_Q] = &rgb_red[0];
  alpha_colors[LET_R] = &rgb_teal[0];
  alpha_colors[LET_S] = &rgb_white[0];
  alpha_colors[LET_T] = &rgb_yellow[0];
  alpha_colors[LET_U] = &rgb_blue[0];
  alpha_colors[LET_V] = &rgb_red[0];
  alpha_colors[LET_W] = &rgb_blue[0];
  alpha_colors[LET_X] = &rgb_yellow[0];
  alpha_colors[LET_Y] = &rgb_red[0];
  alpha_colors[LET_Z] = &rgb_red[0];
  
  alpha_map[97] = LET_A;
  alpha_map[98] = LET_B;
  alpha_map[99] = LET_C;
  alpha_map[100] = LET_D;
  alpha_map[101] = LET_E;
  alpha_map[102] = LET_F;
  alpha_map[103] = LET_G;
  alpha_map[104] = LET_H;
  alpha_map[105] = LET_I;
  alpha_map[106] = LET_J;
  alpha_map[107] = LET_K;
  alpha_map[108] = LET_L;
  alpha_map[109] = LET_M;
  alpha_map[110] = LET_N;
  alpha_map[111] = LET_O;
  alpha_map[112] = LET_P;
  alpha_map[113] = LET_Q;
  alpha_map[114] = LET_R;
  alpha_map[115] = LET_S;
  alpha_map[116] = LET_T;
  alpha_map[117] = LET_U;
  alpha_map[118] = LET_V;
  alpha_map[119] = LET_W;
  alpha_map[120] = LET_X;
  alpha_map[121] = LET_Y;
  alpha_map[122] = LET_Z;
  
  Serial.begin(115200);
  Serial.println(F("Stranger Leds"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      idle();
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change Bluefruit LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));

  // blink lights to indicate setup is complete
  show_solid_fill(0,0,0,300);
  show_solid_fill(0,0,255,300);
  show_solid_fill(0,0,0,300);
  show_solid_fill(0,0,255,300);
  show_solid_fill(0,0,0,300);
  show_solid_fill(0,0,255,300);
  show_solid_fill(0,0,0,300);

  previous = 0;
  idle_animation = OFF;
 
}


/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/

void loop() {
  if (ble.available()) {
    char c = tolower(ble.read());
    Serial.println(c);
      if ((c >= 97) && (c <=122)){
        if (previous != 1){
          flicker_off_strip(300);
        }
        show_letter(alpha_map[c]);
        previous = 1;
      }
      else if ((c == 32) && (previous == 1)){
        previous = 0;
        FastLED.delay(200);
      }
      else if (c == '.'){
        delay(800);
        idle();
      }
      else{
        if (c == '0'){
        flicker_off_strip(200);
        FastLED.delay(800);
        idle_animation = OFF;
      }
      else if (c == '1'){
        flicker_off_strip(200);
        FastLED.delay(800);
        idle_animation = RAINBOW_CHASE;
      }
      else if (c == '2'){
        flicker_off_strip(200);
        FastLED.delay(800);
        idle_animation = RAINBOW_CYCLE;
      }
      else if (c == '3'){
        flicker_off_strip(200);
        FastLED.delay(800);
        idle_animation = COLOR_CHASE;
      }
      else if (c == '4'){
        flicker_off_strip(200);
        FastLED.delay(800);
        idle_animation = BLINKY;
      }
       else if (c == '5'){
        flicker_off_strip(200);
        FastLED.delay(800);
        idle_animation = DEF;
      }
      else if (c == '6'){
        flicker_off_strip(200);
        FastLED.delay(800);
        idle_animation = FIERY;
      }
      previous = 0;
      idle();
    }
  }
  else{
    idle();
  }
}

void show_solid_fill(int r, int g, int b, int ms){
  fill_solid(leds, NUM_LEDS, CRGB(r,g,b));
  FastLED.show();
  FastLED.delay(ms);
}

void flicker_off_strip(int ms){
  dim_strip(3,170);
  dim_strip(2,100);
  dim_strip(4,180);
  show_solid_fill(0,0,0,0);
  FastLED.setBrightness(MASTER_BRIGHTNESS);
  FastLED.delay(ms);
}

void dim_strip(int denominator, int ms){
  FastLED.setBrightness(MASTER_BRIGHTNESS/denominator);
  FastLED.show();
  FastLED.delay(ms);
}

void idle(){
  if (idle_animation == OFF){
    show_solid_fill(0,0,0,0);
  }
  else if (idle_animation == RAINBOW_CHASE){
    rainbow_chase();
  }
  else if (idle_animation == RAINBOW_CYCLE){
    rainbow_cycle();
  }
  else if (idle_animation == COLOR_CHASE){
    color_chase();
  }
  else if (idle_animation == BLINKY){
    blinky();
  }
  else if (idle_animation == DEF){
    def_animate();
  }
  else if (idle_animation == FIERY){
    Fire2012(); // run simulation frame
    FastLED.show(); // display this frame
    FastLED.delay(1000 / FRAMES_PER_SECOND);
  }
  else {
    idle_animation = default_animation;
    idle();
  }
}

void def_animate(){
  int lol = random(12);
  if (lol == 0){
    spell("run");
  }
  else if (lol == 1){
    int r = random(255);
    int g = random(255);
    int b = random(255);
    chase_up(r, g, b, 100);
    chase_down(r, g, b, 100);
  }
  else if (lol == 2){
    delay(10000);
  }
  else if (lol == 3){
    spell("will  dustin  mike  lucas");
  }
  else if (lol == 4){
    spell("im right here");
  }
  else if (lol == 5){
    delay(8000);
  }
  else if (lol == 6){
    show_letter(rand() % 26);
  }
  else if (lol == 7){
    spell("waffles");
  }
  else if (lol == 8){
    spell_alphabet();
  }
  else if (lol == 9){
    spell("ask me about my demogorgon");
  }
  else if (lol == 10){
    
  }
  else if (lol == 11){
    spell("eleven");
  }
  delay(random(800,3000));
}

void blinky(){
  show_solid_fill(0,0,255,1000);
  show_solid_fill(255,0,80,1000);
  show_solid_fill(255,255,200,1000);
  show_solid_fill(0,130,130,1000);
  show_solid_fill(255,200,0,1000);
}

void rainbow_chase(){
  for (int i = 0; i < 60; ++i){
    fill_rainbow(leds, NUM_LEDS, (i*255)/60, (255/NUM_LEDS));
    FastLED.show();
    FastLED.delay(50);
  }
}

void rainbow_cycle(){
  for( int colorStep=0; colorStep<256; colorStep++ ) {
      int r = colorStep;
      int b = 255-colorStep;
      int g = 0;
      show_solid_fill(r,g,b,15); 
  }
  for( int colorStep=0; colorStep<256; colorStep++ ) {
      int g = colorStep;
      int r = 255-colorStep;
      int b = 0;
      show_solid_fill(r,g,b,15);
  }
    for( int colorStep=0; colorStep<256; colorStep++ ) {
      int b = colorStep;
      int g = 255-colorStep;
      int r = 0;
      show_solid_fill(r,g,b,15);
  }
}

void color_chase(){
  int ms = 125;
  chase_up(231, 76, 60, ms);
  chase_down(243, 156, 18, ms);
  chase_up(244, 208, 63, ms);
  chase_down(46, 204, 113, ms);
  chase_up(52, 152, 219, ms);
  chase_down(142, 68, 173, ms);
}

void chase_up(int r, int g, int b, int ms){
  for (int i = 0; i < NUM_LEDS; ++i){
    leds[i].setRGB(r,g,b);
    FastLED.show();
    FastLED.delay(ms);
    leds[i].setRGB(0,0,0);
    FastLED.show();
  }
  FastLED.delay(2*ms);
}

void chase_down(int r, int g, int b, int ms){
  for (int i = NUM_LEDS - 1; i >= 0; --i){
    leds[i].setRGB(r,g,b);
    FastLED.show();
    FastLED.delay(ms);
    leds[i].setRGB(0,0,0);
    FastLED.show();
  }
  FastLED.delay(2*ms);
}

void spell_alphabet(){
  for(int i = 0; i < NUM_LEDS; ++i){
    show_letter(i+97);
  }
}

void spell(String str){
  int len = str.length();
  for(int i = 0; i < len; ++i){
    char c = tolower(str[i]);
    int val = c;
    if (c == 32){
      FastLED.delay(500);
    }
    else if ((c >= 97) && (c <=122)){
      show_letter(alpha_map[c]);
    }
  }
}

void show_letter(int letter){
  leds[letter].setRGB(alpha_colors[letter][RED],alpha_colors[letter][BLU],alpha_colors[letter][GRN]);
  //sometimes "flicker" letter on
  int flick_on = rand() % 3;
  if (flick_on > 0){
    FastLED.setBrightness(MASTER_BRIGHTNESS/3);
    FastLED.show();
    FastLED.delay(100);
    if (flick_on == 1){
      FastLED.setBrightness(MASTER_BRIGHTNESS/2);
      FastLED.show();
      FastLED.delay(75);
    }
    FastLED.setBrightness(MASTER_BRIGHTNESS);
  }
  //display letter
  FastLED.show();
  FastLED.delay(500);
  //sometimes "flicker" letter off
  int flick_off = rand() % 4;
  if (flick_off > 1){
    FastLED.setBrightness(MASTER_BRIGHTNESS/2);
    FastLED.show();
    FastLED.delay(75);
    if (flick_off == 2){
      FastLED.setBrightness(MASTER_BRIGHTNESS/4);
      FastLED.show();
      FastLED.delay(50);
    }
    FastLED.setBrightness(MASTER_BRIGHTNESS);
  }
  //turn letter off
  leds[letter].setRGB(0,0,0);
  FastLED.show();
  FastLED.delay(300);
}



void Fire2012()
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
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}
