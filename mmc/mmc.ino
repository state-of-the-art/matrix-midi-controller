/**
 * State of the Art Matrix Midi Controller
 * 
 * License: Apache v2.0
 * Site: https://state-of-the-art.io/projects/matrix-midi-controller/
 */
#include <Adafruit_NeoTrellis.h> // SeeSaw library
#include <MIDI.h> // Teensy library
#include <EEPROM.h> // Arduino/Teensy library
#include "font.h"

#define VERSION "1"

#define INT_PIN 2 // interrupt signal from neotrellis

#define Y_DIM 8 // number of rows
#define X_DIM 24 // number of columns

#define MENU_BUTTON (Y_DIM*X_DIM-1) // menu button

// TODO: Dynamic t_array create based on Y_DIM/X_DIM
Adafruit_NeoTrellis t_array[Y_DIM/4][X_DIM/4] = {
  { Adafruit_NeoTrellis(0x2E), Adafruit_NeoTrellis(0x30), Adafruit_NeoTrellis(0x32), Adafruit_NeoTrellis(0x34), Adafruit_NeoTrellis(0x36), Adafruit_NeoTrellis(0x38) },
  { Adafruit_NeoTrellis(0x2F), Adafruit_NeoTrellis(0x31), Adafruit_NeoTrellis(0x33), Adafruit_NeoTrellis(0x35), Adafruit_NeoTrellis(0x37), Adafruit_NeoTrellis(0x39) }
};

Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)t_array, Y_DIM/4, X_DIM/4);

// TODO: move EEPROM configs addresses to definitions
byte _velocity = 100;
int8_t _tune = 0;
byte _channel = 1;
byte _mode = 0; // 0 - menu, 1 - keys
byte _mode_prev = 0; // Previous mode to return back
unsigned long _key_press_time = 0; // To store long press for key
unsigned long _menu_press_time = 0; // To store long press for menu
byte _brightness = 6; // Keys brightness

byte _midi_clock_tick = 0;

bool _highlight_bg_keys = true;
bool _vertical_lines_keys = true;
bool _highlight_c_keys = true;

struct ScrollItem {
  const char *text; // Text to scroll
  byte x;
  byte y;
  int8_t width;
  int8_t offset;
  bool active;
  unsigned long last_update;
};
ScrollItem menu_opt_desc;

// Map for existing keys for MIDI signals
// TODO: Dynamic filling the map based on the choosen (option) map function
byte keys_mapping[Y_DIM*X_DIM] = {
  // B2, C3, C#4...
  47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
  // F#2, G2, G#2...
  42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
  // C#2, D2, D#2...
  37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
  // G#1, A1, A#1...
  32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
  // D#1, E1, F1...
  27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
  // A#0, B1, C1...
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
  // F0, F#0, G0...
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  // C0, C#0, D0...
  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};

bool menu_items_act[] = { // Active items
  false, // NULL (not an element)
  false, true, false, // MIDI Velocity set
  false, false, // Tune keys
  true, // Highlight background keys
  false, false, // Brightness
  true, // Vertical lines
  true, // C highlight
  false, false, // Octave Up / Down
};

void velocityMax() {
  menu_items_act[1] = true;
  menu_items_act[2] = false;
  menu_items_act[3] = false;
  _velocity = 127;
  EEPROM.write(101, _velocity);
}

void velocityMed() {
  menu_items_act[1] = false;
  menu_items_act[2] = true;
  menu_items_act[3] = false;
  _velocity = 100;
  EEPROM.write(101, _velocity);
}

void velocityMin() {
  menu_items_act[1] = false;
  menu_items_act[2] = false;
  menu_items_act[3] = true;
  _velocity = 75;
  EEPROM.write(101, _velocity);
}

void tuneSet(int8_t tune) {
  _tune = tune;
  EEPROM.write(104, _tune);
}

inline void tuneUp() {
  tuneSet(_tune+1);
}

inline void tuneDown() {
  tuneSet(_tune-1);
}

inline void octaveUp() {
  tuneSet(_tune+12);
}

inline void octaveDown() {
  tuneSet(_tune-12);
}

