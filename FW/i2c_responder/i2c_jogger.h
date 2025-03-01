#ifndef __I2C_JOGGER_H__
#define __I2C_JOGGER_H__

#define PLUGIN_VERSION "PLUGIN: KEYPAD v1.4"
#define JOG2K_VERSION  "FW: v1.0.6"

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        22 // On Trinket or Gemma, suggest changing this to 1
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 10 // Popular NeoPixel ring size
#define DELAYVAL 200 // Time (in milliseconds) to pause between pixels
#define CYCLEDELAY 12 // Time to wait after a cycle
#define NEO_BRIGHTNESS 128

//flash defines
#define FLASH_TARGET_OFFSET (256 * 1024)
const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

#define SDA_PIN 2
#define SCL_PIN 3
#define RESET_PIN -1

#define STATE_ALARM         1 //!< In alarm state. Locks out all g-code processes. Allows settings access.
#define STATE_CYCLE         2 //!< Cycle is running or motions are being executed.
#define STATE_HOLD          3 //!< Active feed hold
#define STATE_TOOL_CHANGE   4 //!< Manual tool change, similar to #STATE_HOLD - but stops spindle and allows jogging.
#define STATE_IDLE          5 //!< Must be zero. No flags.
#define STATE_HOMING        6 //!< Performing homing cycle
#define STATE_JOG           7 //!< Jogging mode.
#define STATE_RESET         8 //!< Screen set when reset is issued.
                 
#define NORMAL_MODE         0
#define LASER_MODE          1
#define LATHE_MODE          2

#define STATE_DISCONNECTED  1

typedef union {
    uint8_t value;                 //!< Bitmask value
    struct {
        uint8_t 
        state        :4, //Machine state machine status
        mode         :3, //machine mode
        disconnected :1; //Connection status 
    };
} machine_state_t;

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

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

//ACTION DEFINES
#define MACROUP 0x18  //MACRO_KEY0
#define MACRODOWN 0x19 //MACRO_KEY1
#define MACROLEFT 0x1B //MACRO_KEY2
#define MACRORIGHT 0x1A //MACRO_KEY3
#define MACROLOWER  0x7D //MACRO_KEY4
#define MACRORAISE 0x7C //MACRO_KEY5
#define MACROHOME  0x8E //MACRO_KEY6
#define RESET  0x7F
#define UNLOCK 0x80
#define SPINON 0x83

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define RAISE 4
#define LOWER 5

//keycode mappings:
#define JOG_XR   0b000010
#define JOG_XL   0b001000
#define JOG_YF   0b000100
#define JOG_YB   0b000001
#define JOG_ZU   0b010000
#define JOG_ZD   0b100000
#define JOG_XRYF JOG_XR | JOG_YB
#define JOG_XRYB JOG_XR | JOG_YF
#define JOG_XLYF JOG_XL | JOG_YB
#define JOG_XLYB JOG_XL | JOG_YF
/*#define JOG_XRZU JOG_XR | JOG_ZU
#define JOG_XRZD JOG_XR | JOG_ZD
#define JOG_XLZU JOG_XL | JOG_ZU
#define JOG_XLZD JOG_XL | JOG_ZD*/

#define CHAR_XR   'R'
#define CHAR_XL   'L'
#define CHAR_YF   'F'
#define CHAR_YB   'B'
#define CHAR_ZU   'U'
#define CHAR_ZD   'D'
#define CHAR_XRYF 'r'
#define CHAR_XRYB 'q'
#define CHAR_XLYF 's'
#define CHAR_XLYB 't'
#define CHAR_XRZU 'w'
#define CHAR_XRZD 'v'
#define CHAR_XLZU 'u'
#define CHAR_XLZD 'x'
#define CHAR_AR   'A'
#define CHAR_AL   'a'

