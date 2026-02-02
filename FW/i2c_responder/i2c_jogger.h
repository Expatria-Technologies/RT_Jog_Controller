#ifndef __I2C_JOGGER_H__
#define __I2C_JOGGER_H__

#define PLUGIN_VERSION "PLUGIN: Keypad v1.44"
#define JOG2K_FW_VERSION "1.2.0"

#ifndef BUILD_SHA
#define BUILD_SHA "dev"
#endif

#define JOG2K_VERSION "v" JOG2K_FW_VERSION "+" BUILD_SHA

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

// #define STATE_ALARM         1 //!< In alarm state. Locks out all g-code processes. Allows settings access.
// #define STATE_CYCLE         2 //!< Cycle is running or motions are being executed.
// #define STATE_HOLD          3 //!< Active feed hold
// #define STATE_TOOL_CHANGE   4 //!< Manual tool change, similar to #STATE_HOLD - but stops spindle and allows jogging.
// #define STATE_IDLE          5 //!< Must be zero. No flags.
// #define STATE_HOMING        6 //!< Performing homing cycle
// #define STATE_JOG           7 //!< Jogging mode.
// #define STATE_RESET         8 //!< Screen set when reset is issued.
                 
// #define NORMAL_MODE         0
// #define LASER_MODE          1
// #define LATHE_MODE          2

//#define STATE_DISCONNECTED  1

// typedef union {
//     uint8_t value;                 //!< Bitmask value
//     struct {
//         uint8_t 
//         state        :4, //Machine state machine status
//         mode         :3, //machine mode
//         disconnected :1; //Connection status 
//     };
// } machine_state_t;

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
#define MACROUP         0xB0 //MACRO_KEY0
#define MACRODOWN       0xB1 //MACRO_KEY1
#define MACROLEFT       0xB2 //MACRO_KEY2
#define MACRORIGHT      0xB3 //MACRO_KEY3
#define MACROSPINDLE    0xB4 //MACRO_KEY4
#define MACRORAISE      0xB5 //MACRO_KEY5
#define MACROLOWER      0xB6 //MACRO_KEY6

#define MACROHOME   'o'  //toggle WCS offset
#define RESET  0x18
#define UNLOCK 'X'

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define RAISE 4
#define LOWER 5

//keycode mappings:
#define JOG_XR   0b00000010
#define JOG_XL   0b00001000
#define JOG_YF   0b00000100
#define JOG_YB   0b00000001
#define JOG_ZU   0b00010000
#define JOG_ZD   0b00100000
#define JOG_XRYF JOG_XR | JOG_YB
#define JOG_XRYB JOG_XR | JOG_YF
#define JOG_XLYF JOG_XL | JOG_YB
#define JOG_XLYB JOG_XL | JOG_YF
#define JOG_AR   0b10000000
#define JOG_AL   0b01000000
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

enum Jogmodify {
    JogModify_1 = 0,
    JogModify_01  = 1,
    JogModify_001 = 2
};

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

// // Define spindle stop override control states.
// typedef union {
//     uint8_t value;
//     struct {
//         uint8_t enabled       :1,
//                 initiate      :1,
//                 restore       :1,
//                 restore_cycle :1,
//                 unassigned    :4;
//     };
// } spindle_stop_t;

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

enum msg_type_t {
    MachineMsg_None = 0,
// 1-127 reserved for message string length
    MachineMsg_Overrides = 253,
    MachineMsg_WorkOffset = 254,
    MachineMsg_ClearMessage = 255,
};

enum machine_state_t {
    MachineState_Alarm = 1,
    MachineState_Cycle = 2,
    MachineState_Hold = 3,
    MachineState_ToolChange = 4,
    MachineState_Idle = 5,
    MachineState_Homing = 6,
    MachineState_Jog = 7,
    // MachineState_Reset = 8,
    MachineState_Other = 254
};

typedef union {
    uint8_t mask;
    uint8_t bits;
    uint8_t value;
    struct {
        uint8_t x :1,
                y :1,
                z :1,
                a :1,
                b :1,
                c :1,
                u :1,
                v :1;
    };
} axes_signals_t;

typedef union {
    uint8_t value;
    uint8_t mask;
    struct {
        uint8_t on               :1,
                ccw              :1,
                pwm              :1, //!< NOTE: only used for PWM inversion setting
                reserved         :1,
                override_disable :1,
                encoder_error    :1,
                at_speed         :1, //!< Spindle is at speed.
                synchronized     :1;
    };
} spindle_state_t;

typedef union {
    uint16_t bits;
    uint16_t mask;
    uint16_t value;
    struct {
        uint16_t reset                :1,
                 feed_hold            :1,
                 cycle_start          :1,
                 safety_door_ajar     :1,
                 block_delete         :1,
                 stop_disable         :1, //! M1
                 e_stop               :1,
                 probe_disconnected   :1,
                 motor_fault          :1,
                 motor_warning        :1,
                 limits_override      :1,
                 single_block         :1,
                 tls_overtravel       :1, //! used for probe (toolsetter) protection
                 probe_overtravel     :1, //! used for probe protection
                 probe_triggered      :1, //! used for probe protection
                 deasserted           :1; //! this flag is set if signals are deasserted.
    };
} control_signals_t;

typedef union {
    uint8_t value;
    struct {
        uint8_t modifier :4,
                mode     :4;
    };
} jog_mode_t;