void keysBGHighlightTrigger() {
  _highlight_bg_keys = !_highlight_bg_keys;
  menu_items_act[6] = _highlight_bg_keys;
  EEPROM.write(106, _highlight_bg_keys);
}

void brightnessInc() {
  if( _brightness < 20 )
    _brightness += 2;
  EEPROM.write(107, _brightness);
}

void brightnessDec() {
  if( _brightness > 2 )
    _brightness -= 2;
  EEPROM.write(107, _brightness);
}

void keysVerticalLinesTrigger() {
  _vertical_lines_keys = !_vertical_lines_keys;
  menu_items_act[9] = _vertical_lines_keys;
  EEPROM.write(109, _vertical_lines_keys);
}

void keysCHighlightTrigger() {
  _highlight_c_keys = !_highlight_c_keys;
  menu_items_act[10] = _highlight_c_keys;
  EEPROM.write(110, _highlight_c_keys);
}

struct MenuItem {
  long color; // Color of the menu button
  void (*fun)(); // Function to execute on activate
  const char *desc; // Short description
};

const MenuItem menu_items[] { // DON'T FORGET TO ADD menu_items_act default active
  { 0x000000, NULL, "" }, // 0 - not an element
  // Velocity
  { 0x010000, velocityMax, "Velocity maximum" }, // 1
  { 0x000001, velocityMed, "Velocity medium" },  // 2
  { 0x000100, velocityMin, "Velocity minimum" }, // 3
  // Tune
  { 0x010000, tuneUp, "Tune up" }, // 4
  { 0x000100, tuneDown, "Tune down" }, // 5
  // Keys BG Highlight
  { 0x010101, keysBGHighlightTrigger, "Keys BG Highlight" }, // 6
  // Brightness
  { 0x010000, brightnessInc, "Brightness increase" }, // 7
  { 0x000100, brightnessDec, "Brightness decrease" }, // 8
  // Vertical lines
  { 0x010001, keysVerticalLinesTrigger, "Keys vertical lines" }, // 9
  // C highlight
  { 0x000100, keysCHighlightTrigger, "Keys C highlight" }, // 10
  // Octave
  { 0x010000, octaveUp, "Octave up" }, // 11
  { 0x000100, octaveDown, "Octave down" }, // 12
};

// TODO: Allow to map menu options as hotkey to the keys layout
const byte menu_mapping[Y_DIM*X_DIM] = {
  // 1
  1, 0, 5, 4,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  // 2
  2, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  // 3
  3, 0, 12, 11,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  // 4
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  // 5
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  // 6
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  // 7
  7, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  // 8
  8, 0, 6, 0,
  9, 0, 10, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
};

inline byte keyGet(int key_num) {
  return keys_mapping[key_num] + _tune;
}

void keySetColor(int x, int y, long color = 0x0) {
  int key_n = y*X_DIM+x;
  if( key_n == MENU_BUTTON ) // Menu button show in a different color
    trellis.setPixelColor(x, y, 0x010000 * _brightness);
  else if( keyGet(key_n) > 127 ) // Do not show keys not in the midi range 0..127
    trellis.setPixelColor(x, y, 0x0);
  else if( _highlight_c_keys && keyGet(key_n) == 60 ) // Show C3 note (middle to locate current tune)
    trellis.setPixelColor(x, y, 0x000101 * (_brightness/2));
  else if( _highlight_c_keys && keyGet(key_n) % 12 == 0 ) // Show all C keys
    trellis.setPixelColor(x, y, 0x000100 * _brightness);
  else if( _vertical_lines_keys && (x == 7 || x == 12 || x == 19) ) // Show vertical lines for fretboard navigation
    trellis.setPixelColor(x, y, 0x010001 * (_brightness/2));
  else if( _highlight_bg_keys )
    trellis.setPixelColor(x, y, 0x010101);
  else
    trellis.setPixelColor(x, y, 0x0);
}

void _menuOpen() {
  int button_max = Y_DIM*X_DIM;
  for( int b=0; b<button_max; b++ ) {
    if( menu_mapping[b] != 0 ) {
      const MenuItem *item = &menu_items[menu_mapping[b]];
      trellis.setPixelColor(b, item->color * (menu_items_act[menu_mapping[b]] ? _brightness : max(1, _brightness/4)));
    } else if( b == MENU_BUTTON )
      trellis.setPixelColor(b, 0x010000 * _brightness);
    else
      trellis.setPixelColor(b, 0x0);
  }
  trellis.show();
}