#define CMD_STATUS_REPORT_LEGACY '?'
#define CMD_CYCLE_START 0x81   // TODO: use 0x06 ctrl-F ACK instead? or SYN/DC2/DC3?
#define CMD_FEED_HOLD 0x82     // TODO: use 0x15 ctrl-U NAK instead?
#define CMD_RESET 0x18 // ctrl-X (CAN)
#define CMD_SAFETY_DOOR 0x84
#define CMD_OVERRIDE_FAN0_TOGGLE 0x8A       // Toggle Fan 0 on/off, not implemented by the core.
#define CMD_MPG_MODE_TOGGLE 0x8B            // Toggle MPG mode on/off, not implemented by the core.
#define CMD_AUTO_REPORTING_TOGGLE 0x8C      // Toggle auto real time reporting if configured.
#define CMD_OVERRIDE_FEED_RESET 0x90        // Restores feed override value to 100%.
#define CMD_OVERRIDE_FEED_COARSE_PLUS 0x91
#define CMD_OVERRIDE_FEED_COARSE_MINUS 0x92
#define CMD_OVERRIDE_FEED_FINE_PLUS 0x93
#define CMD_OVERRIDE_FEED_FINE_MINUS 0x94
#define CMD_OVERRIDE_RAPID_RESET 0x95       // Restores rapid override value to 100%.
#define CMD_OVERRIDE_RAPID_MEDIUM 0x96
#define CMD_OVERRIDE_RAPID_LOW 0x97
#define CMD_OVERRIDE_SPINDLE_RESET 0x99     // Restores spindle override value to 100%.
#define CMD_OVERRIDE_SPINDLE_COARSE_PLUS 0x9A
#define CMD_OVERRIDE_SPINDLE_COARSE_MINUS 0x9B
#define CMD_OVERRIDE_SPINDLE_FINE_PLUS 0x9C
#define CMD_OVERRIDE_SPINDLE_FINE_MINUS 0x9D
#define CMD_OVERRIDE_SPINDLE_STOP 0x9E
#define CMD_OVERRIDE_COOLANT_FLOOD_TOGGLE 'C'
#define CMD_OVERRIDE_COOLANT_MIST_TOGGLE 'M'
#define CMD_PID_REPORT 0xA2
#define CMD_TOOL_ACK 0xA3
#define CMD_PROBE_CONNECTED_TOGGLE 0xA4

#define JOGMODE_FAST '0'
#define JOGMODE_SLOW '1'
#define JOGMODE_STEP '2'
#define JOGMODE_CYCLE 'h'
#define JOGMODIFY_CYCLE 'm'

// The slave implements a 256 byte memory. To write a series of bytes, the master first
// writes the memory address, followed by the data. The address is automatically incremented
// for each byte transferred, looping back to 0 upon reaching the end. Reading is done
// sequentially from the current memory address.
#define I2C_TIMEOUT_VALUE 100000  //microseconds

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

enum Jogmode {FAST = 0,
              SLOW = 1,
              STEP = 2};

enum Jogmode current_jogmode;

enum Jogmodify {
    JogModify_1 = 0,
    JogModify_01  = 1,
    JogModify_001 = 2
};
enum Jogmodify current_jogmodify;

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

typedef enum {
    CoordinateSystem_G54 = 0,                       //!< 0 - G54 (G12)
    CoordinateSystem_G55,                           //!< 1 - G55 (G12)
    CoordinateSystem_G56,                           //!< 2 - G56 (G12)
    CoordinateSystem_G57,                           //!< 3 - G57 (G12)
    CoordinateSystem_G58,                           //!< 4 - G58 (G12)
    CoordinateSystem_G59,                           //!< 5 - G59 (G12)
#if COMPATIBILITY_LEVEL <= 1
    CoordinateSystem_G59_1,                         //!< 6 - G59.1 (G12) - availability depending on #COMPATIBILITY_LEVEL <= 1
    CoordinateSystem_G59_2,                         //!< 7 - G59.2 (G12) - availability depending on #COMPATIBILITY_LEVEL <= 1
    CoordinateSystem_G59_3,                         //!< 8 - G59.3 (G12) - availability depending on #COMPATIBILITY_LEVEL <= 1
#endif
    N_WorkCoordinateSystems,                        //!< 9 when #COMPATIBILITY_LEVEL <= 1, 6 otherwise
    CoordinateSystem_G28 = N_WorkCoordinateSystems, //!< 9 - G28 (G0) when #COMPATIBILITY_LEVEL <= 1, 6 otherwise
    CoordinateSystem_G30,                           //!< 10 - G30 (G0) when #COMPATIBILITY_LEVEL <= 1, 7 otherwise
    CoordinateSystem_G92,                           //!< 11 - G92 (G0) when #COMPATIBILITY_LEVEL <= 1, 8 otherwise
    N_CoordinateSystems                             //!< 12 when #COMPATIBILITY_LEVEL <= 1, 9 otherwise
} coord_system_id_t;

typedef struct Machine_status_packet {
uint8_t address;
machine_state_t machine_state;
uint8_t alarm;
uint8_t home_state;
uint8_t feed_override;
uint8_t spindle_override;
uint8_t spindle_stop;
int spindle_rpm;
float feed_rate;
coolant_state_t coolant_state;
uint8_t jog_mode;  //includes both modifier as well as mode
float jog_stepsize;
coord_system_id_t current_wcs;  //active WCS or MCS modal state
float x_coordinate;
float y_coordinate;
float z_coordinate;
float a_coordinate;
} Machine_status_packet;

enum ScreenMode{
    DEFAULT = 0,
    JOGGING,
    RUN,
    JOG_MODIFY
};
enum ScreenMode screenmode;

#if defined(_LINUX_) && defined(__cplusplus)
}
#endif // _LINUX_

#endif // 