typedef union {
    uint8_t value;
    struct {
        uint8_t diameter       :1,
                mpg            :1,
                homed          :1,
                tlo_referenced :1,
                imperial       :1,
                mode           :3; // from machine_mode_t setting
    };
} machine_modes_t;

typedef enum  {
    Mode_Standard = 0,      //!< 0
    Mode_Laser,             //!< 1
    Mode_Lathe              //!< 2
} machine_mode_t;

typedef union {
    float values[4];
    struct {
        float x;
        float y;
        float z;
        float a;
    };
} machine_coords_t;

// Define grblHAL status codes. Valid values (0-255)
typedef enum {
    Status_OK = 0,
    Status_ExpectedCommandLetter = 1,
    Status_BadNumberFormat = 2,
    Status_InvalidStatement = 3,
    Status_NegativeValue = 4,
    Status_HomingDisabled = 5,
    Status_SettingStepPulseMin = 6,
    Status_SettingReadFail = 7,
    Status_IdleError = 8,
    Status_SystemGClock = 9,
    Status_SoftLimitError = 10,
    Status_Overflow = 11,
    Status_MaxStepRateExceeded = 12,
    Status_CheckDoor = 13,
    Status_LineLengthExceeded = 14,
    Status_TravelExceeded = 15,
    Status_InvalidJogCommand = 16,
    Status_SettingDisabledLaser = 17,
    Status_Reset = 18,
    Status_NonPositiveValue = 19,

    Status_GcodeUnsupportedCommand = 20,
    Status_GcodeModalGroupViolation = 21,
    Status_GcodeUndefinedFeedRate = 22,
    Status_GcodeCommandValueNotInteger = 23,
    Status_GcodeAxisCommandConflict = 24,
    Status_GcodeWordRepeated = 25,
    Status_GcodeNoAxisWords = 26,
    Status_GcodeInvalidLineNumber = 27,
    Status_GcodeValueWordMissing = 28,
    Status_GcodeUnsupportedCoordSys = 29,
    Status_GcodeG53InvalidMotionMode = 30,
    Status_GcodeAxisWordsExist = 31,
    Status_GcodeNoAxisWordsInPlane = 32,
    Status_GcodeInvalidTarget = 33,
    Status_GcodeArcRadiusError = 34,
    Status_GcodeNoOffsetsInPlane = 35,
    Status_GcodeUnusedWords = 36,
    Status_GcodeG43DynamicAxisError = 37,
    Status_GcodeIllegalToolTableEntry = 38,
    Status_GcodeValueOutOfRange = 39,
    Status_GcodeToolChangePending = 40,
    Status_GcodeSpindleNotRunning = 41,
    Status_GcodeIllegalPlane = 42,
    Status_GcodeMaxFeedRateExceeded = 43,
    Status_GcodeRPMOutOfRange = 44,
    Status_LimitsEngaged = 45,
    Status_HomingRequired = 46,
    Status_GCodeToolError = 47,
    Status_ValueWordConflict = 48,
    Status_SelfTestFailed = 49,
    Status_EStop = 50,
    Status_MotorFault = 51,
    Status_SettingValueOutOfRange = 52,
    Status_SettingDisabled = 53,
    Status_GcodeInvalidRetractPosition = 54,
    Status_IllegalHomingConfiguration = 55,
    Status_GCodeCoordSystemLocked = 56,
    Status_UnexpectedDemarcation = 57,

    Status_SDMountError = 60,
    Status_FileReadError = 61,
    Status_FsFailedOpenDir = 62,
    Status_FSDirNotFound = 63,
    Status_SDNotMounted = 64,
    Status_FsNotMounted = 65,
    Status_FsReadOnly = 66,

    Status_BTInitError = 70,

//
    Status_ExpressionUknownOp = 71,
    Status_ExpressionDivideByZero = 72,
    Status_ExpressionArgumentOutOfRange = 73,
    Status_ExpressionInvalidArgument = 74,
    Status_ExpressionSyntaxError = 75,
    Status_ExpressionInvalidResult = 76,

    Status_AuthenticationRequired = 77,
    Status_AccessDenied = 78,
    Status_NotAllowedCriticalEvent = 79,

    Status_FlowControlNotExecutingMacro = 80,
    Status_FlowControlSyntaxError = 81,
    Status_FlowControlStackOverflow = 82,
    Status_FlowControlOutOfMemory = 83,
    Status_FileOpenFailed = 84,
    Status_StatusMax = Status_FlowControlOutOfMemory,
    Status_UserException = 253,
    Status_Handled,   // For internal use only
    Status_Unhandled  // For internal use only
} __attribute__ ((__packed__)) status_code_t;

typedef struct {
    uint8_t address;
    machine_state_t machine_state;
    uint8_t machine_substate;
    axes_signals_t home_state;
    uint8_t feed_override; // size changed in latest version!
    uint8_t spindle_override;
    uint8_t spindle_stop;
    spindle_state_t spindle_state;
    int spindle_rpm;
    float feed_rate;
    coolant_state_t coolant_state;
    jog_mode_t jog_mode;
    control_signals_t signals;
    float jog_stepsize;
    coord_system_id_t current_wcs;  //active WCS or MCS modal state
    axes_signals_t limits;
    status_code_t status_code;
    machine_modes_t machine_modes;
    machine_coords_t coordinate;
    msg_type_t msgtype; //<! 1 - 127 -> msg[] contains a string msgtype long
    uint8_t msg[128];
} machine_status_packet_t;

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