void _keysOpen() {
  menu_opt_desc.active = false;
  for( int y=0; y<Y_DIM; y++ ) {
    for( int x=0; x<X_DIM; x++ ) {
      keySetColor(x, y);
    }
  }
  trellis.show();
}

void textDrawScroll(const char *text, int x, int y, int8_t width, int8_t offset) {
  menu_opt_desc.text = text;
  menu_opt_desc.x = x;
  menu_opt_desc.y = y;
  menu_opt_desc.width = width;
  menu_opt_desc.offset = offset;
  menu_opt_desc.last_update = 0;
  menu_opt_desc.active = true;
}

void textDraw(const char *text, int x, int y, int8_t width, int8_t offset) {
  int8_t text_counter = 0;
  char ch = text[text_counter];
  while( ch != 0x00 && offset < width ) {
    for( byte h = 0; h<FONT_HEIGHT; h++ ) {
      const int line = *(fontCharGet(ch)+h);
      for( int w = 0; w<FONT_WIDTH; w++ ) {
        if( offset+w < 0 )
          continue;
        else if( offset+w > width )
          break;
        if( (line >> (FONT_WIDTH-w-1)) & 1 )
          trellis.setPixelColor(x+offset+w, y+h, 0x010101 * (_brightness/2));
        else
          trellis.setPixelColor(x+offset+w, y+h, 0x0);
      }
    }
    text_counter++;
    ch = text[text_counter];
    offset += FONT_WIDTH;
  }
  trellis.show();
}

void keysInit() {
  _mode = 1;
  textDraw("SMMC" VERSION, 1, 4, 22, 0);
  for( int y=0; y<Y_DIM; y++ ) {
    for( int x=0; x<X_DIM; x++ ) {
      trellis.setPixelColor(x, y, 0x040404 * _brightness);
      trellis.show();
    }
    for( int x=0; x<X_DIM; x++ ) {
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, _buttonPressed);

      keySetColor(x, y);
    }
    trellis.show();
  }
}

void modeSet(byte mode) {
  _mode = mode;
  if( _mode == 0 )
    _menuOpen();
  else if( _mode == 1 )
    _keysOpen();
}

// Callback for button
TrellisCallback _buttonPressed(keyEvent evt) {
  if( evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING ) { // on rising
    _key_press_time = millis();
    Serial.printf("Pressing button %i\n", evt.bit.NUM);
    if( evt.bit.NUM == MENU_BUTTON ) {
      if( _mode != 0 ) {
        _menu_press_time = millis();
        _mode_prev = _mode;
        modeSet(0);
      } else
        modeSet(_mode_prev);
      trellis.setPixelColor(evt.bit.NUM, 0x040000 * _brightness);
    } else {
      if( _mode == 1 ) { // Keys
        if( keyGet(evt.bit.NUM) <= 127 ) {
          usbMIDI.sendNoteOn(keyGet(evt.bit.NUM), _velocity, _channel);
          trellis.setPixelColor(evt.bit.NUM, 0x000404 * _brightness);
        }
      } else if( _mode == 0 ) { // Menu
        if( menu_mapping[evt.bit.NUM] != 0 ) {
          trellis.setPixelColor(evt.bit.NUM, menu_items[menu_mapping[evt.bit.NUM]].color * _brightness);
          textDrawScroll(menu_items[menu_mapping[evt.bit.NUM]].desc, 12, 4, 12, FONT_WIDTH);
        }
      }
    }
  } else if( evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING ) { // off falling
    Serial.printf("Releasing button %i\n", evt.bit.NUM);
    if( evt.bit.NUM == MENU_BUTTON ) {
      trellis.setPixelColor(evt.bit.NUM, 0x010000 * _brightness);
      if( _menu_press_time < millis() - 500 )
        modeSet(_mode_prev);
    } else {
      if( _mode == 1 ) { // Keys
        if( keyGet(evt.bit.NUM) <= 127 ) {
          usbMIDI.sendNoteOff(keyGet(evt.bit.NUM), _velocity, _channel);
          int x = evt.bit.NUM % X_DIM;
          
          keySetColor(x, evt.bit.NUM / X_DIM);
        }
      } else if( _mode == 0 ) { // Menu
        if( _key_press_time > millis() - 500 )
          menu_items[menu_mapping[evt.bit.NUM]].fun();
        menu_opt_desc.active = false;
        _menuOpen();
      }
    }
  }
    
  trellis.show();
  return 0;
}

