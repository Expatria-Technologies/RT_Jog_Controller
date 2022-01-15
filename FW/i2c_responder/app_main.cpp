/**
 * Example program for basic use of pico as an I2C peripheral (previously known as I2C slave)
 * 
 * This example allows the pico to act as a 256byte RAM
 * 
 * Author: Graham Smith (graham@smithg.co.uk)
 */


// Usage:
//
// When writing data to the pico the first data byte updates the current address to be used when writing or reading from the RAM
// Subsequent data bytes contain data that is written to the ram at the current address and following locations (current address auto increments)
//
// When reading data from the pico the first data byte returned will be from the ram storage located at current address
// Subsequent bytes will be returned from the following ram locations (again current address auto increments)
//
// N.B. if the current address reaches 255, it will autoincrement to 0 after next read / write

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "ss_oled.h"

#include "pico/stdio.h"
#include "pico/time.h"
#include <tusb.h>
#include "Adafruit_NeoPixel.hpp"

#include <i2c_fifo.h>
#include <i2c_slave.h>

//#define SHOWJOG 1
//#define SHOWOVER 1
#define SHOWRAM 1

#define TICK_TIMER_PERIOD 10

enum Jogmode {FAST = 0,
              SLOW = 1,
              STEP = 2};

enum Jogmode current_jogmode;
uint8_t jog_color[] = {0,255,0};
uint8_t halt_color[] = {0,255,0};
uint8_t hold_color[] = {0,255,0};
uint8_t run_color[] = {0,255,0};
int32_t feed_color[] = {0,10000,0};
int32_t rpm_color[] = {0,10000,0};

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        22 // On Trinket or Gemma, suggest changing this to 1
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 10 // Popular NeoPixel ring size
#define DELAYVAL 200 // Time (in milliseconds) to pause between pixels
#define CYCLEDELAY 12 // Time to wait after a cycle
#define NEO_BRIGHTNESS 128

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// define I2C addresses to be used for this peripheral
static const uint I2C_SLAVE_ADDRESS = 0x49;
static const uint I2C_BAUDRATE = 100000; // 100 kHz

// GPIO pins to use for I2C SLAVE
static const uint I2C_SLAVE_SDA_PIN = 0;
static const uint I2C_SLAVE_SCL_PIN = 1;

// RPI Pico
#define SDA_PIN 2
#define SCL_PIN 3
#define RESET_PIN -1
int rc;
SSOLED oled;
static uint8_t ucBuffer[1024];
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_SCREEN_FLIP 1

//neopixel led locations
#define RAISELED 0
#define JOGLED 1
#define SPINLED 2
#define FEEDLED 3
#define HALTLED 4
#define HOLDLED 5
#define RUNLED 6
#define SPINDLELED 7
#define COOLED 8
#define HOMELED 9

#define KPSTR_PIN 28

#define HALTBUTTON 5
#define RUNBUTTON 20
#define HOLDBUTTON 19

#define JOG_SELECT 14
#define UPBUTTON 6
#define DOWNBUTTON 7
#define LEFTBUTTON 8
#define RIGHTBUTTON 9
#define LOWERBUTTON 10
#define RAISEBUTTON 11

#define FEEDOVER_UP 4
#define FEEDOVER_DOWN 12
#define FEEDOVER_RESET 13

#define SPINOVER_UP 15
#define SPINOVER_DOWN 16
#define SPINOVER_RESET 26

#define SPINDLEBUTTON 21
#define FLOODBUTTON 17
#define MISTBUTTON 18
#define HOMEBUTTON 27

#define ONBOARD_LED 25

#define LED_UPDATE_PERIOD 10
#define KEYPAD_UPDATE_PERIOD 10
#define SCREEN_UPDATE_PERIOD 20
#define STATUS_REQUEST_PERIOD 100
int led_update_counter = 0;
int update_neopixel_leds = 0;
int screen_update_counter = 0;
int status_update_counter = 0;
int update_main_screen = 0;
int onboard_led_count = 0;
int keypad_update_counter = 0;
int jogmode = 0;

// ram_addr is the current address to be used when writing / reading the RAM
// N.B. the address auto increments, as stored in 8 bit value it automatically rolls round when reaches 255
uint8_t ram_addr = 0;
uint8_t status_addr = 0;
uint8_t key_pressed = 0;
uint8_t jog_toggle_pressed = 0;
uint8_t execute_toggle_action = 0;
uint8_t key_character = '\0';

uint8_t feed_up_pressed = 0;
uint8_t feed_down_pressed = 0;
uint8_t feed_reset_pressed = 0;

uint8_t spin_up_pressed = 0;
uint8_t spin_down_pressed = 0;
uint8_t spin_reset_pressed = 0;

uint8_t mist_pressed = 0;
uint8_t flood_pressed = 0;
uint8_t spinoff_pressed = 0;
uint8_t home_pressed = 0;


#define STATE_ALARM         1 //!< In alarm state. Locks out all g-code processes. Allows settings access.
#define STATE_CYCLE         2 //!< Cycle is running or motions are being executed.
#define STATE_HOLD          3 //!< Active feed hold
#define STATE_TOOL_CHANGE   4 //!< Manual tool change, similar to #STATE_HOLD - but stops spindle and allows jogging.
#define STATE_IDLE          5 //!< Must be zero. No flags.
#define STATE_HOMING        6 //!< Performing homing cycle
#define STATE_JOG           7 //!< Jogging mode.


// The slave implements a 256 byte memory. To write a series of bytes, the master first
// writes the memory address, followed by the data. The address is automatically incremented
// for each byte transferred, looping back to 0 upon reaching the end. Reading is done
// sequentially from the current memory address.
#define I2C_TIMEOUT_VALUE 1000
static struct
{
    uint8_t mem[256];
    uint8_t mem_address;
    bool mem_address_written;
} context;

