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
#include "hardware/flash.h"
#include "ss_oled.h"

#include "pico/stdio.h"
#include "pico/time.h"
#include <tusb.h>
#include "Adafruit_NeoPixel.hpp"

#include <i2c_fifo.h>
#include <i2c_slave.h>

#include <math.h>

#include "i2c_jogger.h"

//#define SHOWJOG 1
//#define SHOWOVER 1
#define SHOWRAM 1
#define TWOWAY 1

#define OLED_SCREEN_FLIP 1

#define TICK_TIMER_PERIOD 10
#define ROLLOVER_DELAY_PERIOD 7


uint8_t jog_color[] = {0,255,0};
uint8_t halt_color[] = {0,255,0};
uint8_t hold_color[] = {0,255,0};
uint8_t run_color[] = {0,255,0};
int32_t feed_color[] = {0,10000,0};
int32_t rpm_color[] = {0,10000,0};


Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// define I2C addresses to be used for this peripheral
static const uint I2C_SLAVE_ADDRESS = 0x49;
static const uint I2C_BAUDRATE = 100000; // 100 kHz

// GPIO pins to use for I2C SLAVE
static const uint I2C_SLAVE_SDA_PIN = 0;
static const uint I2C_SLAVE_SCL_PIN = 1;

// RPI Pico

int rc;
SSOLED oled;
static uint8_t ucBuffer[1024];
bool screenflip = false;
bool joggle_reset =false;

int led_update_counter = 0;
int update_neopixel_leds = 0;
int screen_update_counter = 0;
int status_update_counter = 0;
int onboard_led_count = 0;
int jogmode = 0;

int command_error = 0;

// ram_addr is the current address to be used when writing / reading the RAM
// N.B. the address auto increments, as stored in 8 bit value it automatically rolls round when reaches 255

static struct
{
    uint8_t mem[256];
    uint8_t version;
    uint8_t mem_address;
    bool mem_address_written;
} context;

char buf[8];

uint8_t ram_addr = 0;
uint8_t status_addr = 0;
uint8_t key_pressed = 0;
uint8_t key_character = '\0';

uint8_t feed_up_pressed = 0;
uint8_t feed_down_pressed = 0;
uint8_t feed_up_fine_pressed = 0;
uint8_t feed_down_fine_pressed = 0;
uint8_t feed_reset_pressed = 0;

uint8_t spin_up_pressed = 0;
uint8_t spin_down_pressed = 0;
uint8_t spin_up_fine_pressed = 0;
uint8_t spin_down_fine_pressed = 0;
uint8_t spin_reset_pressed = 0;

uint8_t jog_mod_pressed = 0;
uint8_t jog_mode_pressed = 0;
uint8_t jog_toggle_pressed = 0;

uint8_t mist_pressed = 0;
uint8_t flood_pressed = 0;
uint8_t spinoff_pressed = 0;
uint8_t home_pressed = 0;

uint8_t direction_pressed = 0;
uint8_t previous_direction_pressed = 0;
uint8_t keysent = 0;
int rollover_delay = 0;
int transition_delay = 0;

uint8_t macro_top_pressed = 0;
uint8_t macro_bot_pressed = 0;
uint8_t macro_left_pressed = 0;
uint8_t macro_right_pressed = 0;
uint8_t macro_lower_pressed = 0;
uint8_t macro_raise_pressed = 0;
uint8_t macro_home_pressed = 0;
uint8_t macro_spindle_pressed = 0;
uint8_t reset_pressed = 0;
uint8_t unlock_pressed = 0;
uint8_t halt_pressed = 0;

machine_status_packet_t *packet = (machine_status_packet_t*) context.mem;
machine_status_packet_t prev_packet;
machine_status_packet_t *previous_packet = &prev_packet;
//Jogmode previous_jogmode;
//Jogmodify previous_jogmodify;
ScreenMode previous_screenmode = DEFAULT;

char *ram_ptr = (char*) &context.mem[0];
int character_sent;

