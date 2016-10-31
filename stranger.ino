#include "FastLED.h"

// number of leds in chain
#define NUM_LEDS 26
// data pin that led data will be written out over
#define DATA_PIN 6
// necessary to avoid blindness
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

// set default idle animation
int default_animation = OFF;
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

// user input to spell on wall
String str;

void setup(){
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  FastLED.delay(2000);

  // initialize serial communication
  Serial.begin(9600);
  Serial.setTimeout(500);
  Serial.println("Ready");
  
  // set up leds
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
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

  // blink lights to indicate setup is complete
  show_solid_fill(0,0,0,300);
  show_solid_fill(255,255,255,300);
  show_solid_fill(0,0,0,300);
}

void loop() {
  if(Serial.available() > 0){
    flicker_off_strip();
    str = Serial.readString();
    //TODO change these string comparisons to direct character comparisons
    if (str.equals("0")){
      idle_animation = OFF;
    }
    else if (str.equals("1")){
      idle_animation = RAINBOW_CHASE;
    }
    else if (str.equals("2")){
      idle_animation = RAINBOW_CYCLE;
    }
    else if (str.equals("3")){
      idle_animation = COLOR_CHASE;
    }
    else if (str.equals("4")){
      idle_animation = BLINKY;
    }
    else{
      FastLED.delay(1500);
      spell(str);
      FastLED.delay(3000);
    }
  }
  idle();
}

void show_solid_fill(int r, int g, int b, int ms){
  fill_solid(leds, NUM_LEDS, CRGB(r,g,b));
  FastLED.show();
  FastLED.delay(ms);
}

void flicker_off_strip(){
  dim_strip(3,170);
  dim_strip(2,100);
  dim_strip(4,180);
  show_solid_fill(0,0,0,0);
  FastLED.setBrightness(MASTER_BRIGHTNESS);
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
  else{
    idle_animation = default_animation;
    idle();
  }
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