// Alarm executor codes. Valid values (1-255). Zero is reserved.
typedef enum {
    Alarm_None = 0,
    Alarm_HardLimit = 1,
    Alarm_SoftLimit = 2,
    Alarm_AbortCycle = 3,
    Alarm_ProbeFailInitial = 4,
    Alarm_ProbeFailContact = 5,
    Alarm_HomingFailReset = 6,
    Alarm_HomingFailDoor = 7,
    Alarm_FailPulloff = 8,
    Alarm_HomingFailApproach = 9,
    Alarm_EStop = 10,
    Alarm_HomingRequried = 11,
    Alarm_LimitsEngaged = 12,
    Alarm_ProbeProtect = 13,
    Alarm_Spindle = 14,
    Alarm_HomingFailAutoSquaringApproach = 15,
    Alarm_SelftestFailed = 16,
    Alarm_MotorFault = 17,
    Alarm_AlarmMax = Alarm_MotorFault
} alarm_code_t;

typedef union {
    uint8_t value;                 //!< Bitmask value
    uint8_t mask;                  //!< Synonym for bitmask value
    struct {
        uint8_t flood          :1, //!< Flood coolant.
                mist           :1, //!< Mist coolant, optional.
                shower         :1, //!< Shower coolant, currently unused.
                trough_spindle :1, //!< Through spindle coolant, currently unused.
                unused         :4;
    };
} coolant_state_t;

// Define spindle stop override control states.
typedef union {
    uint8_t value;
    struct {
        uint8_t enabled       :1,
                initiate      :1,
                restore       :1,
                restore_cycle :1,
                unassigned    :4;
    };
} spindle_stop_t;

typedef struct Machine_status_packet {
  uint8_t address;
  uint8_t machine_state;
  uint8_t alarm;
  uint8_t home_state;
  uint8_t feed_override;
  uint8_t spindle_override;
  uint8_t spindle_stop;
  coolant_state_t coolant_state;
  uint8_t jog_mode;
} Machine_status_packet;

Machine_status_packet *packet = (Machine_status_packet*) context.mem;

char *ram_ptr = (char*) &context.mem[0];
int character_sent;

// Our handler is called from the I2C ISR, so it must complete quickly. Blocking calls /
// printing to stdio may interfere with interrupt handling.
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
    case I2C_SLAVE_RECEIVE: // master has written some data
        if (!context.mem_address_written) {
            // writes always start with the memory address
            context.mem_address = i2c_read_byte(i2c);
            context.mem_address_written = true;
        } else {
            // save into memory
            context.mem[context.mem_address] = i2c_read_byte(i2c);
            context.mem_address++;
        }
        break;
    case I2C_SLAVE_REQUEST: // master is requesting data
        // load from memory
        i2c_write_byte(i2c, context.mem[context.mem_address]);
        context.mem_address++;
        break;
    case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
        context.mem_address_written = false;
        update_main_screen = 1;
        break;
    default:
        break;
    }
}

static void setup_slave() {
    gpio_init(I2C_SLAVE_SDA_PIN);
    gpio_set_function(I2C_SLAVE_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SLAVE_SDA_PIN);

    gpio_init(I2C_SLAVE_SCL_PIN);
    gpio_set_function(I2C_SLAVE_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SLAVE_SCL_PIN);

    i2c_init(i2c0, I2C_BAUDRATE);
    // configure I2C0 for slave mode
    i2c_slave_init(i2c0, I2C_SLAVE_ADDRESS, &i2c_slave_handler);
}

uint8_t adjust (uint8_t value) {
	if (NEO_BRIGHTNESS == 0) return value ;
	return ((value * neopixels_gamma8(NEO_BRIGHTNESS)) >> 8) ;
};

volatile bool timer_fired = false;

void update_host_status (void) {
  //maybe use key_pressed variable to avoid spamming?
  int timeout = I2C_TIMEOUT_VALUE;
  gpio_put(KPSTR_PIN, false);
  sleep_ms(2);
  context.mem[0] = '?';
  context.mem_address = 0;
  gpio_put(KPSTR_PIN, true);
  while (context.mem_address == 0 && timeout){
    sleep_us(100);
    gpio_put(ONBOARD_LED, !gpio_get_out_level(ONBOARD_LED));
    timeout = timeout - 1;}
  gpio_put(KPSTR_PIN, false);

  if(!timeout)//if timed out then host status is unknown
    packet->machine_state = 0xFF;

  return;
};

uint8_t keypad_sendchar (uint8_t character, bool clearpin, bool update_status) {
  //maybe use key_pressed variable to avoid spamming?
  int timeout = I2C_TIMEOUT_VALUE;

  //if (context.mem_address_written == false && context.mem_address>0){
    context.mem[0] = character;
    context.mem_address = 0;
    //sleep_us(10);
    gpio_put(KPSTR_PIN, true);
    //sleep_us(10);
    while (context.mem_address == 0 && timeout){
      sleep_us(100);
      gpio_put(ONBOARD_LED, !gpio_get_out_level(ONBOARD_LED));
      timeout = timeout - 1;}
    //sleep_ms(2);
    if (clearpin){
      sleep_ms(5);
      gpio_put(KPSTR_PIN, false);
    }

    if (update_status && (packet->machine_state != 0xFF)){
      update_host_status();
    }
  return true;
};