// Our handler is called from the I2C ISR, so it must complete quickly. Blocking calls /
// printing to stdio may interfere with interrupt handling.
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
    case I2C_SLAVE_RECEIVE: // master has written some data
        if (!context.mem_address_written) {
            // writes always start with the memory address
            context.version = i2c_read_byte(i2c); // read struct version TODO: error checking on version
            context.mem_address = context.version == 0x02 ? 0x01 : context.version; // set address to 1
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

uint8_t keypad_sendchar (uint8_t character, bool clearpin, bool update_status) {
  //maybe use key_pressed variable to avoid spamming?
  int timeout = I2C_TIMEOUT_VALUE;

  command_error = 0;

  gpio_put(ONBOARD_LED, 0);

  while (context.mem_address_written != false && context.mem_address>0);

  context.mem[0] = character;
  context.mem_address = 0;
  //gpio_put(KPSTR_PIN, false);
  sleep_us(1000);
  gpio_put(KPSTR_PIN, true);
  //sleep_us(100);
  while (context.mem_address == 0 && timeout){
    sleep_us(1);      
    timeout = timeout - 1;}

  if(!timeout)
    command_error = 1;
  //sleep_ms(2);
  if (clearpin){
    //sleep_ms(5);
    gpio_put(KPSTR_PIN, false);
  }
  gpio_put(ONBOARD_LED, 1);
  return true;
};

static void update_neopixels(void){

  if (context.mem_address_written || (packet->status_code == Status_UserException))// < offsetof(machine_status_packet_t, msgtype))
    return;
  
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
  if(packet->machine_modes.homed)
    pixels.setPixelColor(HOMELED,pixels.Color(0, 255, 0));
  else
    pixels.setPixelColor(HOMELED,pixels.Color(200, 135, 0));

  //set spindleoff LED
  if(packet->spindle_rpm > 0)
    pixels.setPixelColor(SPINDLELED,pixels.Color(255, 75, 0));
  else
    pixels.setPixelColor(SPINDLELED,pixels.Color(75, 255, 130));

  //set Coolant LED
  if(packet->coolant_state.value)
    pixels.setPixelColor(COOLED,pixels.Color(0, 100, 255));
  else
    pixels.setPixelColor(COOLED,pixels.Color(0, 0, 100));  

  //preload jog LED colors depending on speed
  switch (packet->jog_mode.mode) {
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
    //jog_color[0] = 255; jog_color[1] = 255; jog_color[2] = 255; //RGB
  break;      
  }//close jogmode

  switch (packet->system_state){
    case SystemState_Idle :
    //no change to jog colors
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB   
    break; //close idle case

    case SystemState_Hold :
    //no jog during hold to jog colors
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB
      jog_color[0] = 0; jog_color[1] = 0; jog_color[2] = 0; //RGB     
    break; //close idle case

    case SystemState_ToolChange : 
    //no change to jog colors
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB   
    break;//close tool change case 

    case SystemState_Jog :
        //Indicate jog in progress
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB        
      jog_color[0] = 255; jog_color[1] = 150; jog_color[2] = 0; //RGB
    break;//close jog case

    case SystemState_Homing :
        //No jogging during homing
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB        
      jog_color[0] = 0; jog_color[1] = 0; jog_color[2] = 0; //RGB    
    break;//close homing case

    case SystemState_Cycle :
      //No jogging during job
      run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB        
      jog_color[0] = 0; jog_color[1] = 0; jog_color[2] = 0; //RGB     
    break;//close cycle state

    case SystemState_Alarm :
          //handle alarm state at bottom
      jog_color[0] = 0; jog_color[1] = 0; jog_color[2] = 0; //RGB
      //run_color[0] = 255; run_color[1] = 0; run_color[2] = 0; //RGB
      //hold_color[0] = 255; hold_color[1] = 0; hold_color[2] = 0; //RGB   
      halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB 

      //also override above colors for maximum alarm 
      pixels.setPixelColor(SPINDLELED,pixels.Color(255, 0, 0));
      pixels.setPixelColor(COOLED,pixels.Color(255, 0, 0));
      pixels.setPixelColor(HOMELED,pixels.Color(255, 0, 0));
      pixels.setPixelColor(SPINLED,pixels.Color(255, 0, 0));
      pixels.setPixelColor(FEEDLED,pixels.Color(255, 0, 0));
    break;//close alarm state

    default :  //this is active when there is a non-interactive controller
      //run_color[0] = 0; run_color[1] = 255; run_color[2] = 0; //RGB
      //hold_color[0] = 255; hold_color[1] = 150; hold_color[2] = 0; //RGB
      //halt_color[0] = 255; halt_color[1] = 0; halt_color[2] = 0; //RGB   

    break;//close alarm state
  }//close system_state switch statement

  if(screenmode == JOG_MODIFY){
    //some overrides for alternate functions
    //violet = (138, 43, 226)
    //jog_color[0] = 138; jog_color[1] = 43; jog_color[2] = 226;
    //run_color[0] = 138; run_color[1] = 43; run_color[2] = 226; //RGB
    //hold_color[0] = 138; hold_color[1] = 43; hold_color[2] = 226; //RGB   
    //halt_color[0] = 0; halt_color[1] = 0; halt_color[2] = 0; //RGB 
    pixels.setPixelColor(COOLED,pixels.Color(138, 43, 226));
    pixels.setPixelColor(HOMELED,pixels.Color(138, 43, 226));
    //pixels.setPixelColor(FEEDLED,pixels.Color(0, 0, 0));
    //pixels.setPixelColor(SPINLED,pixels.Color(0, 0, 0));
  }

  //set jog LED values
  if(screenmode == JOG_MODIFY)
    pixels.setPixelColor(JOGLED,pixels.Color(138, 43, 226));
  else
    pixels.setPixelColor(JOGLED,pixels.Color(jog_color[0], jog_color[1], jog_color[2]));

  if(isnan(packet->coordinate.a) && screenmode == JOG_MODIFY)
    pixels.setPixelColor(RAISELED,pixels.Color(138, 43, 226));
  else
    pixels.setPixelColor(RAISELED,pixels.Color(jog_color[0], jog_color[1], jog_color[2]));
  pixels.setPixelColor(HALTLED,pixels.Color(halt_color[0], halt_color[1], halt_color[2]));
  pixels.setPixelColor(HOLDLED,pixels.Color(hold_color[0], hold_color[1], hold_color[2]));
  pixels.setPixelColor(RUNLED,pixels.Color(run_color[0], run_color[1], run_color[2]));

  pixels.show();
}

void activate_jogled(void){
  jog_color[0] = 255; jog_color[1] = 150; jog_color[2] = 0; //RGB
  pixels.setPixelColor(JOGLED,pixels.Color(jog_color[0], jog_color[1], jog_color[2]));
  pixels.setPixelColor(RAISELED,pixels.Color(jog_color[0], jog_color[1], jog_color[2]));
  pixels.show();
}

// Converts an uint32 variable to string.
char *uitoa (uint32_t n)
{
    char *bptr = buf + sizeof(buf);

    *--bptr = '\0';

    if (n == 0)
        *--bptr = '0';
    else while (n) {
        *--bptr = '0' + (n % 10);
        n /= 10;
    }

    return bptr;
}

static char *map_coord_system (coord_system_id_t id)
{
    uint8_t g5x = id + 54;

    strcpy(buf, uitoa((uint32_t)(g5x > 59 ? 59 : g5x)));
    if(g5x > 59) {
        strcat(buf, ".");
        strcat(buf, uitoa((uint32_t)(g5x - 59)));
    }

    return buf;
}

static void draw_main_screen(bool force){ 
  int i = 0;
  int j = 0;
  char charbuf[32];

  int x, y;

  #define JOGLINE 7
  #define JOGFONT FONT_8x8
  #define TOPLINE 2
  #define INFOLINE 0
  #define BOTTOMLINE 7
  #define INFOFONT FONT_8x8

  //while(context.mem_address < sizeof(Machine_status_packet));

  /*if(screenmode != previous_screenmode){
    oledFill(&oled, 0,1);  //only clear screen if the mode changes.
    force = 1;
  }*/
  
  switch (screenmode){
  case JOG_MODIFY:
  case JOGGING: 
  case RUN:   
  case DEFAULT:  

      /*if(packet->machine_state != previous_packet->machine_state)
        oledFill(&oled, 0,1);//clear screen on state change*/

      switch (packet->system_state){
        case SystemState_Jog : //jogging is allowed       
        case SystemState_Idle : //jogging is allowed
        if (packet->jog_mode.value!=previous_packet->jog_mode.value || packet->jog_stepsize!=previous_packet->jog_stepsize || force){
          sprintf(charbuf, "        : %3.3f ", packet->jog_stepsize * (packet->machine_modes.reports_imperial ? 0.03937f : 1.0f));
          oledWriteString(&oled, 0,0,INFOLINE,charbuf, INFOFONT, 0, 1);
          switch (packet->jog_mode.mode) {
            case FAST :
            case SLOW : 
              oledWriteString(&oled, 0,0,INFOLINE,(char *)"JOG FEED", INFOFONT, 0, 1); 
              break;
            case STEP : 
              oledWriteString(&oled, 0,0,INFOLINE,(char *)"JOG STEP", INFOFONT, 0, 1);
              break;
            default :
              //oledWriteString(&oled, 0,0,INFOLINE,(char *)"ERR ", INFOFONT, 0, 1);
            break; 
              }//close jog states
        }

        if (packet->current_wcs != previous_packet->current_wcs || force){
          oledWriteString(&oled, 0,0,2,(char *)"                G", FONT_6x8, 0, 1);
          oledWriteString(&oled, 0,-1,-1,map_coord_system(packet->current_wcs), FONT_6x8, 0, 1);
          oledWriteString(&oled, 0,-1,-1,(char *)"  ", FONT_6x8, 0, 1);
        }

        oledWriteString(&oled, 0,94,4,(char *)" ", FONT_6x8, 0, 1);
        switch (packet->system_state){
          case SystemState_Idle :
          oledWriteString(&oled, 0,-1,-1,(char *)"IDLE", FONT_6x8, 0, 1); 
          break;
          case SystemState_Jog :
          oledWriteString(&oled, 0,-1,-1,(char *)"JOG ", FONT_6x8, 0, 1);
          break;
          case SystemState_ToolChange :
          oledWriteString(&oled, 0,-1,-1,(char *)"TOOL", FONT_6x8, 0, 1); 
          break;                   
        }
        //oledWriteString(&oled, 0,0,5,(char *)"              ", FONT_6x8, 0, 1);
        //sprintf(charbuf, "%d %d %d  ", direction_pressed, previous_direction_pressed, transition_delay);
        //oledWriteString(&oled, 0,-1,-1,charbuf, FONT_6x8, 0, 1); 

        //oledWriteString(&oled, 2,0,2,(char *)"        ", FONT_8x8, 0, 1);
        if(packet->coordinate.x != previous_packet->coordinate.x || 
           packet->coordinate.y != previous_packet->coordinate.y || 
           packet->coordinate.z != previous_packet->coordinate.z || 
           packet->coordinate.a != previous_packet->coordinate.a || force){
          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "X %8.4F", packet->coordinate.x);
          else
            sprintf(charbuf, "X %8.3F", packet->coordinate.x);
          oledWriteString(&oled, 0,0,2,charbuf, FONT_8x8, 0, 1);
          //}
          //oledWriteString(&oled, 2,0,3,(char *)"        ", FONT_8x8, 0, 1);
          //if(packet->y_coordinate != previous_packet.y_coordinate || force){ 
          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "Y %8.4F", packet->coordinate.y);
          else
            sprintf(charbuf, "Y %8.3F", packet->coordinate.y);
          oledWriteString(&oled, 0,0,3,charbuf, FONT_8x8, 0, 1);
          //}
          //oledWriteString(&oled, 2,0,4,(char *)"        ", FONT_8x8, 0, 1);
          //if(packet->z_coordinate != previous_packet.z_coordinate || force){ 
          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "Z %8.4F", packet->coordinate.z);
          else
            sprintf(charbuf, "Z %8.3F", packet->coordinate.z);
          oledWriteString(&oled, 0,0,4,charbuf, FONT_8x8, 0, 1);
          //}
          if(!isnan(packet->coordinate.a)){          
            if(packet->machine_modes.reports_imperial == 1)
              sprintf(charbuf, "A %8.4F", packet->coordinate.a);
            else
              sprintf(charbuf, "A %8.3F", packet->coordinate.a);
            oledWriteString(&oled, 0,0,5,charbuf, FONT_8x8, 0, 1);
          }else if (command_error){
            sprintf(charbuf, "COMMAND ERR", packet->coordinate.a);
            oledWriteString(&oled, 0,0,5,charbuf, FONT_8x8, 0, 1);
            sleep_ms(100);            
          }else{
            sprintf(charbuf, "           ", packet->coordinate.a);
            oledWriteString(&oled, 0,0,5,charbuf, FONT_8x8, 0, 1);            
          }
        }          

        if(packet->machine_modes.mode == Mode_Laser)
          oledWriteString(&oled, 0,0,6,(char *)"                 PWR", FONT_6x8, 0, 1);
        else
          oledWriteString(&oled, 0,0,6,(char *)"                 RPM", FONT_6x8, 0, 1);

        sprintf(charbuf, "S:%3d  F:%3d    ", packet->spindle_override, packet->feed_override);
        oledWriteString(&oled, 0,0,BOTTOMLINE,charbuf, FONT_6x8, 0, 1);
        //this is the RPM number
        sprintf(charbuf, "%5d", packet->spindle_rpm);
        oledWriteString(&oled, 0,-1,-1,charbuf, FONT_6x8, 0, 1);            
        break;//close idle state

        case SystemState_Cycle :
          //can still adjust overrides during hold
          //no jog during hold, show feed rate.
          sprintf(charbuf, "        : %3.3f ", packet->feed_rate);
          oledWriteString(&oled, 0,0,INFOLINE,charbuf, INFOFONT, 0, 1);

          oledWriteString(&oled, 0,0,INFOLINE,(char *)"RUN FEED", INFOFONT, 0, 1); 

          oledWriteString(&oled, 0,0,2,(char *)"                G", FONT_6x8, 0, 1);
          oledWriteString(&oled, 0,-1,-1,map_coord_system(packet->current_wcs), FONT_6x8, 0, 1);   

          oledWriteString(&oled, 0,0,4,(char *)"                ", FONT_6x8, 0, 1);
          oledWriteString(&oled, 0,-1,-1,(char *)"RUN  ", FONT_6x8, 0, 1); 

          oledWriteString(&oled, 2,0,2,(char *)"        ", FONT_8x8, 0, 1); 
          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "X %8.4F", packet->coordinate.x);
          else
            sprintf(charbuf, "X %8.3F", packet->coordinate.x);
          oledWriteString(&oled, 0,0,2,charbuf, FONT_8x8, 0, 1);
          oledWriteString(&oled, 2,0,3,(char *)"        ", FONT_8x8, 0, 1); 
          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "Y %8.4F", packet->coordinate.y);
          else
            sprintf(charbuf, "Y %8.3F", packet->coordinate.y);
          oledWriteString(&oled, 0,0,3,charbuf, FONT_8x8, 0, 1);
          oledWriteString(&oled, 2,0,4,(char *)"        ", FONT_8x8, 0, 1); 
          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "Z %8.4F", packet->coordinate.z);
          else
            sprintf(charbuf, "Z %8.3F", packet->coordinate.z);
          oledWriteString(&oled, 0,0,4,charbuf, FONT_8x8, 0, 1);
          if(!isnan(packet->coordinate.a)){          
            if(packet->machine_modes.reports_imperial == 1)
              sprintf(charbuf, "A %8.4F", packet->coordinate.a);
            else
              sprintf(charbuf, "A %8.3F", packet->coordinate.a);
            oledWriteString(&oled, 0,0,5,charbuf, FONT_8x8, 0, 1);
          }else{
            sprintf(charbuf, "          ", packet->coordinate.a);
            oledWriteString(&oled, 0,0,5,charbuf, FONT_8x8, 0, 1);            
          }         

        if(packet->machine_modes.mode == Mode_Laser)
          oledWriteString(&oled, 0,0,6,(char *)"                 PWR", FONT_6x8, 0, 1);
        else
          oledWriteString(&oled, 0,0,6,(char *)"                 RPM", FONT_6x8, 0, 1);         

          sprintf(charbuf, "S:%3d  F:%3d    ", packet->spindle_override, packet->feed_override);
          oledWriteString(&oled, 0,0,BOTTOMLINE,charbuf, FONT_6x8, 0, 1);
          //this is the RPM number
          sprintf(charbuf, "%5d", packet->spindle_rpm);
          oledWriteString(&oled, 0,-1,-1,charbuf, FONT_6x8, 0, 1);    
        break; //close cycle case        

        case SystemState_Hold :
          //can still adjust overrides during hold
          //no jog during hold
          oledWriteString(&oled, 0,0,INFOLINE,(char *)"    HOLDING     ", JOGFONT, 0, 1);
          //can still adjust overrides during hold
          oledWriteString(&oled, 0,0,2,(char *)"                G", FONT_6x8, 0, 1);
          oledWriteString(&oled, 0,-1,-1,map_coord_system(packet->current_wcs), FONT_6x8, 0, 1);   

          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "X %8.4F", packet->coordinate.x);
          else
            sprintf(charbuf, "X %8.3F", packet->coordinate.x);
          oledWriteString(&oled, 0,0,2,charbuf, FONT_8x8, 0, 1); 
          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "Y %8.4F", packet->coordinate.y);
          else
            sprintf(charbuf, "Y %8.3F", packet->coordinate.y);
          oledWriteString(&oled, 0,0,3,charbuf, FONT_8x8, 0, 1); 
          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "Z %8.4F", packet->coordinate.z);
          else
            sprintf(charbuf, "Z %8.3F", packet->coordinate.z);
          oledWriteString(&oled, 0,0,4,charbuf, FONT_8x8, 0, 1);
          if(!isnan(packet->coordinate.a)){          
            if(packet->machine_modes.reports_imperial == 1)
              sprintf(charbuf, "A %8.4F", packet->coordinate.a);
            else
              sprintf(charbuf, "A %8.3F", packet->coordinate.a);
            oledWriteString(&oled, 0,0,5,charbuf, FONT_8x8, 0, 1);
          }else{
            sprintf(charbuf, "          ", packet->coordinate.a);
            oledWriteString(&oled, 0,0,5,charbuf, FONT_8x8, 0, 1);            
          }           

        if(packet->machine_modes.mode == Mode_Laser)
          oledWriteString(&oled, 0,0,6,(char *)"                 PWR", FONT_6x8, 0, 1);
        else
          oledWriteString(&oled, 0,0,6,(char *)"                 RPM", FONT_6x8, 0, 1);           

          sprintf(charbuf, "S:%3d  F:%3d    ", packet->spindle_override, packet->feed_override);
          oledWriteString(&oled, 0,0,BOTTOMLINE,charbuf, FONT_6x8, 0, 1);
          //this is the RPM number
          sprintf(charbuf, "%5d", packet->spindle_rpm);
          oledWriteString(&oled, 0,-1,-1,charbuf, FONT_6x8, 0, 1);                
        break; //close hold case

        case SystemState_ToolChange :
          //dream feature is to put tool info/comment/number on screen during tool change.
          //cannot adjust overrides during tool change
          //jogging allowed during tool change
          sprintf(charbuf, "        : %3.3f ", packet->jog_stepsize * (packet->machine_modes.reports_imperial ? 0.03937f : 1.0f));
          oledWriteString(&oled, 0,0,INFOLINE,charbuf, INFOFONT, 0, 1);
          switch (packet->jog_mode.mode) {
            case FAST :
            case SLOW : 
              oledWriteString(&oled, 0,0,INFOLINE,(char *)"JOG FEED", INFOFONT, 0, 1); 
              break;
            case STEP : 
              oledWriteString(&oled, 0,0,INFOLINE,(char *)"JOG STEP", INFOFONT, 0, 1);
              break;
            default :
            break; 
              }//close jog states
          oledWriteString(&oled, 0,0,2,(char *)"                G", FONT_6x8, 0, 1);
          oledWriteString(&oled, 0,-1,-1,map_coord_system(packet->current_wcs), FONT_6x8, 0, 1);             

          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "X %8.4F", packet->coordinate.x);
          else
            sprintf(charbuf, "X %8.3F", packet->coordinate.x);
          oledWriteString(&oled, 0,0,2,charbuf, FONT_8x8, 0, 1); 
          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "Y %8.4F", packet->coordinate.y);
          else
            sprintf(charbuf, "Y %8.3F", packet->coordinate.y);
          oledWriteString(&oled, 0,0,3,charbuf, FONT_8x8, 0, 1); 
          if(packet->machine_modes.reports_imperial == 1)
            sprintf(charbuf, "Z %8.4F", packet->coordinate.z);
          else
            sprintf(charbuf, "Z %8.3F", packet->coordinate.z);
          oledWriteString(&oled, 0,0,4,charbuf, FONT_8x8, 0, 1);         
          if(!isnan(packet->coordinate.a)){          
            if(packet->machine_modes.reports_imperial == 1)
              sprintf(charbuf, "A %8.4F", packet->coordinate.a);
            else
              sprintf(charbuf, "A %8.3F", packet->coordinate.a);
            oledWriteString(&oled, 0,0,5,charbuf, FONT_8x8, 0, 1);
          }else{
            sprintf(charbuf, "          ", packet->coordinate.a);
            oledWriteString(&oled, 0,0,5,charbuf, FONT_8x8, 0, 1);            
          }
          oledWriteString(&oled, 0,0,BOTTOMLINE,(char *)" TOOL CHANGE", INFOFONT, 0, 1);
        break; //close tool case

        case SystemState_Homing :
          //no overrides during homing
          if( (prev_packet.system_state != packet->system_state) )
          oledFill(&oled, 0,1);
          oledWriteString(&oled, 0,0,0,(char *)" *****************", FONT_6x8, 0, 1);
          oledWriteString(&oled, 0,0,7,(char *)" *****************", FONT_6x8, 0, 1);
          //no jog during hold
          oledWriteString(&oled, 0,0,4,(char *)"HOMING", JOGFONT, 0, 1);
        break; //close home case

        case SystemState_Alarm : 
          //only re-fill the screen if the state or alarm code have changed.
          if( (prev_packet.system_substate != packet->system_substate) || (prev_packet.system_state != packet->system_state) )
            oledFill(&oled, 0,1);
            prev_packet.system_substate = packet->system_substate;
          oledWriteString(&oled, 0,0,0,(char *)" *****************", FONT_6x8, 0, 1);
          oledWriteString(&oled, 0,0,7,(char *)" *****************", FONT_6x8, 0, 1);
          //no jog during hold
          oledWriteString(&oled, 0,0,3,(char *)"ALARM", JOGFONT, 0, 1);
          sprintf(charbuf, "Code: %d ", packet->system_substate);
          oledWriteString(&oled, 0,0,4,charbuf, INFOFONT, 0, 1);        
        break; //close alarm case
 
        default :
          if( (packet->status_code == Status_Reset)){
            oledFill(&oled, 0,1);
            oledWriteString(&oled, 0,0,0,(char *)" *****************", FONT_6x8, 0, 1);
            oledWriteString(&oled, 0,0,7,(char *)" *****************", FONT_6x8, 0, 1);
            //no jog during hold
            oledWriteString(&oled, 0,0,3,(char *)"RESETTING", JOGFONT, 0, 1);
            oledWriteString(&oled, 0,0,4,(char *)"CONTROLLER", INFOFONT, 0, 1);
          }     
          else if( (packet->status_code == Status_UserException)){
            oledFill(&oled, 0,1);
            oledWriteString(&oled, 0,0,0,(char *)" *****************", FONT_6x8, 0, 1);
            oledWriteString(&oled, 0,0,7,(char *)" *****************", FONT_6x8, 0, 1);
            //no jog during hold
            oledWriteString(&oled, 0,0,4,(char *)"NO CONNECTION", JOGFONT, 0, 1);
            sleep_ms(1000);
            oledFill(&oled, 0,1);
          }
        break; //close default case
      }//close system_state switch statement
  }//close screen mode switch statement
  packet->system_substate = prev_packet.system_substate;
  prev_packet = *packet;
  // previous_jogmode = current_jogmode;
  // previous_jogmodify = current_jogmodify;
  previous_screenmode = screenmode;  
}//close draw main screen