void settingsRead() {
  // Read EEPROM settings
  // Init
  byte data = EEPROM.read(0);
  if( data != 0x01 ) {
    EEPROM.write(0, 0x01);
    velocityMed();
    //tuneSet(_tune);
    EEPROM.write(106, (byte)_highlight_bg_keys);
    EEPROM.write(107, _brightness);
    return;
  }

  // Velocity
  data = EEPROM.read(101);
  if( data == 75 )
    velocityMin();
  else if( data == 127 )
    velocityMax();

  // Tune
  //data = EEPROM.read(104);
  //tuneSet(data);

  // Highlight BG keys
  data = EEPROM.read(106);
  if( _highlight_bg_keys != (bool)data )
    keysBGHighlightTrigger();

  // Brightness
  data = EEPROM.read(107);
  if( data >= 2 && data <= 20 )
    _brightness = data;

  // Vertical lines keys
  data = EEPROM.read(109);
  if( _vertical_lines_keys != (bool)data )
    keysVerticalLinesTrigger();

  // Highlight BG keys
  data = EEPROM.read(110);
  if( _highlight_c_keys != (bool)data )
    keysCHighlightTrigger();
}

void onNoteOn(byte channel, byte note, byte velocity) {
  if( _mode == 1 ) { // Only on keys mode
    for( int i = 0; i < Y_DIM*X_DIM; i++ ) {
      if( keyGet(i) == note ) {
        trellis.setPixelColor(i, velocity/10 * _brightness);
        break;
      }
    }
  }
  trellis.show();
}

void onNoteOff(byte channel, byte note, byte velocity) {
  if( _mode == 1 ) { // Only on keys mode
    for( int i = 0; i < Y_DIM*X_DIM; i++ ) {
      if( keyGet(i) == note ) {
        keySetColor(i%X_DIM, i/X_DIM);
        break;
      }
    }
  }
  trellis.show();
}

void onStart() {
  _midi_clock_tick = 0;
}

void onClock() {
  _midi_clock_tick++;
  if( _midi_clock_tick == 24 )
    _midi_clock_tick = 0;
  
  if( _mode == 1 ) { // Only on keys mode
    if( _midi_clock_tick == 0 ) {
      trellis.setPixelColor(MENU_BUTTON, 0x020000 * _brightness);
      trellis.show();
    }
    else if( _midi_clock_tick == 4 ) {
      trellis.setPixelColor(MENU_BUTTON, 0x010000 * _brightness);
      trellis.show();
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(INT_PIN, INPUT);

  if( !trellis.begin() ) {
    Serial.println("failed to init trellis");
    while(1);
  }
  
  //while (!Serial); // Waiting for Serial Monitor
  Serial.println("State of the Art Matrix MIDI controller");

  menu_opt_desc.active = false;
  menu_opt_desc.text = nullptr;

  // Init settings
  settingsRead();

  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
  usbMIDI.setHandleStart(onStart);
  usbMIDI.setHandleClock(onClock);

  // Activate keys
  keysInit();
}

void loop() {
  if( !digitalRead(INT_PIN) )
    trellis.read();

  // Reading all the incoming MIDI messages
  while( usbMIDI.read() ) {}

  if( menu_opt_desc.active && menu_opt_desc.last_update < millis() - 200 ) {
    if( (-menu_opt_desc.offset) > (int)strlen(menu_opt_desc.text)*FONT_WIDTH )
      menu_opt_desc.offset = menu_opt_desc.width;
    textDraw(menu_opt_desc.text, menu_opt_desc.x, menu_opt_desc.y, menu_opt_desc.width, menu_opt_desc.offset--);
    menu_opt_desc.last_update = millis();
  }

  delay(2);
}