static void update_neopixels(void){

  //set override LEDS
  if (packet->feed_override > 100)
    feed_color[0] = ((packet->feed_override - 100) * 255)/100;
  else
    feed_color[0] = 0;
  if (packet->feed_override > 100)
    feed_color[1] = 255 - (((packet->feed_override - 100) * 255)/100);
  else
    feed_color[1] = 255- (((100-packet->feed_override) * 255)/100);
  if(packet->feed_override < 100)
    feed_color[2] = ((100 - packet->feed_override) * 255)/100;
  else
    feed_color[2] = 0;

  if (packet->spindle_override > 100)
    rpm_color[0] = ((packet->spindle_override - 100) * 255)/100;
  else
    rpm_color[0] = 0;
  if (packet->spindle_override > 100)
    rpm_color[1] = 255 - (((packet->spindle_override - 100) * 255)/100);
  else
    rpm_color[1] = 255- (((100-packet->spindle_override) * 255)/100);
  if(packet->spindle_override < 100)
    rpm_color[2] = ((100 - packet->spindle_override) * 255)/100;
  else
    rpm_color[2] = 0;    

  pixels.setPixelColor(FEEDLED,pixels.Color((uint8_t) feed_color[0], (uint8_t) feed_color[1], (uint8_t) feed_color[2]));
  pixels.setPixelColor(SPINLED,pixels.Color(rpm_color[0], rpm_color[1], rpm_color[2]));

  //set home LED
  if(packet->home_state)
    pixels.setPixelColor(HOMELED,pixels.Color(0, 255, 0));
  else
    pixels.setPixelColor(HOMELED,pixels.Color(200, 135, 0));

  //set spindleoff LED
  if(packet->spindle_stop)
    pixels.setPixelColor(SPINDLELED,pixels.Color(255, 75, 0));
  else
    pixels.setPixelColor(SPINDLELED,pixels.Color(75, 255, 130));

  //set Coolant LED
  if(packet->coolant_state.value)
    pixels.setPixelColor(COOLED,pixels.Color(0, 100, 255));
  else
    pixels.setPixelColor(COOLED,pixels.Color(0, 0, 100));  

  //preload jog LED colors depending on speed
  switch (current_jogmode) {
  case FAST :
    jog_color[0] = 255; jog_color[1] = 0; jog_color[2] = 0; //RGB
    break;
  case SLOW : 
    jog_color[0] = 0; jog_color[1] = 255; jog_color[2] = 0; //RGB      
    break;
  case STEP : 
    jog_color[0] = 0; jog_color[1] = 0; jog_color[2] = 255; //RGB      
    break;
  default :
    jog_color[0] = 255; jog_color[1] = 255; jog_color[2] = 255; //RGB      
  }//close jogmode

  switch (packet->machine_state){
    case STATE_IDLE :
    //no change to jog colors
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB   
    break; //close idle case

    case STATE_HOLD :
    //no jog during hold to jog colors
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB
      jog_color[0] = 0; jog_color[1] = 0; jog_color[2] = 0; //RGB     
    break; //close idle case

    case STATE_TOOL_CHANGE : 
    //no change to jog colors
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB   
    break;//close tool change case 

    case STATE_JOG :
        //Indicate jog in progress
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB        
      jog_color[0] = 255; jog_color[1] = 150; jog_color[2] = 0; //RGB
    break;//close jog case

    case STATE_HOMING :
        //No jogging during homing
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB        
      jog_color[0] = 0; jog_color[1] = 0; jog_color[2] = 0; //RGB    
    break;//close homing case

    case STATE_CYCLE :
      //No jogging during job
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB        
      jog_color[0] = 0; jog_color[1] = 0; jog_color[2] = 0; //RGB     

    break;//close cycle state

    case STATE_ALARM :
          //handle alarm state at bottom
      jog_color[0] = 0; jog_color[1] = 0; jog_color[2] = 0; //RGB
      run_color[0] = 255; run_color[1] = 0; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 0; hold_color[2] = 0; //RGB   
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB 

      //also override above colors for maximum alarm 
      pixels.setPixelColor(SPINDLELED,pixels.Color(255, 0, 0));
      pixels.setPixelColor(COOLED,pixels.Color(255, 0, 0));
      pixels.setPixelColor(HOMELED,pixels.Color(255, 0, 0));
      pixels.setPixelColor(SPINLED,pixels.Color(255, 0, 0));
      pixels.setPixelColor(FEEDLED,pixels.Color(255, 0, 0));
    break;//close alarm state

    default :  //this is active when there is a non-interactive controller
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB   

    break;//close alarm state
  }//close machine_state switch statement

  //set jog LED values
  pixels.setPixelColor(JOGLED,pixels.Color(jog_color[0], jog_color[1], jog_color[2]));
  pixels.setPixelColor(RAISELED,pixels.Color(jog_color[0], jog_color[1], jog_color[2]));
  pixels.setPixelColor(HALTLED,pixels.Color(halt_color[0], halt_color[1], halt_color[2]));
  pixels.setPixelColor(HOLDLED,pixels.Color(hold_color[0], hold_color[1], hold_color[2]));
  pixels.setPixelColor(RUNLED,pixels.Color(run_color[0], run_color[1], run_color[2]));   

  pixels.show();
}