bool tick_timer_callback(struct repeating_timer *t) {
    if (onboard_led_count == 0){
    //gpio_put(ONBOARD_LED, !gpio_get_out_level(ONBOARD_LED));                // toggle the LED
    onboard_led_count = 20;  //thi ssets the heartbeat
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

    if(direction_pressed){
      rollover_delay++;
      if(rollover_delay > ROLLOVER_DELAY_PERIOD)
        rollover_delay = ROLLOVER_DELAY_PERIOD;

      transition_delay = transition_delay - 1;
      if(transition_delay < 0)
        transition_delay = 0;
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
packet->system_state = SystemState_Undefined; // ADD STATUS FOR CONTROLLER DISCONNECTED?
packet->status_code = Status_UserException; // ADD STATUS FOR CONTROLLER DISCONNECTED?
key_character = CMD_STATUS_REPORT_LEGACY;
//keypad_sendchar (key_character, 1, 1);
status_update_counter = STATUS_REQUEST_PERIOD;

if (*flash_target_contents != 0xff)
  screenflip = *flash_target_contents;

uint8_t uc[8];
int i, j;
char szTemp[32];
    
rc = oledInit(&oled, OLED_128x64, 0x3c, screenflip, 0, 0, SDA_PIN, SCL_PIN, RESET_PIN, 1000000L);
oledSetBackBuffer(&oled, ucBuffer);
oledFill(&oled, 0,1);
oledWriteString(&oled, 0,0,1,(char *)"JOG2K", FONT_12x16, 0, 1);
oledWriteString(&oled, 0,0,4,(char *)JOG2K_VERSION, FONT_8x8, 0, 1);
oledWriteString(&oled, 0,0,7,(char *)PLUGIN_VERSION, FONT_6x8, 0, 1);
sleep_ms(1000);
oledFill(&oled, 0,1);

draw_main_screen(1);

    // Main loop handles the buttons, everything else handled in interrupts
    while (true) {

        //draw_main_screen(1);
        
        // if (!packet->machine_state.disconnected){
        //   current_jogmode = (Jogmode) (packet->jog_mode.mode);
        //   current_jogmodify =  (Jogmodify) (packet->jog_mode.modifier);
        // }

        if( packet->system_state != previous_packet->system_state ||
            packet->feed_override != previous_packet->feed_override ||
            packet->spindle_override != previous_packet->spindle_override||
            packet->jog_mode.value != previous_packet->jog_mode.value ||
            packet->coordinate.x != previous_packet->coordinate.x ||
            packet->coordinate.y != previous_packet->coordinate.y ||
            packet->coordinate.z != previous_packet->coordinate.z ||
            (!isnan(packet->coordinate.a) && (packet->coordinate.a != previous_packet->coordinate.a)) ||                  
            packet->current_wcs != previous_packet->current_wcs ||
            packet->jog_stepsize != previous_packet->jog_stepsize ||
            packet->feed_rate != previous_packet->feed_rate ||
            packet->spindle_rpm != previous_packet->spindle_rpm ||
            packet->jog_mode.modifier != previous_packet->jog_mode.modifier ||
            screenmode != previous_screenmode
            ){          
          draw_main_screen(1);        
        }

        //if(screenmode != previous_screenmode)
        //  draw_main_screen(1);
        
        if(packet->system_state == SystemState_Jog){
          draw_main_screen(1);
          update_neopixels();
        }

        if (update_neopixel_leds && (packet->status_code != Status_UserException) ){
          if(!context.mem_address_written) // >= offsetof(machine_status_packet_t, msgtype))          
            update_neopixels();
          update_neopixel_leds = 0;
        }

        //BUTTON READING ***********************************************************************
        if(gpio_get(HALTBUTTON)){
          pixels.setPixelColor(HALTLED,pixels.Color(0, 0, 0));        
          key_character = 0x18;
          //while(gpio_get(HALTBUTTON))
          //  sleep_ms(250);     
        } else if (gpio_get(HOLDBUTTON)){
          pixels.setPixelColor(HOLDLED,pixels.Color(0, 0, 0));
          if(!jog_toggle_pressed){             
          key_character = CMD_FEED_HOLD ;
          keypad_sendchar(key_character, 1, 1);
          while(gpio_get(HOLDBUTTON))
            sleep_us(100);
          } 
          //gpio_put(KPSTR_PIN, false);
                                
        } else if (gpio_get(RUNBUTTON)){
          pixels.setPixelColor(RUNLED,pixels.Color(0, 0, 0));
          if(!jog_toggle_pressed){             
          key_character = CMD_CYCLE_START ;
          keypad_sendchar(key_character, 1, 1);
          while(gpio_get(RUNBUTTON))
            sleep_us(100);
          }
          //gpio_put(KPSTR_PIN, false);    
        //misc commands.  These activate on lift
        } else if (gpio_get(SPINOVER_UP)){  
          if(!jog_toggle_pressed){    
            spin_up_pressed = 1;
          }            
        } else if (gpio_get(SPINOVER_DOWN)){
          if(!jog_toggle_pressed){    
            spin_down_pressed = 1;
          }
        } else if (gpio_get(SPINOVER_RESET)){  
          spin_reset_pressed = 1; 
        } else if (gpio_get(FEEDOVER_UP)){
          if(!jog_toggle_pressed){    
            feed_up_pressed = 1;
          }    
        } else if (gpio_get(FEEDOVER_DOWN)){
          if(!jog_toggle_pressed){     
            feed_down_pressed = 1;
          }            
        } else if (gpio_get(FEEDOVER_RESET)){  
          feed_reset_pressed = 1;              
        } else if (gpio_get(HOMEBUTTON)){  
          home_pressed = 1;        
        } else if (gpio_get(MISTBUTTON)){  
          mist_pressed = 1;
        } else if (gpio_get(FLOODBUTTON)){  
          flood_pressed = 1;   
        } else if (gpio_get(SPINDLEBUTTON)){ 
          spinoff_pressed = 1;
        } else if (gpio_get(JOG_SELECT) && (!joggle_reset)){  //Toggle Jog modes
          jog_toggle_pressed = 1;
        } else if (!jog_toggle_pressed &&//only read jog actions when jog toggle is released.
                   gpio_get(UPBUTTON) ||
                   gpio_get(RIGHTBUTTON) ||
                   gpio_get(DOWNBUTTON) ||
                   gpio_get(LEFTBUTTON) ||
                   gpio_get(RAISEBUTTON) ||
                   gpio_get(LOWERBUTTON) ){
          activate_jogled();
          direction_pressed = 0;           
          direction_pressed = direction_pressed | gpio_get(UPBUTTON) << UP;
          direction_pressed = direction_pressed | gpio_get(RIGHTBUTTON) << RIGHT;
          direction_pressed = direction_pressed | gpio_get(DOWNBUTTON) << DOWN;
          direction_pressed = direction_pressed | gpio_get(LEFTBUTTON) << LEFT;
          direction_pressed = direction_pressed | gpio_get(RAISEBUTTON) << RAISE;
          direction_pressed = direction_pressed | gpio_get(LOWERBUTTON) << LOWER;
        } else {
            if(direction_pressed){
              direction_pressed = 0;
              rollover_delay = 0;
            }
            joggle_reset = false;       
            gpio_put(KPSTR_PIN, false); //make sure stobe is clear when no button is pressed.
          if (status_update_counter < 1){
            status_update_counter = STATUS_REQUEST_PERIOD;
            draw_main_screen(1);
            update_neopixels();
          }
        }        

          //close button reads
//Handle jogging commands ***********************************************************************
        if(direction_pressed){
          
          //draw_main_screen(0);
          if(rollover_delay >= ROLLOVER_DELAY_PERIOD && previous_direction_pressed != direction_pressed){ //wait the elapsed time to handle multi-button press

            if (transition_delay <= 0){
            previous_direction_pressed = direction_pressed;
            gpio_put(KPSTR_PIN, false);
            sleep_ms(5);
            }
            switch (direction_pressed) {
              case JOG_XR :
              key_character = CHAR_XR;
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_XL :
              key_character = CHAR_XL;
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_YF :
              key_character = CHAR_YB; //note inversion is intentional
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_YB :
              key_character = CHAR_YF; //note inversion is intentional
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_ZU :
              key_character = CHAR_ZU;
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_ZD :
              key_character = CHAR_ZD;
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_XRYF :
              key_character = CHAR_XRYF;
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_XRYB :
              key_character = CHAR_XRYB;
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_XLYF :
              key_character = CHAR_XLYF;
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_XLYB :
              key_character = CHAR_XLYB;
              keypad_sendchar (key_character, 0, 1);
              break;
              /*case JOG_XRZU :
              key_character = 'w';
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_XRZD :
              key_character = 'v';
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_XLZU :
              key_character = 'u';
              keypad_sendchar (key_character, 0, 1);
              break;
              case JOG_XLZD :
              key_character = 'x';
              keypad_sendchar (key_character, 0, 1);
              break;*/
              case JOG_AL :
              key_character = CHAR_AL;
              keypad_sendchar (key_character, 0, 1);
              break;   
              case JOG_AR :
              key_character = CHAR_AR;
              keypad_sendchar (key_character, 0, 1);
              break;                                                                                                                                                                                     
              default:
              break;
            }
          }
          //check for button transitions
          if (previous_direction_pressed == direction_pressed){
            transition_delay = packet->feed_rate / 10;
            if(transition_delay < 30)
              transition_delay = 30;

            if(transition_delay > 250)
              transition_delay= 250;
          }
        } else{
          direction_pressed = 0;
          previous_direction_pressed = direction_pressed;
          rollover_delay = 0;
          transition_delay = 0;
        }

//SINGLE BUTTON PRESSES ***********************************************************************
//Alternate functions ***********************************************************************
        if (jog_toggle_pressed){  //Pure modifier button.          
          if (gpio_get(JOG_SELECT)){//keyis still helddown, check alternate keys.
            screenmode = JOG_MODIFY;
            update_neopixels();
            //draw_main_screen(1);
            if (gpio_get(FLOODBUTTON)){
              jog_mod_pressed = 1;
            }
            if (gpio_get(MISTBUTTON)){
              jog_mode_pressed = 1;
            }
            if (gpio_get(LEFTBUTTON)){
              macro_left_pressed = 1;
            } 
            if (gpio_get(RIGHTBUTTON)){
              macro_right_pressed = 1;
            } 
            if (gpio_get(UPBUTTON)){
              macro_top_pressed = 1;
            } 
            if (gpio_get(DOWNBUTTON)){
              macro_bot_pressed = 1;
            } 
            if (gpio_get(RAISEBUTTON)){
              macro_raise_pressed = 1;
            } 
            if (gpio_get(LOWERBUTTON)){
              macro_lower_pressed = 1;
            }
            if (gpio_get(HOMEBUTTON)){
              macro_home_pressed = 1;
            }  
            if (gpio_get(SPINDLEBUTTON)){
              macro_spindle_pressed = 1;
            }  
            if (gpio_get(HOLDBUTTON)){
              reset_pressed = 1;
            }  
            if (gpio_get(RUNBUTTON)){
              unlock_pressed = 1;
            }
            if (gpio_get(HALTBUTTON)){
              halt_pressed = 1;              
            }
            if (gpio_get(SPINOVER_UP)){  
              spin_up_fine_pressed = 1;            
            }
            if (gpio_get(SPINOVER_DOWN)){  
              spin_down_fine_pressed = 1;
            }
            if (gpio_get(FEEDOVER_UP)){ 
              feed_up_fine_pressed = 1;    
            }
            if (gpio_get(FEEDOVER_DOWN)){ 
              feed_down_fine_pressed = 1;            
            }                                                                                                                    
          }//close jog toggle pressed.
        }//close jog button pressed statement
//Single functions ***********************************************************************
        if (jog_toggle_pressed) {
          if (gpio_get(JOG_SELECT)){}//button is still pressed, do nothing
          else{
            jog_toggle_pressed = 0;
            screenmode = DEFAULT;
            update_neopixels();                         
          }
        }
        if (feed_down_pressed) {
          if (gpio_get(FEEDOVER_DOWN)){}//button is still pressed, do nothing
          else{
            key_character = CMD_OVERRIDE_FEED_COARSE_MINUS;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            feed_down_pressed = 0;
            update_neopixels();                       
          }
        }
        if (feed_up_pressed) {
          if (gpio_get(FEEDOVER_UP)){}//button is still pressed, do nothing
          else{
            key_character = CMD_OVERRIDE_FEED_COARSE_PLUS;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            feed_up_pressed = 0;
            update_neopixels();                
          }
        }
        if (feed_reset_pressed) {
          if (gpio_get(FEEDOVER_RESET)){}//button is still pressed, do nothing
          else{
            key_character = CMD_OVERRIDE_FEED_RESET;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            feed_reset_pressed = 0;
            update_neopixels();                
          }
        }
        if (spin_down_pressed) {
          if (gpio_get(SPINOVER_DOWN)){}//button is still pressed, do nothing
          else{
            key_character = CMD_OVERRIDE_SPINDLE_COARSE_MINUS;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            spin_down_pressed = 0;
            update_neopixels();           
          }
        }
        if (spin_up_pressed) {
          if (gpio_get(SPINOVER_UP)){}//button is still pressed, do nothing
          else{
            key_character = CMD_OVERRIDE_SPINDLE_COARSE_PLUS;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            spin_up_pressed = 0;
            update_neopixels();         
          }
        }
        if (spin_reset_pressed) {
          if (gpio_get(SPINOVER_RESET)){}//button is still pressed, do nothing
          else{
            key_character = CMD_OVERRIDE_SPINDLE_RESET;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            spin_reset_pressed = 0;
            update_neopixels();          
          }
        }
        if (mist_pressed) {
          if (gpio_get(MISTBUTTON)){}//button is still pressed, do nothing
          else{
            if(!jog_toggle_pressed){
            key_character = CMD_OVERRIDE_COOLANT_MIST_TOGGLE;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            }
            mist_pressed = 0;
            update_neopixels();           
          }
        }
        if (flood_pressed) {
          if (gpio_get(FLOODBUTTON)){}//button is still pressed, do nothing
          else{
            if(!jog_toggle_pressed){
            key_character = CMD_OVERRIDE_COOLANT_FLOOD_TOGGLE;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            }
            flood_pressed = 0;
            update_neopixels();           
          }                    
        }
        if (spinoff_pressed) {
          if (gpio_get(SPINDLEBUTTON)){}//button is still pressed, do nothing
          else{
            if(!jog_toggle_pressed){
            key_character = CMD_OVERRIDE_SPINDLE_STOP;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            }
            spinoff_pressed = 0;
            update_neopixels();           
          }                    
        } 
        if (home_pressed) {
          if (gpio_get(HOMEBUTTON)){}//button is still pressed, do nothing
          else{
            if(!jog_toggle_pressed){
            key_character = 'H';         
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
          }
            home_pressed = 0;         
            update_neopixels();          
          }                    
        }
        if (jog_mod_pressed) {
          if (gpio_get(FLOODBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = JOGMODIFY_CYCLE;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            jog_mod_pressed = 0;
            sleep_ms(10);
            update_neopixels();                       
          }
        }
        if (jog_mode_pressed) {
          if (gpio_get(MISTBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = JOGMODE_CYCLE;     
            keypad_sendchar (key_character, 1, 1);
            //gpio_put(KPSTR_PIN, false);
            gpio_put(ONBOARD_LED,1);
            jog_mode_pressed = 0;
            sleep_ms(10);
            update_neopixels();                   
          }
        }
        if (macro_left_pressed){
          if (gpio_get(LEFTBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = MACROLEFT;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            macro_left_pressed = 0;
            sleep_ms(10);
            update_neopixels();
        }}
        if (macro_right_pressed){
          if (gpio_get(RIGHTBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = MACRORIGHT;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            macro_right_pressed = 0;
            sleep_ms(10);
            update_neopixels();
        }}
        if (macro_top_pressed){
          if (gpio_get(UPBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = MACROUP;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            macro_top_pressed = 0;
            sleep_ms(10);
            update_neopixels();
        }}
        if (macro_bot_pressed){
          if (gpio_get(DOWNBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = MACRODOWN;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            macro_bot_pressed = 0;
            sleep_ms(10);
            update_neopixels();
        }}
        if (macro_lower_pressed){
          if (gpio_get(LOWERBUTTON)){
            if(!isnan(packet->coordinate.a)){
              //switch screen to jogmode
              screenmode = JOGGING;
              //send jog character
              direction_pressed = JOG_AL;
              //update_neopixels();
            // } else {
            //   key_character = MACROLOWER;
            //   keypad_sendchar (key_character, 1, 1);
            //   update_neopixels();
            }
          }//button is still pressed, Jog A Axis//button is still pressed, Jog A axis
          else{
              if(!isnan(packet->coordinate.a)){          
                //gpio_put(KPSTR_PIN, false);
                jog_toggle_pressed = 0;
                joggle_reset = true;
              }
              else{
                key_character = MACROLOWER;
                keypad_sendchar (key_character, 1, 1);
              }
              gpio_put(ONBOARD_LED,1);
              macro_lower_pressed = 0;
              sleep_ms(10);
              update_neopixels();
        }}  
        if (macro_raise_pressed){
          if (gpio_get(RAISEBUTTON)){
            if(!isnan(packet->coordinate.a)){
              //switch screen to jogmode
              screenmode = JOGGING;
              //send jog character
              direction_pressed = JOG_AR;
              //update_neopixels();
            // } else {
            //   key_character = MACRORAISE;
            //   keypad_sendchar (key_character, 1, 1);
            //   update_neopixels();
            }
          }//button is still pressed, Jog A Axis//button is still pressed, Jog A axis
          else{
              if(!isnan(packet->coordinate.a)){        
                //gpio_put(KPSTR_PIN, false);
                jog_toggle_pressed = 0;
                joggle_reset = true;
              }
              else{
                key_character = MACRORAISE;
                keypad_sendchar (key_character, 1, 1);
              }
              gpio_put(ONBOARD_LED,1);
              macro_raise_pressed = 0;
              sleep_ms(10);
              update_neopixels();              
        }}        
        if (macro_spindle_pressed){
          if (gpio_get(SPINDLEBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = MACROSPINDLE;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            macro_spindle_pressed = 0;
            sleep_ms(10);
            update_neopixels();
        }}
        if (macro_home_pressed){
          if (gpio_get(HOMEBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = MACROHOME;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            macro_home_pressed = 0;
            sleep_ms(10);
            update_neopixels();
        }}
        if (reset_pressed){
          if (gpio_get(HOLDBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = RESET;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            reset_pressed = 0;
            sleep_ms(10);
            packet->system_state = SystemState_Undefined;
            packet->status_code = Status_Reset;
            draw_main_screen(1);
            sleep_ms(500);            
            update_neopixels();
        }}
        if (unlock_pressed){
          if (gpio_get(RUNBUTTON)){}//button is still pressed, do nothing
          else{
            key_character = UNLOCK;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            unlock_pressed = 0;
            sleep_ms(10);
            update_neopixels();
        }}
        if (feed_down_fine_pressed) {
          if (gpio_get(FEEDOVER_DOWN)){}//button is still pressed, do nothing
          else{
            key_character = CMD_OVERRIDE_FEED_FINE_MINUS;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            feed_down_fine_pressed = 0;
            sleep_ms(10);
            update_neopixels();                       
         }}
        if (feed_up_fine_pressed) {
          if (gpio_get(FEEDOVER_UP)){}//button is still pressed, do nothing
          else{
            key_character = CMD_OVERRIDE_FEED_FINE_PLUS;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            feed_up_fine_pressed = 0;
            sleep_ms(10);
            update_neopixels();                
        }}
        if (spin_down_fine_pressed) {
          if (gpio_get(SPINOVER_DOWN)){}//button is still pressed, do nothing
          else{
            key_character = CMD_OVERRIDE_SPINDLE_FINE_MINUS;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            spin_down_fine_pressed = 0;
            sleep_ms(10);
            update_neopixels();           
        }}
        if (spin_up_fine_pressed) {
          if (gpio_get(SPINOVER_UP)){}//button is still pressed, do nothing
          else{
            key_character = CMD_OVERRIDE_SPINDLE_FINE_PLUS;
            keypad_sendchar (key_character, 1, 1);
            gpio_put(ONBOARD_LED,1);
            spin_up_fine_pressed = 0;
            sleep_ms(10);
            update_neopixels();
        }}  
        if (halt_pressed){
          if (gpio_get(HALTBUTTON)){
            pixels.setPixelColor(HALTLED,pixels.Color(0,255,0));
            pixels.show();
          }//button is still pressed, do nothing
          else{
            //erase and write memory location based on current value of screenflip.
            
            pixels.setPixelColor(HALTLED,pixels.Color(255,255,0));
            pixels.show();
            sleep_ms(250);
            screenflip = !screenflip;
            uint32_t status = save_and_disable_interrupts();
            flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
            
            restore_interrupts(status);
            sleep_ms(250);
            flash_range_program(FLASH_TARGET_OFFSET, (uint8_t*)&screenflip, 1);
            halt_pressed = 0;
            //update_neopixels();

            #define AIRCR_Register (*((volatile uint32_t*)(PPB_BASE + 0x0ED0C)))

            AIRCR_Register = 0x5FA0004;
        }}                                                                                                                                                  
    }//close main while loop
    return 0;
}