static void show_ram(void){ 
  #ifdef SHOWRAM
  
  int i = 0;
  int j = 0;
  char charbuf[32];

  if (gpio_get(MISTBUTTON)){
    oledFill(&oled, 0,1);
    sprintf(charbuf, "addr: %d", context.mem_address);
    oledWriteString(&oled, 0,0,0,(char*)charbuf, FONT_6x8, 0, 1);

    sprintf(charbuf, "Current RAM: %d", context.mem_address);
    oledWriteString(&oled, 0,0,0,(char*)charbuf, FONT_6x8, 0, 1);

    sprintf(charbuf, "State: %d", packet->machine_state);
    oledWriteString(&oled, 0,0,1,(char*)charbuf, FONT_6x8, 0, 1);

    sprintf(charbuf, "Alarm: %d", packet->alarm);
    oledWriteString(&oled, 0,0,2,(char*)charbuf, FONT_6x8, 0, 1);  

    sprintf(charbuf, "Homed: %d", packet->home_state);
    oledWriteString(&oled, 0,0,3,(char*)charbuf, FONT_6x8, 0, 1);

    sprintf(charbuf, "Feed: %d", packet->feed_override);
    oledWriteString(&oled, 0,0,4,(char*)charbuf, FONT_6x8, 0, 1);

    sprintf(charbuf, "RPM: %d", packet->spindle_override);
    oledWriteString(&oled, 0,0,5,(char*)charbuf, FONT_6x8, 0, 1);

    sprintf(charbuf, "Spinstop: %d", packet->spindle_stop);
    oledWriteString(&oled, 0,0,6,(char*)charbuf, FONT_6x8, 0, 1);  

    sprintf(charbuf, "Cool: %d", packet->coolant_state);
    oledWriteString(&oled, 0,0,7,(char*)charbuf, FONT_6x8, 0, 1);

    sprintf(charbuf, " Jogmode:%d ", context.mem[0]);
    oledWriteString(&oled,-1,-1,7,(char*)charbuf, FONT_6x8, 0, 1);
  }
#endif
}

static void draw_main_screen(){ 
  static Machine_status_packet previous_packet;
  static Jogmode previous_jogmode;
  int i = 0;
  int j = 0;
  char charbuf[32];

  #define JOGLINE 4
  #define JOGFONT FONT_12x16
  #define INFOLINE 2
  #define INFOFONT FONT_8x8

  if(packet->machine_state != previous_packet.machine_state || 
     packet->feed_override != previous_packet.feed_override ||
     packet->spindle_override != previous_packet.spindle_override||
     current_jogmode != previous_jogmode){
    
    oledFill(&oled, 0,1);

    switch (packet->machine_state){
      case STATE_IDLE : //jogging is allowed
      oledWriteString(&oled, 0,0,0,(char *)" Spin DOWN/RESET/UP", FONT_6x8, 0, 1);
      oledWriteString(&oled, 0,0,7,(char *)" Feed DOWN/RESET/UP", FONT_6x8, 0, 1);
      switch (current_jogmode) {
        case FAST :
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG:*FAST*", JOGFONT, 0, 1);
          break;
        case SLOW : 
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: SLOW", JOGFONT, 0, 1); 
          break;
        case STEP : 
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: STEP", JOGFONT, 0, 1);
          break;
        default :
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: ERROR", JOGFONT, 0, 1); 
          }//close jog states
      sprintf(charbuf, "RP:%d ", packet->spindle_override);
      oledWriteString(&oled, 0,0,INFOLINE,charbuf, INFOFONT, 0, 1);
      sprintf(charbuf, "  FD:%d", packet->feed_override);
      oledWriteString(&oled, -1,-1,INFOLINE,charbuf, INFOFONT, 0, 1);      
      break;//close idle state

      case STATE_HOLD :
        //can still adjust overrides during hold
        oledWriteString(&oled, 0,0,0,(char *)" Spin DOWN/RESET/UP", FONT_6x8, 0, 1);
        oledWriteString(&oled, 0,0,7,(char *)" Feed DOWN/RESET/UP", FONT_6x8, 0, 1);
        //no jog during hold
        oledWriteString(&oled, 0,0,JOGLINE,(char *)"HOLDING", JOGFONT, 0, 1);
        //can still adjust overrides during hold
        sprintf(charbuf, "RP:%d ", packet->spindle_override);
        oledWriteString(&oled, 0,0,INFOLINE,charbuf, INFOFONT, 0, 1);
        sprintf(charbuf, "  FD:%d", packet->feed_override);
        oledWriteString(&oled, -1,-1,INFOLINE,charbuf, INFOFONT, 0, 1);  
      break; //close hold case

      case STATE_TOOL_CHANGE :
        //dream feature is to put tool info/comment/number on screen during tool change.

        //cannot adjust overrides during tool change
        oledWriteString(&oled, 0,0,0,(char *)" *****************", FONT_6x8, 0, 1);
        oledWriteString(&oled, 0,0,7,(char *)" *****************", FONT_6x8, 0, 1);
        //jogging allowed during tool change
        switch (current_jogmode) {
          case FAST :
            oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG:*FAST*", JOGFONT, 0, 1);
            break;
          case SLOW : 
            oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: SLOW", JOGFONT, 0, 1); 
            break;
          case STEP : 
            oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: STEP", JOGFONT, 0, 1);
            break;
          default :
            oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: ERROR", JOGFONT, 0, 1); 
            }//close jog states
        //cannot adjust overrides
        oledWriteString(&oled, 0,0,INFOLINE,(char *)" TOOL CHANGE", INFOFONT, 0, 1);
      break; //close tool case

      case STATE_JOG : //jogging is allowed
      oledWriteString(&oled, 0,0,0,(char *)" Spin DOWN/RESET/UP", FONT_6x8, 0, 1);
      oledWriteString(&oled, 0,0,7,(char *)" Feed DOWN/RESET/UP", FONT_6x8, 0, 1);
      switch (current_jogmode) {
        case FAST :
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG:*FAST*", JOGFONT, 0, 1);
          break;
        case SLOW : 
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: SLOW", JOGFONT, 0, 1); 
          break;
        case STEP : 
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: STEP", JOGFONT, 0, 1);
          break;
        default :
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: ERROR", JOGFONT, 0, 1); 
          }//close jog states
        oledWriteString(&oled, 0,0,INFOLINE,(char *)" JOGGING", INFOFONT, 0, 1);   
      break;//close jog state

      case STATE_HOMING : //no overrides during homing
        oledWriteString(&oled, 0,0,0,(char *)" *****************", FONT_6x8, 0, 1);
        oledWriteString(&oled, 0,0,7,(char *)" *****************", FONT_6x8, 0, 1);
        //no jog during hold
        oledWriteString(&oled, 0,0,JOGLINE,(char *)"HOMING", JOGFONT, 0, 1);
      break; //close home case

      case STATE_CYCLE :
        //can still adjust overrides during hold
        oledWriteString(&oled, 0,0,0,(char *)" Spin DOWN/RESET/UP", FONT_6x8, 0, 1);
        oledWriteString(&oled, 0,0,7,(char *)" Feed DOWN/RESET/UP", FONT_6x8, 0, 1);
        //no jog during hold
        oledWriteString(&oled, 0,0,JOGLINE,(char *)"CYCLE RUN", JOGFONT, 0, 1);
        //can still adjust overrides during hold
        sprintf(charbuf, "RP:%d ", packet->spindle_override);
        oledWriteString(&oled, 0,0,INFOLINE,charbuf, INFOFONT, 0, 1);
        sprintf(charbuf, "  FD:%d", packet->feed_override);
        oledWriteString(&oled, -1,-1,INFOLINE,charbuf, INFOFONT, 0, 1);  
      break; //close cycle case

      case STATE_ALARM : //no overrides during homing
        oledWriteString(&oled, 0,0,0,(char *)" *****************", FONT_6x8, 0, 1);
        oledWriteString(&oled, 0,0,7,(char *)" *****************", FONT_6x8, 0, 1);
        //no jog during hold
        oledWriteString(&oled, 0,0,JOGLINE,(char *)"ALARM", JOGFONT, 0, 1);
        sprintf(charbuf, "Code: %d ", packet->alarm);
        oledWriteString(&oled, 0,0,INFOLINE,charbuf, INFOFONT, 0, 1);        
      break; //close home case                               

      default :
      oledWriteString(&oled, 0,0,0,(char *)" Spin DOWN/RESET/UP", FONT_6x8, 0, 1);
      oledWriteString(&oled, 0,0,7,(char *)" Feed DOWN/RESET/UP", FONT_6x8, 0, 1);
      switch (current_jogmode) {
        case FAST :
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG:*FAST*", JOGFONT, 0, 1);
          break;
        case SLOW : 
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: SLOW", JOGFONT, 0, 1); 
          break;
        case STEP : 
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: STEP", JOGFONT, 0, 1);
          break;
        default :
          oledWriteString(&oled, 0,0,JOGLINE,(char *)"JOG: ERROR", JOGFONT, 0, 1);  
        }//close jogmode
        oledWriteString(&oled, 0,0,INFOLINE,(char *)"Default State", INFOFONT, 0, 1);
      break; //close default case
    }//close machine_state switch statement
  }//close previous packet if statement

  previous_packet = *packet;
  previous_jogmode = current_jogmode;
}//close draw main screen

bool tick_timer_callback(struct repeating_timer *t) {
    if (onboard_led_count == 0){
    gpio_put(ONBOARD_LED, !gpio_get_out_level(ONBOARD_LED));                // toggle the LED
    onboard_led_count = 20;  //thi ssets the heartbeat
      character_sent = 1;
    }else{
    onboard_led_count = onboard_led_count - 1;
    }

    status_update_counter = status_update_counter - 1;
    if (status_update_counter < 0){
      status_update_counter = 0;
    }

    led_update_counter = led_update_counter - 1;
    if (led_update_counter <= 0){
        update_neopixel_leds = 1;
        led_update_counter = LED_UPDATE_PERIOD;
    }

    keypad_update_counter = keypad_update_counter - 1;
    if (keypad_update_counter < 0){
      keypad_update_counter = 0;
    }   
    
    return true;
}

// Main loop - initilises system and then loops while interrupts get on with processing the data

/*
red = (255, 0, 0)
orange = (255, 165, 0)
yellow = (255, 150, 0)
green = (0, 255, 0)
blue = (0, 0, 255)
indigo = (75, 0, 130)
violet = (138, 43, 226)
off = (0, 0, 0)
*/

int main() {

  stdio_init_all();

  gpio_init(KPSTR_PIN);
  gpio_set_dir(KPSTR_PIN, GPIO_OUT);

  gpio_init(HALTBUTTON);
  gpio_set_dir(HALTBUTTON, GPIO_IN);
  gpio_set_pulls(HALTBUTTON,true,false);

  gpio_init(UPBUTTON);
  gpio_set_dir(UPBUTTON, GPIO_IN);
  gpio_set_pulls(UPBUTTON,true,false);

  gpio_init(DOWNBUTTON);
  gpio_set_dir(DOWNBUTTON, GPIO_IN);
  gpio_set_pulls(DOWNBUTTON,true,false);

  gpio_init(LEFTBUTTON);
  gpio_set_dir(LEFTBUTTON, GPIO_IN);
  gpio_set_pulls(LEFTBUTTON,true,false);

  gpio_init(RIGHTBUTTON);
  gpio_set_dir(RIGHTBUTTON, GPIO_IN);
  gpio_set_pulls(RIGHTBUTTON,true,false);

  gpio_init(LOWERBUTTON);
  gpio_set_dir(LOWERBUTTON, GPIO_IN);
  gpio_set_pulls(LOWERBUTTON,true,false);

  gpio_init(RAISEBUTTON);
  gpio_set_dir(RAISEBUTTON, GPIO_IN);
  gpio_set_pulls(RAISEBUTTON,true,false);

  gpio_init(JOG_SELECT);
  gpio_set_dir(JOG_SELECT, GPIO_IN);
  gpio_set_pulls(JOG_SELECT,true,false);

  gpio_init(FEEDOVER_UP);
  gpio_set_dir(FEEDOVER_UP, GPIO_IN);
  gpio_set_pulls(FEEDOVER_UP,true,false);
  
  gpio_init(FEEDOVER_DOWN);
  gpio_set_dir(FEEDOVER_DOWN, GPIO_IN);
  gpio_set_pulls(FEEDOVER_DOWN,true,false);

  gpio_init(FEEDOVER_RESET);
  gpio_set_dir(FEEDOVER_RESET, GPIO_IN);
  gpio_set_pulls(FEEDOVER_RESET,true,false);

  gpio_init(SPINDLEBUTTON);
  gpio_set_dir(SPINDLEBUTTON, GPIO_IN);
  gpio_set_pulls(SPINDLEBUTTON,true,false);

  gpio_init(RUNBUTTON);
  gpio_set_dir(RUNBUTTON, GPIO_IN);
  gpio_set_pulls(RUNBUTTON,true,false);

  gpio_init(HOLDBUTTON);
  gpio_set_dir(HOLDBUTTON, GPIO_IN);
  gpio_set_pulls(HOLDBUTTON,true,false);

  gpio_init(MISTBUTTON);
  gpio_set_dir(MISTBUTTON, GPIO_IN);
  gpio_set_pulls(MISTBUTTON,true,false);

  gpio_init(FLOODBUTTON);
  gpio_set_dir(FLOODBUTTON, GPIO_IN);
  gpio_set_pulls(FLOODBUTTON,true,false);

  gpio_init(HOMEBUTTON);
  gpio_set_dir(HOMEBUTTON, GPIO_IN);
  gpio_set_pulls(HOMEBUTTON,true,false);

  gpio_init(SPINOVER_UP);
  gpio_set_dir(SPINOVER_UP, GPIO_IN);
  gpio_set_pulls(SPINOVER_UP,true,false);

  gpio_init(SPINOVER_DOWN);
  gpio_set_dir(SPINOVER_DOWN, GPIO_IN);
  gpio_set_pulls(SPINOVER_DOWN,true,false);

  gpio_init(SPINOVER_RESET);
  gpio_set_dir(SPINOVER_RESET, GPIO_IN);
  gpio_set_pulls(SPINOVER_RESET,true,false);

  gpio_init(ONBOARD_LED);
  gpio_set_dir(ONBOARD_LED, GPIO_OUT);
  gpio_put(ONBOARD_LED, 1);
  sleep_ms(250);
  gpio_put(ONBOARD_LED, 0);
  sleep_ms(250);
  gpio_put(ONBOARD_LED, 1);

  struct repeating_timer timer;
  add_repeating_timer_ms(TICK_TIMER_PERIOD, tick_timer_callback, NULL, &timer);
  
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightnessFunctions(adjust,adjust,adjust,adjust);
  
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.setPixelColor(JOGLED,pixels.Color(255, 255, 255));
  pixels.setPixelColor(RAISELED,pixels.Color(255, 255, 255)); 
  pixels.setPixelColor(HALTLED,pixels.Color(255, 255, 255));
  pixels.setPixelColor(FEEDLED,pixels.Color(255, 255, 255));
  pixels.setPixelColor(SPINLED,pixels.Color(255, 255, 255));
  pixels.setPixelColor(COOLED,pixels.Color(255, 255, 255));
  pixels.setPixelColor(HOLDLED,pixels.Color(255, 255, 255));
  pixels.setPixelColor(RUNLED,pixels.Color(255, 255, 255));
  pixels.setPixelColor(SPINDLELED,pixels.Color(255, 255, 255));
  pixels.setPixelColor(HOMELED,pixels.Color(255, 255, 255));
  pixels.show();   // Send the updated pixel colors to the hardware.


// Setup I2C0 as slave (peripheral)
setup_slave();
packet->machine_state = 0;
key_character = '?';
keypad_sendchar (key_character, 1, 1);
status_update_counter = STATUS_REQUEST_PERIOD;

uint8_t uc[8];
int i, j;
char szTemp[32];
    
rc = oledInit(&oled, OLED_128x64, 0x3c, OLED_SCREEN_FLIP, 0, 0, SDA_PIN, SCL_PIN, RESET_PIN, 1000000L);
oledSetBackBuffer(&oled, ucBuffer);
oledFill(&oled, 0,1);
oledWriteString(&oled, 0,0,1,(char *)"GRBLHAL", FONT_12x16, 0, 1);
oledWriteString(&oled, 0,0,4,(char *)"I2C JOGGER", FONT_12x16, 0, 1);
sleep_ms(750);
oledFill(&oled, 0,1);

draw_main_screen();

    // Main loop handles the buttons, everything else handled in interrupts
    while (true) {

        if (packet->machine_state != 0xFF)
          current_jogmode = (Jogmode) packet->jog_mode;

        if (update_main_screen){//used to update screen after status packet is received
          draw_main_screen();
          update_main_screen = 0;}

        if (update_neopixel_leds){
          update_neopixels();
          update_neopixel_leds = 0;
        }

        if(status_update_counter < 1){
          update_host_status();
        }

        //BUTTON READING ***********************************************************************
        if(gpio_get(HALTBUTTON)){
          pixels.setPixelColor(HALTLED,pixels.Color(0, 0, 0));
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"HALT", FONT_12x16, 0, 1);
          #endif           
          key_character = 0x18;
          //keypad_sendchar(key_character, 0);
          //do nothine else while button is pressed.
          //pixels.show();
          while(gpio_get(HALTBUTTON))
            sleep_ms(250);
          //gpio_put(KPSTR_PIN, false);        
        } else if (gpio_get(HOLDBUTTON)){
          pixels.setPixelColor(HOLDLED,pixels.Color(0, 0, 0));
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"FEED HOLD", FONT_12x16, 0, 1);
          #endif             
          key_character = '!';
          keypad_sendchar(key_character, 0, 1);
          //pixels.show();  
          while(gpio_get(HOLDBUTTON))
            sleep_us(100);
          gpio_put(KPSTR_PIN, false);
                                
        } else if (gpio_get(RUNBUTTON)){
          pixels.setPixelColor(RUNLED,pixels.Color(0, 0, 0));
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"RUN!", FONT_12x16, 0, 1);
          #endif             
          key_character = '~';
          keypad_sendchar(key_character, 0, 1);
          //pixels.show(); 
          while(gpio_get(RUNBUTTON))
            sleep_us(100);
          gpio_put(KPSTR_PIN, false);
                

        //misc commands.  These activate on lift
        } else if (gpio_get(SPINOVER_UP)){  
          spin_up_pressed = 1;
          #ifdef SHOWOVER
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"SPIN UP", FONT_12x16, 0, 1);
          #endif
          //pixels.setPixelColor(SPINLED,pixels.Color(0, 0, 0));
          //pixels.show();                 
        } else if (gpio_get(SPINOVER_DOWN)){  
          spin_down_pressed = 1;
          #ifdef SHOWOVER
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"SPIN DOWN", FONT_12x16, 0, 1);
          #endif
          //pixels.setPixelColor(SPINLED,pixels.Color(0, 0, 0));
          //pixels.show();  
        } else if (gpio_get(SPINOVER_RESET)){  
          spin_reset_pressed = 1;
          #ifdef SHOWOVER
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"SPIN RESET", FONT_12x16, 0, 1);
          #endif
          //pixels.setPixelColor(SPINLED,pixels.Color(0, 0, 0));
          //pixels.show();  
        } else if (gpio_get(FEEDOVER_UP)){ 
          feed_up_pressed = 1;
          #ifdef SHOWOVER
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"FEED UP", FONT_12x16, 0, 1);
          #endif
          //pixels.setPixelColor(FEEDLED,pixels.Color(0, 0, 0));
          //pixels.show();         
        } else if (gpio_get(FEEDOVER_DOWN)){ 
          feed_down_pressed = 1;
          #ifdef SHOWOVER
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"FEED DOWN", FONT_12x16, 0, 1);
          #endif
          //pixels.setPixelColor(FEEDLED,pixels.Color(0, 0, 0));
          //pixels.show();                
        } else if (gpio_get(FEEDOVER_RESET)){  
          feed_reset_pressed = 1;
          #ifdef SHOWOVER
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"FEED RESET", FONT_12x16, 0, 1);
          #endif
          //pixels.setPixelColor(FEEDLED,pixels.Color(0, 0, 0));
          //pixels.show();                
        } else if (gpio_get(HOMEBUTTON)){  
          home_pressed = 1;
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"HOME", FONT_12x16, 0, 1);
          #endif
          pixels.setPixelColor(HOMELED,pixels.Color(0, 0, 0));
          //pixels.show();           
        } else if (gpio_get(MISTBUTTON)){  //Toggle Jog modes
          mist_pressed = 1;
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"MIST", FONT_12x16, 0, 1);
          #endif
          pixels.setPixelColor(COOLED,pixels.Color(0, 0, 0));
          //pixels.show(); 
        } else if (gpio_get(FLOODBUTTON)){  //Toggle Jog modes
          flood_pressed = 1;
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"FLOOD", FONT_12x16, 0, 1);
          #endif
          pixels.setPixelColor(COOLED,pixels.Color(0, 0, 0));
          //pixels.show();     
        } else if (gpio_get(SPINDLEBUTTON)){  //Toggle Jog modes
          spinoff_pressed = 1;
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"SPINOFF", FONT_12x16, 0, 1);
          #endif
          pixels.setPixelColor(SPINDLELED,pixels.Color(0, 0, 0));
          //pixels.show();  

        //real-time jogging control
        } else if (gpio_get(JOG_SELECT)){  //Toggle Jog modes
          jog_toggle_pressed = 1;
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"JOG TOGGLE", FONT_12x16, 0, 1);
          #endif
          while(gpio_get(JOG_SELECT)){
            sleep_ms(10);
            show_ram(); }       
        }  else if (gpio_get(RAISEBUTTON)!= false){
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"RAISE", FONT_12x16, 0, 1);
          #endif
          key_character = 'U';
          keypad_sendchar(key_character, 0, 0);
          gpio_put(ONBOARD_LED,1);
          //pixels.show(); 
          while(gpio_get(RAISEBUTTON)){
            sleep_ms(10);
            draw_main_screen();
            update_neopixels();}
          gpio_put(KPSTR_PIN, false);
                        
        } else if (gpio_get(LOWERBUTTON)!= false){
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"LOWER", FONT_12x16, 0, 1);
          #endif           
          key_character = 'D';
          keypad_sendchar(key_character, 0, 0);
          gpio_put(ONBOARD_LED,1);
          //pixels.show();
          while(gpio_get(LOWERBUTTON)){
            sleep_ms(10);
            draw_main_screen();
            update_neopixels();}
          gpio_put(KPSTR_PIN, false);
                          
        } else if (gpio_get(UPBUTTON) != false){
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"Y UP", FONT_12x16, 0, 1);
          #endif           
          key_character = 'F';
          keypad_sendchar(key_character, 0, 0);
          gpio_put(ONBOARD_LED,1);
          //pixels.show(); 
          while(gpio_get(UPBUTTON)){
            sleep_ms(10);
            draw_main_screen();
            update_neopixels();}
          gpio_put(KPSTR_PIN, false);
                        
        } else if (gpio_get(DOWNBUTTON) != false){
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"Y DOWN", FONT_12x16, 0, 1);
          #endif           
          key_character = 'B';
          keypad_sendchar(key_character, 0, 0);
          gpio_put(ONBOARD_LED,1);
          //pixels.show();
          while(gpio_get(DOWNBUTTON)){
            sleep_ms(10);
            draw_main_screen();
            update_neopixels();}
          gpio_put(KPSTR_PIN, false);
                           
        } else if (gpio_get(LEFTBUTTON) != false){
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"X LEFT", FONT_12x16, 0, 1);
          #endif           
          key_character = 'L';
          keypad_sendchar(key_character, 0, 0);
          gpio_put(ONBOARD_LED,1);
          //pixels.show();
          while(gpio_get(LEFTBUTTON)){
            sleep_ms(10);
            draw_main_screen();
            update_neopixels();}
          gpio_put(KPSTR_PIN, false);
                           
        } else if (gpio_get(RIGHTBUTTON) != false){
          #ifdef SHOWJOG
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,2,(char *)"X RIGHT", FONT_12x16, 0, 1);
          #endif           
          key_character = 'R';
          keypad_sendchar(key_character, 0, 0);
          gpio_put(ONBOARD_LED,1);
          while(gpio_get(RIGHTBUTTON)){
            sleep_ms(10);
            draw_main_screen();
            update_neopixels();}
          gpio_put(KPSTR_PIN, false);          
          //on d-pad need to add handlers for the other buttons to do diagonal?
        } else {
            key_character = '\0';
            context.mem[0] = key_character;
          if (status_update_counter < 1){
            //key_character = '?';
            //keypad_sendchar (key_character, 1, 0);
            status_update_counter = STATUS_REQUEST_PERIOD;
            draw_main_screen();
            update_neopixels();
          }                  
          }//close button reads

//SINGLE BUTTON PRESSES ***********************************************************************
        if (jog_toggle_pressed){  //acts only on lift
          if (gpio_get(JOG_SELECT)){}//button is still pressed, do nothing              
          else{
            switch (current_jogmode) {
              case FAST :
                key_character = '1';
                current_jogmode = SLOW;
              break;
              case SLOW :
                key_character = '2';
                current_jogmode = STEP;
              break;
              case STEP :
                key_character = '0';
                current_jogmode = FAST;
              break;
              default :
                key_character = '1';
                current_jogmode = SLOW;
          }//close switch statement          
          keypad_sendchar (key_character, 1, 1);
          gpio_put(KPSTR_PIN, false);
          gpio_put(ONBOARD_LED,1);
          jog_toggle_pressed = 0;
          update_neopixels();
          draw_main_screen();
          }
        }//close jog pressed statement

        if (feed_down_pressed) {
          if (gpio_get(FEEDOVER_DOWN)){}//button is still pressed, do nothing
          else{
            key_character = 0x92;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            feed_down_pressed = 0;
            update_neopixels();
            draw_main_screen();                        
          }
        }
        if (feed_up_pressed) {
          if (gpio_get(FEEDOVER_UP)){}//button is still pressed, do nothing
          else{
            key_character = 0x91;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            feed_up_pressed = 0;
            update_neopixels();
            draw_main_screen();                
          }
        }
        if (feed_reset_pressed) {
          if (gpio_get(FEEDOVER_RESET)){}//button is still pressed, do nothing
          else{
            key_character = 0x90;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            feed_reset_pressed = 0;
            update_neopixels();
            draw_main_screen();                 
          }
        }
        if (spin_down_pressed) {
          if (gpio_get(SPINOVER_DOWN)){}//button is still pressed, do nothing
          else{
            key_character = 0x9B;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            spin_down_pressed = 0;
            update_neopixels();
            draw_main_screen();            
          }
        }
        if (spin_up_pressed) {
          if (gpio_get(SPINOVER_UP)){}//button is still pressed, do nothing
          else{
            key_character = 0x9A;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            spin_up_pressed = 0;
            update_neopixels();
            draw_main_screen();            
          }
        }
        if (spin_reset_pressed) {
          if (gpio_get(SPINOVER_RESET)){}//button is still pressed, do nothing
          else{
            key_character = 0x99;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            spin_reset_pressed = 0;
            update_neopixels();
            draw_main_screen();            
          }
        }
        if (mist_pressed) {
          if (gpio_get(MISTBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = 'M';
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            mist_pressed = 0;
            update_neopixels();
            draw_main_screen();            
          }
        }
        if (flood_pressed) {
          if (gpio_get(FLOODBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = 'C';
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            flood_pressed = 0;
            update_neopixels();
            draw_main_screen();            
          }                    
        }
        if (spinoff_pressed) {
          if (gpio_get(SPINDLEBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = 0x9E;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            spinoff_pressed = 0;
            update_neopixels();
            draw_main_screen();            
          }                    
        } 
        if (home_pressed) {
          if (gpio_get(HOMEBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = 'H';
            //oledFill(&oled, 0,1);          
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            home_pressed = 0;         
            update_neopixels();
            draw_main_screen();            
          }                    
        }                                         

    }//close main while loop
    return 0;
}
