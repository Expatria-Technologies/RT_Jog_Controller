//
// obd.inl
// Drawing code for OneBitDisplay library
//
#ifdef _LINUX_
#define PROGMEM
#define false 0
#define true 1
#define LOW 0
#define HIGH 1
#define memcpy_P memcpy
#ifndef I2C_SLAVE
#define I2C_SLAVE 0
#define INPUT GPIO_IN
#define OUTPUT GPIO_OUT
#define INPUT_PULLUP GPIO_IN_PULLUP
#define HIGH 1
#define LOW 0
#endif
static uint8_t pgm_read_byte(const uint8_t *ptr);
static void digitalWrite(int iPin, int iState) {
#ifndef MEMORY_ONLY
   AIOWriteGPIO(iPin, iState);
#endif
}
static void pinMode(int iPin, int iMode)
{
#ifndef MEMORY_ONLY
   AIOAddGPIO(iPin, iMode);
#endif
} /* pinMode() */
static int digitalRead(int iPin)
{
  return AIOReadGPIO(iPin);
} /* digitalRead() */

#endif // _LINUX_
#ifndef _LINUX_
#ifdef ARDUINO_ARCH_RP2040
MbedSPI *mySPI;
#else
SPIClass *mySPI = &SPI;
#endif
#endif // !_LINUX_
void _delay(int iDelay);
static void EPDWaitBusy(OBDISP *pOBD);
static void EPDSleep(OBDISP *pOBD);
static void EPDWriteImage(OBDISP *pOBD, uint8_t ucCMD, int x, int y, int w, int h, int bRed);
void EPD213_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial);
void EPD293_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial);
void EPD154_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial);
void EPD29R_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial);
void EPD213_CMD(OBDISP *pOBD, uint8_t cmd, uint8_t param);
void obdSetDCMode(OBDISP *pOBD, int iMode);
void InvertBytes(uint8_t *pData, uint8_t bLen);
void SPI_BitBang(OBDISP *pOBD, uint8_t *pData, int iLen, uint8_t iMOSIPin, uint8_t iSCKPin);
static void RawWrite(OBDISP *pOBD, unsigned char *pData, int iLen);
static void RawWriteData(OBDISP *pOBD, unsigned char *pData, int iLen);
// EPD look up tables
// new waveform created by Jean-Marc Zingg for the actual panel
#define T1 25 // color change charge balance pre-phase
#define T2  1 // color change or sustain charge balance pre-phase
#define T3  2 // color change or sustain phase
#define T4 25 // color change phase

const unsigned char lut_20_vcom0_partial[] PROGMEM =
{
  0x00, T1, T2, T3, T4, 1, // 00 00 00 00
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char lut_21_ww_partial[] PROGMEM =
{ // 10 w
  0x18, T1, T2, T3, T4, 1, // 00 01 10 00
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char lut_22_bw_partial[] PROGMEM =
{ // 10 w
  0x5A, T1, T2, T3, T4, 1, // 01 01 10 10
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char lut_23_wb_partial[] PROGMEM =
{ // 01 b
  0xA5, T1, T2, T3, T4, 1, // 10 10 01 01
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char lut_24_bb_partial[] PROGMEM =
{ // 01 b
  0x24, T1, T2, T3, T4, 1, // 00 10 01 00
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

// 2.7" 176x264 LUTs
const uint8_t lut_fast_vcom_dc[] = {
    0x00, 0x00,
    0x00, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x00, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x00, 0x0E, 0x01, 0x0E, 0x01, 0x01,
    0x00, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};
const uint8_t lut_fast_ww[] = {
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x40, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x01,
    0x80, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};
const uint8_t lut_fast_bw[] = {
    0xA0, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x00, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x01,
    0x90, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0xB0, 0x04, 0x10, 0x00, 0x00, 0x05,
    0xB0, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0xC0, 0x23, 0x00, 0x00, 0x00, 0x01
};
const uint8_t lut_fast_bb[] = {
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x40, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x01,
    0x80, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};
const uint8_t lut_fast_wb[] = {
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01,
    0x20, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x84, 0x0E, 0x01, 0x0E, 0x01, 0x01,
    0x10, 0x0A, 0x0A, 0x00, 0x00, 0x01,
    0x00, 0x04, 0x10, 0x00, 0x00, 0x05,
    0x00, 0x03, 0x0E, 0x00, 0x00, 0x0A,
    0x00, 0x23, 0x00, 0x00, 0x00, 0x01
};

const uint8_t lut_vcom_dc[] =
{
    0x00, 0x00,
    0x00, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x00, 0x32, 0x32, 0x00, 0x00, 0x02,
    0x00, 0x0F, 0x0F, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const uint8_t lut_ww[] =
{
    0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
     0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const uint8_t lut_bw[] =
{
    0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
     0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};
const uint8_t lut_bb[] =
{
    0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
      0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
      0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const uint8_t lut_wb[] =
{
    0xA0, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x60, 0x32, 0x32, 0x00, 0x00, 0x02,
     0x50, 0x0F, 0x0F, 0x00, 0x00, 0x05,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_vcom0_full[] =
{
  0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
  0x00, 0x17, 0x17, 0x00, 0x00, 0x02,
  0x00, 0x0A, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x0E, 0x0E, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_ww_full[] =
{
  0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
  0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
  0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_bw_full[] =
{
  0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
  0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
  0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char lut_wb_full[] =
{
  0x80, 0x17, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
  0x80, 0x0A, 0x01, 0x00, 0x00, 0x01,
  0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_bb_full[] =
{
  0x80, 0x17, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x17, 0x17, 0x00, 0x00, 0x02,
  0x80, 0x0A, 0x01, 0x00, 0x00, 0x01,
  0x50, 0x0E, 0x0E, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char epd102_lut_w_full[] PROGMEM =
{
  0x60, 0x5A, 0x5A, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char epd102_lut_b_full[] PROGMEM =
{
  0x90, 0x5A, 0x5A, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
// init sequence for GDEW042Z15
const uint8_t epd42r2_init_sequence_full[] = {
    0x04, UC8151_BTST, 0x17,0x17,0x17, // booster soft-start config - START_10MS | STRENGTH_3 | OFF_6_58US
    0x01, UC8151_PON, // power on
    BUSY_WAIT,
    0x02, 0x00, 0x0f, // LUT from OTP
    0x02, UC8151_CDI, 0x9c, // inverted, white border
    0x00
};

// initialization sequence for 2.9" 296x128 e-paper
const uint8_t epd29_init_sequence_full[] = {
    0x02, UC8151_PSR, 0x80 | 0x00 | 0x10 | 0x00 | 0x02 | 0x01, // RES_128x296, LUT_OTP, FORMAT_BW, SHIFT_LEFT, BOOSTER_ON, RESET_NONE
    0x06, UC8151_PWR, 0x03, 0x00, 0x2b, 0x2b, 0x2b,
    0x01, UC8151_PON, // power on
    BUSY_WAIT,
    0x04, UC8151_BTST, 0x17,0x17,0x17, // booster soft-start config - START_10MS | STRENGTH_3 | OFF_6_58US
    0x02, UC8151_PFS, 0x00, // FRAMES_1
    0x02, UC8151_TSE, 0x00, // TEMP_INTERNAL | OFFSET_0
    0x02, UC8151_TCON, 0x22,
    0x02, UC8151_CDI, 0x9c, // inverted, white border
    0x02, UC8151_PLL, 0x3a, // HZ_100
    0x00 // end of table
};

// initialization sequence for 1.02" 80x128 e-paper
const uint8_t epd102_init_sequence_full[] = {
//    0x02, 0xd2, 0x3f, // ?
    0x02, 0x00, 0x5f,
//    0x05, 0x01, 0x03, 0x00, 0x2b, 0x2b,
//    0x02, 0x06, 0x3f,
    0x03, 0x2a, 0x00, 0x00,
//    0x02, 0x30, 0x13, // PLL 30Hz
//    0x02, 0x50, 0x57, // VCOM default
//    0x02, 0x60, 0x22, // TCON 24us
//    0x03, 0x61, 0x50,0x80, // resolution
//    0x02, 0x82, 0x12, // VCOM -1v
//    0x02, 0xe3, 0x33, // power saving
    0x01, 0x04, // power on
    BUSY_WAIT,
    0x02, 0x50, 0x97, // panel setting
    0x00 // end of table
};

const uint8_t epd42r_init_sequence[] = {
    0x01, 0x12, // soft reset
    BUSY_WAIT,
    0x02, 0x74, 0x54, // set analog block control
    0x02, 0x7e, 0x3b, // set digital block control
    0x03, 0x2b, 0x04, 0x63, // ACVCOM
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f, // Softstart
    0x04, 0x01, 0x27, 0x01, 0x00, // output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x31, // RAM X start/end
    0x05, 0x45, 0,0,0x2b, 0x01, // RAM Y start/end
    0x02, 0x3c, 0x01, // border (0=bk,1=wh,2=red)
    0x02, 0x18, 0x80, // temp sensor = internal
    0x02, 0x21, 0x00, // display update ctrl 1
    0x02, 0x22, 0xb1, // display update ctrl 2
    0x01, 0x20, // master activation
    BUSY_WAIT,
    0x02, 0x4e, 0x00, // RAM X counter
    0x03, 0x4f, 0x2b, 0x01, // RAM Y counter
    0x00
};
const uint8_t epd29r_init_sequence[] = {
    0x01, 0x12, // soft reset
    BUSY_WAIT,
    0x02, 0x74, 0x54, // set analog block control
    0x02, 0x7e, 0x3b, // set digital block control
    0x03, 0x2b, 0x04, 0x63, // ACVCOM
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f, // Softstart
    0x04, 0x01, 0x27, 0x01, 0x00, // output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x0f, // RAM X start/end
    0x05, 0x45, 0,0,0x27, 0x01, // RAM Y start/end
    0x02, 0x3c, 0x01, // border (0=bk,1=wh,2=red)
    0x02, 0x18, 0x80, // temp sensor = internal
    0x02, 0x21, 0x00, // display update ctrl 1
    0x02, 0x22, 0xb1, // display update ctrl 2
    0x01, 0x20, // master activation
    BUSY_WAIT,
    0x02, 0x4e, 0x00, // RAM X counter
    0x03, 0x4f, 0x27, 0x01, // RAM Y counter
    0x00
};
// for 152x152
const uint8_t epd29r_init_sequence_152[] = {
    0x01, 0x12, // soft reset
    BUSY_WAIT,
    0x02, 0x74, 0x54, // set analog block control
    0x02, 0x7e, 0x3b, // set digital block control
    0x03, 0x2b, 0x04, 0x63, // ACVCOM
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f, // Softstart
    0x04, 0x01, 0x97, 0x00, 0x00, // output control
    0x02, 0x11, 0x03, // data entry mode
    0x03, 0x44, 0x00, 0x12, // RAM X start/end
    0x05, 0x45, 0,0,0x97, 0x00, // RAM Y start/end
    0x02, 0x3c, 0x01, // border (0=bk,1=wh,2=red)
    0x02, 0x18, 0x80, // temp sensor = internal
    0x02, 0x21, 0x00, // display update ctrl 1
    0x02, 0x22, 0xb1, // display update ctrl 2
    0x01, 0x20, // master activation
    BUSY_WAIT,
    0x02, 0x4e, 0x00, // RAM X counter
    0x03, 0x4f, 0x97, 0x00, // RAM Y counter
    0x00
};
const uint8_t epd29_init_sequence_partial[] = {
    0x02, UC8151_PSR, 0x80 | 0x20 | 0x10 | 0x00 | 0x02 | 0x01, // RES_128x296, LUT_REG, FORMAT_BW, SHIFT_LEFT, BOOSTER_ON, RESET_NONE
    0x06, UC8151_PWR, 0x03, 0x00, 0x2b, 0x2b, 0x03,
    0x04, UC8151_BTST, 0x17,0x17,0x17, // booster soft-start config - START_10MS | STRENGTH_3 | OFF_6_58US
    0x01, UC8151_PON, // power on
    BUSY_WAIT,
    0x02, UC8151_PLL, 0x3a, // HZ_100
    0x02, UC8151_VDCS, 0x12, // VCOM DC setting
//    0x02, UC8151_CDI, 0x97,
    0x2d, UC8151_LUT_VCOM, // VCOM LUT
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        ,0x00, 0x00,
    0x2b, UC8151_LUT_WW, // WW LUT
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_BW,
        0x80, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_WB,
        0x40, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_BB,
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00 // end of table
};

const uint8_t epd42_init_sequence_partial[] = {
    0x02, UC8151_PSR, 0x80 | 0x20 | 0x10 | 0x00 | 0x02 | 0x01, // RES_128x296, LUT_REG, FORMAT_BW, SHIFT_LEFT, BOOSTER_ON, RESET_NONE
    0x06, UC8151_PWR, 0x03, 0x00, 0x2b, 0x2b, 0x03,
    0x04, UC8151_BTST, 0x17,0x17,0x17, // booster soft-start config - START_10MS | STRENGTH_3 | OFF_6_58US
    0x01, UC8151_PON, // power on
    BUSY_WAIT,
    0x02, UC8151_PLL, 0x3a, // HZ_100
    0x02, UC8151_VDCS, 0x12, // VCOM DC setting
//    0x02, UC8151_CDI, 0x97,
    0x2d, UC8151_LUT_VCOM, // VCOM LUT
    0x00, 0x0E, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_WW, // WW LUT
    0xA0, 0x0E, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_BW,
    0xA0, 0x0E, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_WB,
    0x50, 0x0E, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2b, UC8151_LUT_BB,
    0x50, 0x0E, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00 // end of table
};

const uint8_t st7302_wenting[] PROGMEM = {
    0x02, 0xEB, 0x02, // Enable OTP
    0x02, 0xD7, 0x68, // OTP Load Control
    0x02, 0xD1, 0x01, // Auto Power Control
    0x02, 0xC0, 0x40, // Gate Voltage setting
    0x07, 0xC1, 0x22, 0x22, 0x22, 0x22, 0x14, 0x00, // VSH Setting
    0x05, 0xC2, 0x00, 0x00, 0x00, 0x00, // VSL Setting
    0x02, 0xCB, 0x0E, // VCOMH Setting
    0x0B, 0xB4, 0xE5, 0x66, 0x85, 0xFF, 0xFF, 0x52, 0x85, 0xFF, 0xFF, 0x52, // Gate EQ
    0x03, 0xC7, 0xA6, 0xE9, // OSC Setting
    0x02, 0xB0, 0x3F, // Duty Setting
    0x03, 0xB2, 0x00, 0x00, // Frame Rate Setting (lowest for both HPM/LPM)
    0x02, 0x36, 0x00, // Memory Access Mode
    0x02, 0x3A, 0x11, // Data Format
    0x02, 0xB9, 0x23, // Source Setting
    0x02, 0xB8, 0x09, // Source Setting
    0x01, 0x11, // Sleep out
    0x02, 0xff, 100, // delay 100
    0x02, 0xD0, 0x1F, // enable auto power down
    0x01, 0x39, // Low power mode
    0x01, 0x29, // display on
    0x00
};

uint8_t st7302_hpm_init[] = {
    0x02, 0xEB, 0x02, // Enable OTP
    0x02, 0xD7, 0x68, // OTP Load Control
    0x02, 0xD1, 0x01, // Auto Power Control
    0x02, 0xC0, 0xba , // Gate Voltage setting
    0x02, 0xCB, 0x00, // VCOMH Setting
    0x02, 0xB3, 0x94, // VCOMEQ enable
    0x07, 0xC1, 0x28, 0x28, 0x28, 0x28, 0x14, 0x00, // source voltage VSH
    0x05, 0xC2, 0x00, 0x00, 0x00, 0x00, // VSL Setting
    0x03, 0xB2, 0x01, 0x05, // Frame Rate Setting (lowest for both HPM/LPM)
    0x0B, 0xB4, 0xA5, 0x66, 0xFD, 0xBF, 0x42, 0x55, 0x81, 0xBE, 0x42, 0x55, // Gate EQ
    0x02, 0xB7, 0x01, // Source eq enable
    0x01, 0x11, // Sleep out
    0x02, 0xff, 100, // delay 100
    0x02, 0x72, 0x13, // gate setting
    0x02, 0xB0, 0x3F, // Duty Setting
    0x03, 0xC7, 0xA6, 0xE9, // OSC Setting
    0x02, 0xD6, 0x00, // VSHLSEL source voltage select
    0x02, 0x36, 0x00, // Memory Access Mode
    0x02, 0x3A, 0x11, // Data Format
    0x02, 0xB9, 0x23, // Source Setting
    0x02, 0xB8, 0x09, // Source Setting
    0x02, 0x35, 0x00, // tearing effect line on
//    0x02, 0xD0, 0x1F, // enable auto power down
    0x01, 0x38, // High power mode
    0x01, 0x29, // display on
    0x00
};

// Initialization sequences
const unsigned char oled128_initbuf[] PROGMEM = {0x00, 0xae,0xdc,0x00,0x81,0x40,
      0xa1,0xc8,0xa8,0x7f,0xd5,0x50,0xd9,0x22,0xdb,0x35,0xb0,0xda,0x12,
      0xa4,0xa6,0xaf};

const unsigned char oled64x128_initbuf[] PROGMEM ={
0x00, 0xae, 0xd5, 0x51, 0x20, 0xa8, 0x3f, 0xdc, 0x00, 0xd3, 0x60, 0xad, 0x80, 0xa6, 0xa4, 0xa0, 0xc0, 0x81, 0x40, 0xd9, 0x22, 0xdb, 0x35, 0xaf
};

const unsigned char oled132_initbuf[] PROGMEM = {0x00,0xae,0x02,0x10,0x40,0x81,0xa0,0xc0,0xa6,0xa8,0x3f,0xd3,0x00,0xd5,0x80,0xd9,0xf1,0xda,0x12,0xdb,0x40,0x20,0x02,0xa4,0xa6};

const unsigned char oled64_initbuf[] PROGMEM ={0x00,0xae,0xa8,0x3f,0xd3,0x00,0x40,0xa1,0xc8,
      0xda,0x12,0x81,0xff,0xa4,0xa6,0xd5,0x80,0x8d,0x14,
      0xaf,0x20,0x02};

const unsigned char oled32_initbuf[] PROGMEM  = {
0x00,0xae,0xd5,0x80,0xa8,0x1f,0xd3,0x00,0x40,0x8d,0x14,0xa1,0xc8,0xda,0x02,
0x81,0x7f,0xd9,0xf1,0xdb,0x40,0xa4,0xa6,0xaf};

const unsigned char oled80_initbuf[] PROGMEM = {0x00,0xae,0xa1,0xc8,0xaf
};

const unsigned char oled72_initbuf[] PROGMEM ={0x00,0xae,0xa8,0x3f,0xd3,0x00,0x40,0xa1,0xc8,
      0xda,0x12,0x81,0xff,0xad,0x30,0xd9,0xf1,0xa4,0xa6,0xd5,0x80,0x8d,0x14,
      0xaf,0x20,0x02};

const unsigned char uc1701_initbuf[] PROGMEM  = {0xe2, 0x40, 0xa0, 0xc8, 0xa2, 0x2c, 0x2e, 0x2f, 0xf8, 0x00, 0x23, 0x81, 0x28, 0xac, 0x00, 0xa6};

const unsigned char hx1230_initbuf[] PROGMEM  = {0x2f, 0x90, 0xa6, 0xa4, 0xaf, 0x40, 0xb0, 0x10, 0x00};
const unsigned char nokia5110_initbuf[] PROGMEM  = {0x21, 0xa4, 0xb1, 0x04,0x14,0x20,0x0c};

static void obdCachedFlush(OBDISP *pOBD, int bRender);
static void obdCachedWrite(OBDISP *pOBD, uint8_t *pData, uint8_t u8Len, int bRender);
void obdSetPosition(OBDISP *pOBD, int x, int y, int bRender);
void obdWriteCommand(OBDISP *pOBD, unsigned char c);
void obdWriteDataBlock(OBDISP *pOBD, unsigned char *ucBuf, int iLen, int bRender);
const uint8_t ucMirror[256] PROGMEM =
     {0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240,
      8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248,
      4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244,
      12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252,
      2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242,
      10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250,
      6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246,
      14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254,
      1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241,
      9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249,
      5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245,
      13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253,
      3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243,
      11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251,
      7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247,
      15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255};

const uint8_t ucFont[]PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x5f,0x5f,0x06,0x00,
  0x00,0x07,0x07,0x00,0x07,0x07,0x00,0x14,0x7f,0x7f,0x14,0x7f,0x7f,0x14,
  0x24,0x2e,0x2a,0x6b,0x6b,0x3a,0x12,0x46,0x66,0x30,0x18,0x0c,0x66,0x62,
  0x30,0x7a,0x4f,0x5d,0x37,0x7a,0x48,0x00,0x04,0x07,0x03,0x00,0x00,0x00,
  0x00,0x1c,0x3e,0x63,0x41,0x00,0x00,0x00,0x41,0x63,0x3e,0x1c,0x00,0x00,
  0x08,0x2a,0x3e,0x1c,0x3e,0x2a,0x08,0x00,0x08,0x08,0x3e,0x3e,0x08,0x08,
  0x00,0x00,0x80,0xe0,0x60,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,
  0x00,0x00,0x00,0x60,0x60,0x00,0x00,0x60,0x30,0x18,0x0c,0x06,0x03,0x01,
  0x3e,0x7f,0x59,0x4d,0x47,0x7f,0x3e,0x40,0x42,0x7f,0x7f,0x40,0x40,0x00,
  0x62,0x73,0x59,0x49,0x6f,0x66,0x00,0x22,0x63,0x49,0x49,0x7f,0x36,0x00,
  0x18,0x1c,0x16,0x53,0x7f,0x7f,0x50,0x27,0x67,0x45,0x45,0x7d,0x39,0x00,
  0x3c,0x7e,0x4b,0x49,0x79,0x30,0x00,0x03,0x03,0x71,0x79,0x0f,0x07,0x00,
  0x36,0x7f,0x49,0x49,0x7f,0x36,0x00,0x06,0x4f,0x49,0x69,0x3f,0x1e,0x00,
  0x00,0x00,0x00,0x66,0x66,0x00,0x00,0x00,0x00,0x80,0xe6,0x66,0x00,0x00,
  0x08,0x1c,0x36,0x63,0x41,0x00,0x00,0x00,0x14,0x14,0x14,0x14,0x14,0x14,
  0x00,0x41,0x63,0x36,0x1c,0x08,0x00,0x00,0x02,0x03,0x59,0x5d,0x07,0x02,
  0x3e,0x7f,0x41,0x5d,0x5d,0x5f,0x0e,0x7c,0x7e,0x13,0x13,0x7e,0x7c,0x00,
  0x41,0x7f,0x7f,0x49,0x49,0x7f,0x36,0x1c,0x3e,0x63,0x41,0x41,0x63,0x22,
  0x41,0x7f,0x7f,0x41,0x63,0x3e,0x1c,0x41,0x7f,0x7f,0x49,0x5d,0x41,0x63,
  0x41,0x7f,0x7f,0x49,0x1d,0x01,0x03,0x1c,0x3e,0x63,0x41,0x51,0x33,0x72,
  0x7f,0x7f,0x08,0x08,0x7f,0x7f,0x00,0x00,0x41,0x7f,0x7f,0x41,0x00,0x00,
  0x30,0x70,0x40,0x41,0x7f,0x3f,0x01,0x41,0x7f,0x7f,0x08,0x1c,0x77,0x63,
  0x41,0x7f,0x7f,0x41,0x40,0x60,0x70,0x7f,0x7f,0x0e,0x1c,0x0e,0x7f,0x7f,
  0x7f,0x7f,0x06,0x0c,0x18,0x7f,0x7f,0x1c,0x3e,0x63,0x41,0x63,0x3e,0x1c,
  0x41,0x7f,0x7f,0x49,0x09,0x0f,0x06,0x1e,0x3f,0x21,0x31,0x61,0x7f,0x5e,
  0x41,0x7f,0x7f,0x09,0x19,0x7f,0x66,0x26,0x6f,0x4d,0x49,0x59,0x73,0x32,
  0x03,0x41,0x7f,0x7f,0x41,0x03,0x00,0x7f,0x7f,0x40,0x40,0x7f,0x7f,0x00,
  0x1f,0x3f,0x60,0x60,0x3f,0x1f,0x00,0x3f,0x7f,0x60,0x30,0x60,0x7f,0x3f,
  0x63,0x77,0x1c,0x08,0x1c,0x77,0x63,0x07,0x4f,0x78,0x78,0x4f,0x07,0x00,
  0x47,0x63,0x71,0x59,0x4d,0x67,0x73,0x00,0x7f,0x7f,0x41,0x41,0x00,0x00,
  0x01,0x03,0x06,0x0c,0x18,0x30,0x60,0x00,0x41,0x41,0x7f,0x7f,0x00,0x00,
  0x08,0x0c,0x06,0x03,0x06,0x0c,0x08,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
  0x00,0x00,0x03,0x07,0x04,0x00,0x00,0x20,0x74,0x54,0x54,0x3c,0x78,0x40,
  0x41,0x7f,0x3f,0x48,0x48,0x78,0x30,0x38,0x7c,0x44,0x44,0x6c,0x28,0x00,
  0x30,0x78,0x48,0x49,0x3f,0x7f,0x40,0x38,0x7c,0x54,0x54,0x5c,0x18,0x00,
  0x48,0x7e,0x7f,0x49,0x03,0x06,0x00,0x98,0xbc,0xa4,0xa4,0xf8,0x7c,0x04,
  0x41,0x7f,0x7f,0x08,0x04,0x7c,0x78,0x00,0x44,0x7d,0x7d,0x40,0x00,0x00,
  0x60,0xe0,0x80,0x84,0xfd,0x7d,0x00,0x41,0x7f,0x7f,0x10,0x38,0x6c,0x44,
  0x00,0x41,0x7f,0x7f,0x40,0x00,0x00,0x7c,0x7c,0x18,0x78,0x1c,0x7c,0x78,
  0x7c,0x78,0x04,0x04,0x7c,0x78,0x00,0x38,0x7c,0x44,0x44,0x7c,0x38,0x00,
  0x84,0xfc,0xf8,0xa4,0x24,0x3c,0x18,0x18,0x3c,0x24,0xa4,0xf8,0xfc,0x84,
  0x44,0x7c,0x78,0x4c,0x04,0x0c,0x18,0x48,0x5c,0x54,0x74,0x64,0x24,0x00,
  0x04,0x04,0x3e,0x7f,0x44,0x24,0x00,0x3c,0x7c,0x40,0x40,0x3c,0x7c,0x40,
  0x1c,0x3c,0x60,0x60,0x3c,0x1c,0x00,0x3c,0x7c,0x60,0x30,0x60,0x7c,0x3c,
  0x44,0x6c,0x38,0x10,0x38,0x6c,0x44,0x9c,0xbc,0xa0,0xa0,0xfc,0x7c,0x00,
  0x4c,0x64,0x74,0x5c,0x4c,0x64,0x00,0x08,0x08,0x3e,0x77,0x41,0x41,0x00,
  0x00,0x00,0x00,0x77,0x77,0x00,0x00,0x41,0x41,0x77,0x3e,0x08,0x08,0x00,
  0x02,0x03,0x01,0x03,0x02,0x03,0x01,0x70,0x78,0x4c,0x46,0x4c,0x78,0x70};
// AVR MCUs have very little memory; save 6K of FLASH by stretching the 'normal'
// font instead of using this large font
#ifndef __AVR__
const uint8_t ucBigFont[]PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xfc,0xfc,0xff,0xff,0xff,0xff,0xfc,0xfc,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
  0x00,0x00,0x0f,0x0f,0x3f,0x3f,0x00,0x00,0x00,0x00,0x3f,0x3f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xfc,0xfc,0xfc,0xfc,0xc0,0xc0,0xfc,0xfc,0xfc,0xfc,0xc0,0xc0,0x00,0x00,
  0xc0,0xc0,0xff,0xff,0xff,0xff,0xc0,0xc0,0xff,0xff,0xff,0xff,0xc0,0xc0,0x00,0x00,
  0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfc,0xfc,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0x0f,0x0f,0x3c,0x3c,0x00,0x00,
  0xf0,0xf0,0xc3,0xc3,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x00,0x00,0x03,0x03,0x03,0x03,0x3f,0x3f,0x3f,0x3f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xf0,0xf0,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xf0,0xf0,0x3c,0x3c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x3c,0x3c,0xff,0xff,0xc3,0xc3,0xff,0xff,0x3c,0x3c,0x00,0x00,0x00,0x00,
  0xfc,0xfc,0xff,0xff,0x03,0x03,0x0f,0x0f,0xfc,0xfc,0xff,0xff,0x03,0x03,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x30,0x30,0x3f,0x3f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xf0,0xf0,0xfc,0xfc,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0x03,0x0f,0x0f,0xfc,0xfc,0xf0,0xf0,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
  0x0c,0x0c,0xcc,0xcc,0xff,0xff,0x3f,0x3f,0x3f,0x3f,0xff,0xff,0xcc,0xcc,0x0c,0x0c,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x0c,0x0c,0x0c,0x0c,0xff,0xff,0xff,0xff,0x0c,0x0c,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x30,0x30,0x3f,0x3f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xf0,0xf0,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xf0,0xf0,0x3c,0x3c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfc,0xfc,0xff,0xff,0x03,0x03,0x03,0x03,0xc3,0xc3,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0xff,0xff,0xff,0xff,0x30,0x30,0x0f,0x0f,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x30,0x30,0x3c,0x3c,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x03,0x03,0xc3,0xc3,0xff,0xff,0x3c,0x3c,0x00,0x00,
  0xc0,0xc0,0xf0,0xf0,0x3c,0x3c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xf0,0xf0,0x3c,0x3c,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0xff,0xff,0xff,0xff,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,
  0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x0f,0x0f,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xf0,0xf0,0xfc,0xfc,0x0f,0x0f,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xff,0xff,0x00,0x00,
  0x00,0x00,0x00,0x00,0xf0,0xf0,0xfc,0xfc,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfc,0xfc,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0xfc,0xfc,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfc,0xfc,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xff,0xff,0x00,0x00,
  0x00,0x00,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xf0,0xf0,0x3c,0x3c,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x0c,0x0c,0x3f,0x3f,0xf3,0xf3,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0xc3,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x0c,0x0c,0x3c,0x3c,0xf0,0xf0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xf3,0xf3,0x3f,0x3f,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x3c,0x3c,0x3f,0x3f,0x03,0x03,0x03,0x03,0xc3,0xc3,0xff,0xff,0x3c,0x3c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x3f,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xf0,0xf0,0xfc,0xfc,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0xfc,0xfc,0xf0,0xf0,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x3f,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xf0,0xf0,0x3c,0x3c,0x0f,0x0f,0x3c,0x3c,0xf0,0xf0,0xc0,0xc0,0x00,0x00,
  0xff,0xff,0xff,0xff,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0xff,0xff,0xff,0xff,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xf0,0xf0,0xfc,0xfc,0x0f,0x0f,0x03,0x03,0x03,0x03,0x0f,0x0f,0x3c,0x3c,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0x00,0x00,
  0x00,0x00,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0x0f,0x0f,0xfc,0xfc,0xf0,0xf0,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0xc3,0xc3,0x0f,0x0f,0x3f,0x3f,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x03,0x03,0x0f,0x0f,0x00,0x00,0xc0,0xc0,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0xc3,0xc3,0x0f,0x0f,0x3f,0x3f,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x03,0x03,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xf0,0xf0,0xfc,0xfc,0x0f,0x0f,0x03,0x03,0x03,0x03,0x0f,0x0f,0x3c,0x3c,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x0c,0x0c,0x0c,0x0c,0xfc,0xfc,0xfc,0xfc,0x00,0x00,
  0x00,0x00,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x03,0x03,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0xff,0xff,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xff,0xff,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0x00,0x00,
  0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x00,0x00,0xf0,0xf0,0xff,0xff,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x0f,0x0f,0x3f,0x3f,0xf0,0xf0,0xc0,0xc0,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0xfc,0xfc,0xf0,0xf0,0xfc,0xfc,0xff,0xff,0xff,0xff,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x03,0x03,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0xfc,0xfc,0xf0,0xf0,0xc0,0xc0,0xff,0xff,0xff,0xff,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x03,0x03,0x0f,0x0f,0xff,0xff,0xff,0xff,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xf0,0xf0,0xfc,0xfc,0x0f,0x0f,0x03,0x03,0x0f,0x0f,0xfc,0xfc,0xf0,0xf0,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x00,0x00,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfc,0xfc,0xff,0xff,0x03,0x03,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0xc0,0xc0,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0f,0x0f,0xff,0xff,0xff,0xff,0xc3,0xc3,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x03,0x03,0x0f,0x0f,0xff,0xff,0xf0,0xf0,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x3c,0x3c,0xff,0xff,0xc3,0xc3,0x03,0x03,0x03,0x03,0x3f,0x3f,0x3c,0x3c,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0x03,0x03,0x03,0x03,0x0f,0x0f,0xfc,0xfc,0xf0,0xf0,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x3f,0x3f,0x0f,0x0f,0xff,0xff,0xff,0xff,0x0f,0x0f,0x3f,0x3f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x3f,0x3f,0xff,0xff,0xc0,0xc0,0x00,0x00,0xc0,0xc0,0xff,0xff,0x3f,0x3f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0x03,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0xff,0xff,0xff,0xff,0xc0,0xc0,0xfc,0xfc,0xc0,0xc0,0xff,0xff,0xff,0xff,0x00,0x00,
  0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0f,0x0f,0xff,0xff,0xf0,0xf0,0x00,0x00,0xf0,0xf0,0xff,0xff,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0xf0,0xf0,0xff,0xff,0x0f,0x0f,0xff,0xff,0xf0,0xf0,0x00,0x00,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x3f,0x3f,0x0f,0x0f,0x03,0x03,0x03,0x03,0xc3,0xc3,0xff,0xff,0x3f,0x3f,0x00,0x00,
  0xc0,0xc0,0xf0,0xf0,0x3c,0x3c,0x0f,0x0f,0x03,0x03,0x00,0x00,0xc0,0xc0,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfc,0xfc,0xf0,0xf0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x03,0x03,0x0f,0x0f,0x3f,0x3f,0xfc,0xfc,0xf0,0xf0,0xc0,0xc0,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xc0,0xc0,0xf0,0xf0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,
  0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0xf0,0xf0,0xfc,0xfc,0x0c,0x0c,0x0c,0x0c,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xc0,0xc0,0xc3,0xc3,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0xfc,0xfc,0xff,0xff,0x03,0x03,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xfc,0xfc,0xff,0xff,0x03,0x03,0x0f,0x0f,0x3c,0x3c,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0xc3,0xc3,0xcf,0xcf,0x0c,0x0c,0x0c,0x0c,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x03,0x03,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xc0,0xc0,0xcf,0xcf,0xcf,0xcf,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xcf,0xcf,0xcf,0xcf,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x00,0x00,0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,
  0x03,0x03,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x3c,0x3c,0xff,0xff,0xc3,0xc3,0x00,0x00,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0x03,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x03,0x03,0xff,0xff,0x03,0x03,0xff,0xff,0xff,0xff,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x0f,0x0f,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x03,0x03,0x00,0x00,0x03,0x03,0x0f,0x0f,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x3c,0x3c,0x30,0x30,0xf0,0xf0,0xc3,0xc3,0x03,0x03,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0xfc,0xfc,0xff,0xff,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
  0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0x03,0x0f,0x0f,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0xf0,0xf0,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0f,0x0f,0x03,0x03,0x0f,0x0f,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
  0x00,0x00,0x03,0x03,0xff,0xff,0xfc,0xfc,0xff,0xff,0x03,0x03,0x00,0x00,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
  0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,
  0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0xcc,0xcc,0xff,0xff,0x3f,0x3f,0x00,0x00,
  0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0x00,0x00,
  0x03,0x03,0xc3,0xc3,0xf0,0xf0,0x3c,0x3c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,
  0x0f,0x0f,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x0f,0x0f,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0xfc,0xff,0xff,0x03,0x03,0x03,0x03,0x00,0x00,
  0x00,0x00,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0c,0x0c,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0xfc,0xfc,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x03,0x03,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0xfc,0xff,0xff,0x03,0x03,0x03,0x03,0x00,0x00,
  0x00,0x00,0x0c,0x0c,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x0f,0x0f,0x0c,0x0c,0x0f,0x0f,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xc0,0xc0,0xf0,0xf0,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0xfc,0xfc,0xff,0xff,0x03,0x03,0x00,0x00,0x03,0x03,0xff,0xff,0xfc,0xfc,0x00,0x00,
  0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#endif // !__AVR__

  // 5x7 font (in 6x8 cell)
const uint8_t ucSmallFont[]PROGMEM = {
0x00,0x00,0x00,0x00,0x00,
0x00,0x06,0x5f,0x06,0x00,
0x07,0x03,0x00,0x07,0x03,
0x24,0x7e,0x24,0x7e,0x24,
0x24,0x2b,0x6a,0x12,0x00,
0x63,0x13,0x08,0x64,0x63,
0x36,0x49,0x56,0x20,0x50,
0x00,0x07,0x03,0x00,0x00,
0x00,0x3e,0x41,0x00,0x00,
0x00,0x41,0x3e,0x00,0x00,
0x08,0x3e,0x1c,0x3e,0x08,
0x08,0x08,0x3e,0x08,0x08,
0x00,0xe0,0x60,0x00,0x00,
0x08,0x08,0x08,0x08,0x08,
0x00,0x60,0x60,0x00,0x00,
0x20,0x10,0x08,0x04,0x02,
0x3e,0x51,0x49,0x45,0x3e,
0x00,0x42,0x7f,0x40,0x00,
0x62,0x51,0x49,0x49,0x46,
0x22,0x49,0x49,0x49,0x36,
0x18,0x14,0x12,0x7f,0x10,
0x2f,0x49,0x49,0x49,0x31,
0x3c,0x4a,0x49,0x49,0x30,
0x01,0x71,0x09,0x05,0x03,
0x36,0x49,0x49,0x49,0x36,
0x06,0x49,0x49,0x29,0x1e,
0x00,0x6c,0x6c,0x00,0x00,
0x00,0xec,0x6c,0x00,0x00,
0x08,0x14,0x22,0x41,0x00,
0x24,0x24,0x24,0x24,0x24,
0x00,0x41,0x22,0x14,0x08,
0x02,0x01,0x59,0x09,0x06,
0x3e,0x41,0x5d,0x55,0x1e,
0x7e,0x11,0x11,0x11,0x7e,
0x7f,0x49,0x49,0x49,0x36,
0x3e,0x41,0x41,0x41,0x22,
0x7f,0x41,0x41,0x41,0x3e,
0x7f,0x49,0x49,0x49,0x41,
0x7f,0x09,0x09,0x09,0x01,
0x3e,0x41,0x49,0x49,0x7a,
0x7f,0x08,0x08,0x08,0x7f,
0x00,0x41,0x7f,0x41,0x00,
0x30,0x40,0x40,0x40,0x3f,
0x7f,0x08,0x14,0x22,0x41,
0x7f,0x40,0x40,0x40,0x40,
0x7f,0x02,0x04,0x02,0x7f,
0x7f,0x02,0x04,0x08,0x7f,
0x3e,0x41,0x41,0x41,0x3e,
0x7f,0x09,0x09,0x09,0x06,
0x3e,0x41,0x51,0x21,0x5e,
0x7f,0x09,0x09,0x19,0x66,
0x26,0x49,0x49,0x49,0x32,
0x01,0x01,0x7f,0x01,0x01,
0x3f,0x40,0x40,0x40,0x3f,
0x1f,0x20,0x40,0x20,0x1f,
0x3f,0x40,0x3c,0x40,0x3f,
0x63,0x14,0x08,0x14,0x63,
0x07,0x08,0x70,0x08,0x07,
0x71,0x49,0x45,0x43,0x00,
0x00,0x7f,0x41,0x41,0x00,
0x02,0x04,0x08,0x10,0x20,
0x00,0x41,0x41,0x7f,0x00,
0x04,0x02,0x01,0x02,0x04,
0x80,0x80,0x80,0x80,0x80,
0x00,0x03,0x07,0x00,0x00,
0x20,0x54,0x54,0x54,0x78,
0x7f,0x44,0x44,0x44,0x38,
0x38,0x44,0x44,0x44,0x28,
0x38,0x44,0x44,0x44,0x7f,
0x38,0x54,0x54,0x54,0x08,
0x08,0x7e,0x09,0x09,0x00,
0x18,0xa4,0xa4,0xa4,0x7c,
0x7f,0x04,0x04,0x78,0x00,
0x00,0x00,0x7d,0x40,0x00,
0x40,0x80,0x84,0x7d,0x00,
0x7f,0x10,0x28,0x44,0x00,
0x00,0x00,0x7f,0x40,0x00,
0x7c,0x04,0x18,0x04,0x78,
0x7c,0x04,0x04,0x78,0x00,
0x38,0x44,0x44,0x44,0x38,
0xfc,0x44,0x44,0x44,0x38,
0x38,0x44,0x44,0x44,0xfc,
0x44,0x78,0x44,0x04,0x08,
0x08,0x54,0x54,0x54,0x20,
0x04,0x3e,0x44,0x24,0x00,
0x3c,0x40,0x20,0x7c,0x00,
0x1c,0x20,0x40,0x20,0x1c,
0x3c,0x60,0x30,0x60,0x3c,
0x6c,0x10,0x10,0x6c,0x00,
0x9c,0xa0,0x60,0x3c,0x00,
0x64,0x54,0x54,0x4c,0x00,
0x08,0x3e,0x41,0x41,0x00,
0x00,0x00,0x77,0x00,0x00,
0x00,0x41,0x41,0x3e,0x08,
0x02,0x01,0x02,0x01,0x00,
0x3c,0x26,0x23,0x26,0x3c};

#define MAX_CACHE 192
static uint8_t u8Cache[MAX_CACHE]; // for faster character drawing
static volatile uint8_t u8End = 0;

static void obdCachedFlush(OBDISP *pOBD, int bRender)
{
   if (u8End > 0) {
      obdWriteDataBlock(pOBD, u8Cache, u8End, bRender);
      u8End = 0;
   }
} /* obdCachedFlush() */

static void obdCachedWrite(OBDISP *pOBD, uint8_t *pData, uint8_t u8Len, int bRender)
{
   if (u8End + u8Len > MAX_CACHE) // need to flush it
   {
       obdCachedFlush(pOBD, bRender); // write the old data
   }
   memcpy(&u8Cache[u8End], pData, u8Len);
   u8End += u8Len;
  
} /* obdCachedWrite() */

//
// Draw the contents of a memory buffer onto a display
// The sub-window will be clipped if it specifies too large an area
// for the destination display. The source OBDISP structure must have
// a valid back buffer defined
// The top and bottom destination edges will be drawn on byte boundaries (8 rows)
// The source top/bot edges can be on pixel boundaries
// This can be used for partial screen updates
//
void obdDumpWindow(OBDISP *pOBDSrc, OBDISP *pOBDDest, int srcx, int srcy, int destx, int desty, int width, int height)
{
uint8_t *s,ucTemp[32]; // temp buffer to gather source pixels
int x, y, tx, i;
int iPitch;

   if (pOBDSrc == NULL || pOBDDest == NULL || pOBDSrc->ucScreen == NULL)
      return; // invalid pointers
   if (width > pOBDDest->width)
      width = pOBDDest->width;
   if (height > pOBDDest->height)
      height = pOBDDest->height;
   iPitch = pOBDSrc->width;
   for (y=0; y<height; y+=8)
   {
      obdSetPosition(pOBDDest, destx, (desty+y)/8, 1);
      for (x=0; x<width; x+=32)
      {
         tx = 32;
         if (width-x < 32) tx = width-x;
         s = &pOBDSrc->ucScreen[((srcy+y)/8)*iPitch + srcx+x];
     if (srcy & 7) // need to shift the bits to get 8 rows of src data
         {
            uint8_t uc, ucShift = srcy & 7;
            for (i=0; i<tx; i++)
            { // combine current and next line to capture 8 pixels
               uc = s[0] >> ucShift;
               uc |= s[iPitch] << (7-ucShift);
               ucTemp[i] = uc;
            }
            obdCachedWrite(pOBDDest, ucTemp, tx, 1);
         }
         else
         { // simpler case
            obdCachedWrite(pOBDDest, s, tx, 1); // just copy it
         }
      } // for x
   } // for y
   obdCachedFlush(pOBDDest, 1);
} /* obdDumpWindow() */

//
// Write a single line to a Sharp memory LCD
// You must provide the exact number of bytes needed for a complete line
// e.g. for the 144x168 display, pSrc must provide 144 pixels (18 bytes)
//
void obdWriteLCDLine(OBDISP *pOBD, uint8_t *pSrc, int iLine)
{
    int x;
    uint8_t c, ucInvert, *d, ucStart;
    uint8_t ucLineBuf[54]; // 400 pixels is max supported width = 50 bytes + 4
    int iPitch = pOBD->width / 8;
    static int iVCOM = 0;

//    if (pOBD == NULL || pSrc == NULL || pOBD->type < SHARP_144x168)
//        return; // invalid request
    if (iLine < 0 || iLine >= pOBD->height)
        return;
    
      ucInvert = (pOBD->invert) ? 0x00 : 0xff;
      digitalWrite(pOBD->iCSPin, HIGH); // active high

      ucStart = 0x80; // write command
      iVCOM++;
      if (iVCOM & 0x100) // flip it every 256 lines
        ucStart |= 0x40; // VCOM bit
      ucLineBuf[1] = ucStart;
      // this code assumes I2C, so the first byte is ignored
      RawWrite(pOBD, ucLineBuf, 2); // write command(01) + vcom(02)

     d = &ucLineBuf[2];
     ucLineBuf[1] = pgm_read_byte(&ucMirror[iLine+1]); // current line number
     for (x=0; x<iPitch; x++)
     {
         c = pSrc[0] ^ ucInvert; // we need to brute-force invert it
         *d++ = pgm_read_byte(&ucMirror[c]);
         pSrc++;
     } // for x
    // write this line to the display
    ucLineBuf[iPitch+2] = 0; // end of line
    RawWrite(pOBD, ucLineBuf, iPitch+3);
    ucLineBuf[1] = 0;
    RawWrite(pOBD, ucLineBuf, 2); // final transfer
    digitalWrite(pOBD->iCSPin, LOW); // de-activate
} /* obdWriteLCDLine() */

//
// Turn the display on or off
//
void obdPower(OBDISP *pOBD, int bOn)
{
uint8_t ucCMD;
    
    if (!pOBD) return;
    switch (pOBD->type) {
        case LCD_NOKIA5110:
            ucCMD = (bOn) ? 0x20 : 0x24;
            break;
        case LCD_ST7302:
            ucCMD = (bOn) ? 0x29 : 0x28;
            break;
        default: // all other supported displays
            ucCMD = (bOn) ? 0xaf : 0xae;
            break;
        // e-ink panels are put to sleep after each action
    }
  obdWriteCommand(pOBD, ucCMD);
} /* obdPower() */

// Controls the LED backlight
void obdBacklight(OBDISP *pOBD, int bOn)
{
  if (pOBD->iLEDPin != 0xff)
  {
     digitalWrite(pOBD->iLEDPin, (bOn) ? HIGH:LOW);
  }
} /* obdBacklight() */

//
// Send the command sequence to power up the LCDs
static void LCDPowerUp(OBDISP *pOBD)
{
#ifndef MEMORY_ONLY
    int iLen;
    uint8_t *s, uc[32];
    obdSetDCMode(pOBD, MODE_COMMAND);
    digitalWrite(pOBD->iCSPin, LOW);
    if (pOBD->type == LCD_UC1701 || pOBD->type == LCD_UC1609)
    {
        s = (uint8_t *)uc1701_initbuf;
        iLen = sizeof(uc1701_initbuf);
    }
    else if (pOBD->type == LCD_HX1230)
    {
        s = (uint8_t *)hx1230_initbuf;
        iLen = sizeof(hx1230_initbuf);
    }
    else // Nokia 5110
    {
        s = (uint8_t *)nokia5110_initbuf;
        iLen = sizeof(nokia5110_initbuf);
    }
    memcpy_P(uc, s, iLen);
#ifdef _LINUX_
    AIOWriteSPI(pOBD->bbi2c.file_i2c, uc, iLen);
#else
    if (pOBD->bBitBang)
        SPI_BitBang(pOBD, uc, iLen, pOBD->iMOSIPin, pOBD->iCLKPin);
    else
        mySPI->transfer(uc, iLen);
#endif
    _delay(100);
    obdWriteCommand(pOBD, 0xa5);
    _delay(100);
    obdWriteCommand(pOBD, 0xa4);
    obdWriteCommand(pOBD, 0xaf);
    if (pOBD->iCSPin != 0xff)
        digitalWrite(pOBD->iCSPin, HIGH);
    obdSetDCMode(pOBD, MODE_DATA);
#endif // MEMORY_ONLY
} /* LCDPowerUp() */
//
// More efficient means of sending commands, data and busy-pauses
//
void EPDSendCMDSequence(OBDISP *pOBD, const uint8_t *pSeq)
{
int iLen;
uint8_t ucTemp[128], *s;
    
    s = (uint8_t *)pSeq;
    while (s[0] != 0) {
        iLen = *s++;
        if (iLen == BUSY_WAIT) {
            EPDWaitBusy(pOBD);
        } else {
            obdWriteCommand(pOBD, s[0]);
            s++;
            memcpy(ucTemp, s, iLen-1);
            s += (iLen-1);
            RawWriteData(pOBD, ucTemp, iLen-1);
        }
    } // while more commands to send
} /* EPDSendCMDSequence() */

void EPD29_Init(OBDISP *pOBD)
{
uint8_t ucTemp[4];
    
    obdWriteCommand(pOBD, UC8151_PON); // power on
    EPDWaitBusy(pOBD);
    EPD213_CMD(pOBD, UC8151_PSR, 0x80 | 0x10 | 0x00 | 0x02 | 0x01); // panel setting
    obdWriteCommand(pOBD, UC8151_TRES); // resolution
    ucTemp[0] = (uint8_t)pOBD->native_width;
    ucTemp[1] = (uint8_t)(pOBD->native_height >> 8);
    ucTemp[2] = (uint8_t)(pOBD->native_height & 0xff);
    RawWriteData(pOBD, ucTemp, 3);
//    obdWriteCommand(pOBD, UC8151_CDI);
    EPDSleep(pOBD);
} /* EPD29_Init() */

unsigned char lut_w_full[] =
{
  0x60, 0x5A, 0x5A, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

unsigned char lut_b_full[] =
{
  0x90, 0x5A, 0x5A, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t epd213_lut[] =
{
	// black
	0x48, 0x50, 0x10, 0x10, 0x13, 0x00, 0x00,
	0x48, 0x50, 0x80, 0x00, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x48, 0x55, 0x00, 0xbb, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x10, 0x04, 0x04, 0x04, 0x04,
	0x10, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x08, 0x08, 0x10, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t epd293_lut_partial[] = {
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x0A, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
#ifdef FUTURE
//lut0 (KEEP) voltages
0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//lut1 (W2B) voltages
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//lut2 (B2W) voltages
0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//lut3 (unused) voltages
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//lut4 (vcom) voltages
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

//group0 phase lengths and repeat count
0xa, 0x00, 0x00, 0x00, 0x00,

//group1 not used
0x00, 0x00, 0x00, 0x00, 0x00,

//group2 not used
0x00, 0x00, 0x00, 0x00, 0x00,

//group3 phase lengths and repeat count
0x00, 0x00, 0x00, 0x00, 0x00,

//group4 phase lengths and repeat count
0x00, 0x00, 0x00, 0x00, 0x00,

//group5 phase lengths and repeat count
0x00, 0x00, 0x00, 0x00, 0x00,

//group6 phase lengths and repeat count
0x00, 0x00, 0x00, 0x00, 0x00
#endif
};

const uint8_t epd213_lut2_partial[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT0: BB:     VS 0 ~7
    0x80,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT1: BW:     VS 0 ~7
    0x40,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT2: WB:     VS 0 ~7
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT3: WW:     VS 0 ~7
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT4: VCOM:   VS 0 ~7

    0x0A,0x00,0x00,0x00,0x00,                       // TP0 A~D RP0
    0x00,0x00,0x00,0x00,0x00,                       // TP1 A~D RP1
    0x00,0x00,0x00,0x00,0x00,                       // TP2 A~D RP2
    0x00,0x00,0x00,0x00,0x00,                       // TP3 A~D RP3
    0x00,0x00,0x00,0x00,0x00,                       // TP4 A~D RP4
    0x00,0x00,0x00,0x00,0x00,                       // TP5 A~D RP5
    0x00,0x00,0x00,0x00,0x00,                       // TP6 A~D RP6

};

const uint8_t epd213_lut2_full[] =
{
	// black
	0x80,0x60,0x40,0x00,0x00,0x00,0x00,
	0x10,0x60,0x20,0x00,0x00,0x00,0x00,
	0x80,0x60,0x40,0x00,0x00,0x00,0x00,
	0x10,0x60,0x20,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x03,0x03,0x00,0x00,0x02,
	0x09,0x09,0x00,0x00,0x02,
	0x03,0x03,0x00,0x00,0x02,
	0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00
};

const uint8_t lut154_full_update[] =
    {
        0x80,0x48,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
        0x40,0x48,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
        0x80,   0x48,   0x40,   0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x40,   0x48,   0x80,   0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0xA,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x8,    0x1,    0x0,    0x8,    0x1,    0x0,    0x2,
        0xA,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
        0x22,   0x22,   0x22,   0x22,   0x22,   0x22,   0x0,    0x0,    0x0
    };

const uint8_t lut154_partial_update[] =
{
    0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0xF,0x0,0x0,0x0,0x0,0x0,0x0,
    0x1,0x1,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,
    0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0
};

void EPD29R_Init(OBDISP *pOBD)
{
} /* EPD29R_Init() */

void EPD213_Init(OBDISP *pOBD)
{
uint8_t ucLine[8];
    
    obdWriteCommand(pOBD, SSD1608_SW_RESET);
    EPDWaitBusy(pOBD);
    EPD213_CMD(pOBD, 0x74, 0x54); // set analog block control
    EPD213_CMD(pOBD, 0x7e, 0x3b); // set digital block control
    obdWriteCommand(pOBD, 0x01); // gate setting
    ucLine[0] = pOBD->native_height-1;
    ucLine[1] = 0;
    ucLine[2] = 0;
    RawWriteData(pOBD, ucLine, 3);
    EPD213_CMD(pOBD, 0x11, 0x03); // data entry mode = x/y incremented
    EPD213_CMD(pOBD, 0x3c, (pOBD->type == EPD213_104x212) ? 0x00 : 0x03); // black border
    EPD213_CMD(pOBD, 0x2c, (pOBD->type == EPD213_104x212) ? 0x3c : 0x55); // VCOM register

    if (pOBD->type == EPD213_122x250) {
       EPD213_CMD(pOBD, 0x03, 0x15); // gate driving voltage
       obdWriteCommand(pOBD, 0x04); // source driving voltage
       ucLine[0] = 0x41;
       ucLine[1] = 0xa8;
       ucLine[2] = 0x32;
       RawWriteData(pOBD, ucLine, 3);
       EPD213_CMD(pOBD, 0x3a, 0x30); // dummy line period
       EPD213_CMD(pOBD, 0x3b, 0x0a); // gate line width
    } else { // 104x212
       EPD213_CMD(pOBD, 0x03, 0x17); // gate driving voltage
       obdWriteCommand(pOBD, 0x04); // source driving voltage
       ucLine[0] = 0x41;
       ucLine[1] = 0xac;
       ucLine[2] = 0x32;
       RawWriteData(pOBD, ucLine, 3);
       EPD213_CMD(pOBD, 0x3a, 0x07); // dummy line period
       EPD213_CMD(pOBD, 0x3b, 0x04); // gate line width
    }
    EPD213_CMD(pOBD, 0x4e, 0x00); // set RAM X pointer start
    ucLine[0] = 0x00;
    ucLine[1] = 0x00;
    obdWriteCommand(pOBD, 0x4f); // set RAM Y pointer start
    RawWriteData(pOBD, ucLine, 2);
    EPDWaitBusy(pOBD);

} /* EPD213_Init() */

void EPD154_Init(OBDISP *pOBD)
{
uint8_t ucLine[8];
    
    obdWriteCommand(pOBD, SSD1608_SW_RESET); // Soft Reset
    EPDWaitBusy(pOBD);
    obdWriteCommand(pOBD, SSD1608_DRIVER_CONTROL); // gate setting
    ucLine[0] = pOBD->native_height-1;
    ucLine[1] = 0; // high bit of line count
    ucLine[2] = 0x01; // bit1 = interlaced, bit 0 = Y order (0 = increasing, 1 = decreasing)
    RawWriteData(pOBD, ucLine, 3);
    EPD213_CMD(pOBD, SSD1608_DATA_MODE, 0x03); // data entry mode = x/y incremented
    EPD213_CMD(pOBD, SSD1608_WRITE_BORDER, 0x01);
    EPD213_CMD(pOBD, 0x18, 0x80); // ?
    EPD213_CMD(pOBD, SSD1608_DISP_CTRL2, 0xb1); // load temperature and waveform setting
    obdWriteCommand(pOBD, SSD1608_MASTER_ACTIVATE);
    EPDWaitBusy(pOBD);
} /* EPD154_Init() */

//
// Initialize the display controller on an SPI bus
//
void obdSPIInit(OBDISP *pOBD, int iType, int iDC, int iCS, int iReset, int iMOSI, int iCLK, int iLED, int bFlip, int bInvert, int bBitBang, int32_t iSpeed)
{
#ifndef MEMORY_ONLY
uint8_t uc[32], *s;
int iLen;

    if (pOBD == NULL)
        return;
    pOBD->iTimeout = 5000; // 5 seconds for most e-ink panels
    pOBD->can_flip = 1; // assume can flip 180
  pOBD->ucScreen = NULL; // start with no backbuffer; user must provide one later
  pOBD->iDCPin = iDC;
  pOBD->iCSPin = iCS;
  pOBD->iRSTPin = iReset;
  pOBD->iMOSIPin = iMOSI;
  pOBD->iCLKPin = iCLK;
  pOBD->iLEDPin = iLED;
  pOBD->type = iType;
  pOBD->flip = bFlip;
  pOBD->invert = bInvert;
  pOBD->bBitBang = bBitBang;
  pOBD->wrap = 0; // default - disable text wrap
  pOBD->com_mode = COM_SPI; // communication mode
  if (pOBD->iDCPin != 0xff) // Note - not needed on Sharp Memory LCDs
  {
      pinMode(pOBD->iDCPin, OUTPUT);
      digitalWrite(pOBD->iDCPin, 0); // for some reason, command mode must be set or some OLEDs/LCDs won't initialize correctly even if set later
  }
  if (pOBD->iCSPin != 0xff)
  {
      pinMode(pOBD->iCSPin, OUTPUT);
      digitalWrite(pOBD->iCSPin, HIGH); //(pOBD->type < SHARP_144x168)); // set to not-active
  }
  if (bBitBang)
  {
      pinMode(iMOSI, OUTPUT);
      pinMode(iCLK, OUTPUT);
  }

  // Reset it
  if (pOBD->iRSTPin != 0xff)
  {
    pinMode(pOBD->iRSTPin, OUTPUT);
    digitalWrite(pOBD->iRSTPin, HIGH);
    _delay(200);
    digitalWrite(pOBD->iRSTPin, LOW);
    _delay(10);
    digitalWrite(pOBD->iRSTPin, HIGH);
    _delay(200);
  }
  if (iLED != -1 && iLED != 0xff)
  {
      if (iType >= EPD42_400x300) // for EPD displays, LED = BUSY
         pinMode(iLED, INPUT);
      else
         pinMode(iLED, OUTPUT);
  }
// Initialize SPI
    if (!pOBD->bBitBang) {
#ifdef _LINUX_
        pOBD->bbi2c.file_i2c = AIOOpenSPI(SPI_BUS_NUMBER, iSpeed);
#else
#ifdef ARDUINO_ARCH_RP2040
        if (iMOSI == -1 || iMOSI == 0xff) {
            iMOSI = MOSI; iCLK = SCK; // use the default pins
        }
        mySPI = new MbedSPI(-1,iMOSI,iCLK);
        mySPI->begin();
#endif
#if defined( ARDUINO_ARCH_ESP32 ) || defined( RISCV )
if (iMOSI != -1 && iMOSI != 0xff)
    mySPI->begin(iCLK, -1, iMOSI, iCS);
else
    mySPI->begin();
#endif
        mySPI->beginTransaction(SPISettings(iSpeed, MSBFIRST, SPI_MODE0));
        mySPI->endTransaction(); // N.B. - if you call beginTransaction() again without a matching endTransaction(), it will hang on ESP32
#endif
    //  SPI.setClockDivider(16);
       //  SPI.setBitOrder(MSBFIRST);
        //  SPI.setDataMode(SPI_MODE0);
    }
    pOBD->native_width = pOBD->width = 128; // assume 128x64
  pOBD->height = 64;
  if (iType == LCD_ST7302)
  {
      pOBD->native_width = pOBD->width = 250;
      pOBD->native_height = pOBD->height = 122;
      pOBD->can_flip = 0;
  }
  if (iType == OLED_80x128)
  {
      pOBD->native_width = pOBD->width = 80;
      pOBD->native_height = pOBD->height = 128;
  }
  else if (iType == SHARP_144x168)
  {
      pOBD->native_width = pOBD->width = 144;
      pOBD->native_height = pOBD->height = 168;
      pOBD->can_flip = 0;
      pOBD->iDCPin = 0xff; // no D/C wire on this display
  }
  else if (iType == SHARP_400x240)
  {
      pOBD->native_width = pOBD->width = 400;
      pOBD->native_height = pOBD->height = 240;
      pOBD->can_flip = 0;
      pOBD->iDCPin = 0xff; // no D/C wire on this display
  }
  else if (iType == EPD27_176x264)
  {
      pOBD->native_width = pOBD->width = 176;
      pOBD->native_height = pOBD->height = 264;
      pOBD->busy_idle = HIGH;
      pOBD->can_flip = 0;
      pOBD->iFlags |= OBD_CS_EVERY_BYTE;
      return; // nothing else to do yet
  }
  else if (iType == EPD42_400x300)
  {
      pOBD->native_width = pOBD->width = 400;
      pOBD->native_height = pOBD->height = 300;
      pOBD->busy_idle = HIGH;
      pOBD->can_flip = 0;
      return; // nothing else to do yet
  }
  else if (iType == EPD293_128x296)
  {
      pOBD->native_width = pOBD->width = 128;
      pOBD->native_height = pOBD->height = 296;
      pOBD->busy_idle = LOW;
      pOBD->can_flip = 0;
      return; // nothing else to do yet
  }
  else if (iType == EPD213_122x250 || iType == EPD29B_128x296)
  {
        pOBD->native_width = pOBD->width = 122;
        pOBD->native_height = pOBD->height = 250;
        pOBD->can_flip = 0;
        pOBD->busy_idle = LOW;
        EPD213_Init(pOBD);
  }
  else if (iType == EPD42R2_400x300 || iType == EPD213R_104x212) {
      if (iType == EPD213R_104x212) {
          pOBD->native_width = pOBD->width = 104;
          pOBD->native_height = pOBD->height = 212;
          pOBD->type = EPD42R2_400x300; // use the same commands
      } else {
          pOBD->native_width = pOBD->width = 400;
          pOBD->native_height = pOBD->height = 300;
      }
      pOBD->can_flip = 0;
      pOBD->busy_idle = HIGH;
      pOBD->iFlags |= OBD_3COLOR;
      pOBD->iTimeout = 25000; // 3-color need a longer timeout (25 seconds)
  }
  else if (iType == EPD29R_128x296 || iType == EPD154R_152x152 || iType == EPD42R_400x300)
  { // BLACK/WHITE/RED
      if (iType == EPD29R_128x296) {
        pOBD->native_width = pOBD->width = 128;
        pOBD->native_height = pOBD->height = 296;
      } else if (iType == EPD154R_152x152){
          pOBD->native_width = pOBD->width = 152;
          pOBD->native_height = pOBD->height = 152;
      } else {
          pOBD->native_width = pOBD->width = 400;
          pOBD->native_height = pOBD->height = 300;
      }
      pOBD->type = EPD29R_128x296; // use the rest of the same code
      pOBD->iTimeout = 25000; // 3-color need a longer timeout (25 seconds)
        pOBD->iFlags |= OBD_3COLOR;
        pOBD->can_flip = 0;
        pOBD->busy_idle = LOW;
        EPD29R_Init(pOBD);
        return;
  }
  else if (iType == EPD102_80x128)
  {
      pOBD->native_width = pOBD->width = 80;
      pOBD->native_height = pOBD->height = 128;
      pOBD->can_flip = 0; // flip display commands don't exist
      pOBD->busy_idle = HIGH;
      pOBD->iFlags |= OBD_CS_EVERY_BYTE;
      return;
  }
  else if (iType == EPD213_104x212)
  {
      pOBD->native_width = pOBD->width = 104;
      pOBD->native_height = pOBD->height = 212;
      pOBD->can_flip = 0; // flip display commands don't exist
      pOBD->busy_idle = LOW;
      EPD213_Init(pOBD);
      return;
  }
  else if (iType == EPD154_152x152)
  {
      pOBD->native_width = pOBD->width = 152;
      pOBD->native_height = pOBD->height = 152;
      pOBD->can_flip = 0; // flip display commands don't exist
      pOBD->busy_idle = LOW;
      pOBD->iFlags |= OBD_CS_EVERY_BYTE;
      pOBD->type = EPD213_104x212;
      EPD29_Init(pOBD);
      return;
  }
  else if (iType == EPD154_200x200)
  {
      pOBD->native_width = pOBD->width = 200;
      pOBD->native_height = pOBD->height = 200;
      pOBD->can_flip = 0; // flip display commands don't exist
      pOBD->busy_idle = LOW;
      EPD154_Init(pOBD);
      return;
  }
  else if (iType == EPD29_128x296 || iType == EPD213B_104x212)
  {
      if (iType == EPD29_128x296) {
          pOBD->native_width = pOBD->width = 128;
          pOBD->native_height = pOBD->height = 296;
          pOBD->busy_idle = HIGH;
      } else {
          pOBD->native_width = pOBD->width = 104;
          pOBD->native_height = pOBD->height = 212;
          pOBD->busy_idle = LOW;
          pOBD->type = EPD29_128x296; // fool it into using this config
      }
      pOBD->can_flip = 0; // flip display commands don't exist
      EPD29_Init(pOBD);
      return;
  } else if (iType == EPD47_540x960) {
      pOBD->native_width = pOBD->width = 540;
      pOBD->native_height = pOBD->height = 960;
      pOBD->can_flip = 0; // flip display commands don't exist
      pOBD->busy_idle = HIGH;
      return;
  }
  else if (iType == LCD_UC1609)
  {
      pOBD->native_width = pOBD->width = 192;
      pOBD->native_height = pOBD->height = 64;
  }
  else if (iType == LCD_HX1230)
  {
      pOBD->native_width = pOBD->width = 96;
      pOBD->native_height = pOBD->height = 68;
      pOBD->iDCPin = 0xff; // flag this as being 3-wire SPI
  }
  else if (iType == LCD_NOKIA5110)
  {
      pOBD->native_width = pOBD->width = 84;
      pOBD->native_height = pOBD->height = 48;
  }
  else if (iType == OLED_96x16)
  {
      pOBD->native_width = pOBD->width = 96;
      pOBD->native_height = pOBD->height = 16;
  }
  else if (iType == OLED_64x128)
  {
      // NOTE: 64x128 SH1107 doesn't seem to properly support
      // segment remap, so the init commands are A0/C0 and
      // it can't be flipped 180
      pOBD->can_flip = 0;
      pOBD->native_width = pOBD->width = 64;
      pOBD->native_height = pOBD->height = 128;
  }
  else if (iType == OLED_128x32)
      pOBD->native_height = pOBD->height = 32;
  else if (iType == OLED_128x128)
      pOBD->native_height = pOBD->height = 128;
  else if (iType == OLED_64x48)
  {
      pOBD->native_width = pOBD->width = 64;
      pOBD->native_height = pOBD->height = 48;
  }
  else if (iType == OLED_64x32)
  {
      pOBD->native_width = pOBD->width = 64;
      pOBD->native_height = pOBD->height = 32;
  }
  else if (iType == OLED_72x40)
  {
      pOBD->native_width = pOBD->width = 72;
      pOBD->native_height = pOBD->height = 40;
  }
  if (iType == OLED_80x128)
  {
     s = (uint8_t *)oled80_initbuf;
     iLen = sizeof(oled80_initbuf);
  }
  else if (iType == OLED_128x32 || iType == OLED_96x16)
  {
     s = (uint8_t *)oled32_initbuf;
     iLen = sizeof(oled32_initbuf);
  }
  else if (iType == OLED_64x128)
  {
      s = (uint8_t *)oled64x128_initbuf;
     iLen = sizeof(oled64x128_initbuf);
  }
  else if (iType == OLED_128x128)
  {
     s = (uint8_t *)oled128_initbuf;
     iLen = sizeof(oled128_initbuf);
  }
  else if (iType == OLED_132x64) { // SH1106
     s = (uint8_t *)oled132_initbuf;
     iLen = sizeof(oled132_initbuf);
  }
  else if (iType < LCD_UC1701) // 128x64 and 64x32
  {
     s = (uint8_t *)oled64_initbuf;
     iLen = sizeof(oled64_initbuf);
  }
    // OLED
  if (iType < LCD_UC1701)
  {
      memcpy_P(uc, s, iLen); // do it from RAM
      RawWrite(pOBD, uc, iLen);
      _delay(100); // on SPI display this delay is needed or the display
      // never sees the "display on" command at the end of the sequence
      obdWriteCommand(pOBD, 0xaf); // turn on display
      if (bInvert)
      {
        uc[0] = 0; // command
        uc[1] = 0xa7; // invert command
        RawWrite(pOBD, uc, 2);
      }
      if (bFlip) // rotate display 180
      {
        uc[0] = 0; // command
        uc[1] = 0xa0;
        RawWrite(pOBD, uc, 2);
        uc[0] = 0;
        uc[1] = 0xc0;
        RawWrite(pOBD, uc, 2);
      }
  } // OLED
  if (iType == LCD_ST7302)
  {
    uint8_t ucTemp[16];
 //     uint8_t *s = (uint8_t *)st7302_hpm_init;
      uint8_t *s = (uint8_t *)st7302_wenting; //st7302_lpm_init;
    iLen = 1;

      while (iLen) {
          iLen = pgm_read_byte(s++); // parameter byte count
          if (iLen) {
              if (pgm_read_byte(s) == 0xff) { // delay
                  _delay(pgm_read_byte(&s[1]));
                  s += 2;
              } else {
                  obdWriteCommand(pOBD, pgm_read_byte(s));
                  if (iLen > 1) {
                      memcpy_P(ucTemp, s, iLen);
                      RawWrite(pOBD, ucTemp, iLen);
                  }
                  s += iLen;
              }
          }
      } // while commands to transmit
      if (pOBD->invert)
          obdWriteCommand(pOBD, 0x21); // inversion on
      // Clear RAM
//      obdWriteCommand(pOBD, 0xb9);
//      uc[0] = 0x40;
//      uc[1] = 0xe3;
//      RawWrite(pOBD, uc, 2);
//      _delay(1000);
//      obdWriteCommand(pOBD, 0xb9);
//      uc[0] = 0x40;
//      uc[1] = 0x23;
//      RawWrite(pOBD, uc, 2);
      return;
  } /* ST7302 */
    
  if (iType == LCD_UC1701 || iType == LCD_HX1230)
  {
      uint8_t cCOM = 0xc0;
      LCDPowerUp(pOBD);
      if (iType == LCD_HX1230)
      {
          obdSetContrast(pOBD, 0); // contrast of 0 looks good
          cCOM = 0xc8;
      }
      if (bFlip) // flip horizontal + vertical
      {
         obdWriteCommand(pOBD, 0xa1); // set SEG direction (A1 to flip horizontal)
         obdWriteCommand(pOBD, cCOM); // set COM direction (C0 to flip vert)
      }
      if (bInvert)
      {
         obdWriteCommand(pOBD, 0xa7); // set inverted pixel mode
      }
  }
  if (iType == LCD_UC1609)
  {
      LCDPowerUp(pOBD);
      return;
      obdWriteCommand(pOBD, 0xe2); // system reset
      _delay(100);
      obdWriteCommand(pOBD, 0xa0); // set frame rate to 76fps
      obdWriteCommand(pOBD, 0xeb); // set BR
      obdWriteCommand(pOBD, 0x2f); // set Power Control
      obdWriteCommand(pOBD, 0xc4); // set LCD mapping control
      obdWriteCommand(pOBD, 0x81); // set PM
      obdWriteCommand(pOBD, 0x90); // set contrast to 144
      _delay(100);
      obdWriteCommand(pOBD, 0xaf); // display enable
      if (bFlip) // flip horizontal + vertical
      {
         obdWriteCommand(pOBD, 0xa1); // set SEG direction (A1 to flip horizontal)
         obdWriteCommand(pOBD, 0xc2); // set COM direction (C0 to flip vert)
      }
      if (bInvert)
      {
         obdWriteCommand(pOBD, 0xa7); // set inverted pixel mode
      }
  } // UC1609
#endif // MEMORY_ONLY
} /* obdSPIInit() */
void obdSetRotation(OBDISP *pOBD, int iRotation)
{
    pOBD->iScreenOffset = 0;
    
    switch (iRotation) {
        default: return;
        case 0:
            obdSetFlip(pOBD, 0);
            pOBD->iOrientation = 0;
            pOBD->width = pOBD->native_width;
            pOBD->height = pOBD->native_height;
            break;
        case 1:
        case 90:
            obdSetFlip(pOBD, 0);
            pOBD->iOrientation = 90;
            pOBD->width = pOBD->native_height;
            pOBD->height = pOBD->native_width;
            break;
        case 2:
        case 180:
            pOBD->iOrientation = 180;
            obdSetFlip(pOBD, 1);
            pOBD->width = pOBD->native_width;
            pOBD->height = pOBD->native_height;
            break;
        case 3:
        case 270:
            pOBD->iOrientation = 270;
            obdSetFlip(pOBD, 1);
            pOBD->width = pOBD->native_height;
            pOBD->height = pOBD->native_width;
            break;
  }
} /* obdSetRotation() */
//
// Set the memory configuration to display the pixels at 0 or 180 degrees (flipped)
// Pass true (1) to flip 180, false (0) to set to 0
//
void obdSetFlip(OBDISP *pOBD, int iOnOff)
{
   if (pOBD == NULL) return;
   if (!pOBD->can_flip) return; // don't try it on some displays
    
   pOBD->flip = iOnOff;
   if (pOBD->type == LCD_UC1701 || pOBD->type == LCD_UC1609)
   {
      if (iOnOff) // rotate display 180
      {
         obdWriteCommand(pOBD, 0xa1); // SEG direction (A1 to flip horizontal)
         obdWriteCommand(pOBD, 0xc0); // COM direction (C0 to flip vert)
      } else { // non-rotated
         obdWriteCommand(pOBD, 0xa0);
         obdWriteCommand(pOBD, 0xc8);
      }
   } else { // OLEDs
      if (iOnOff)
      {
          obdWriteCommand(pOBD, 0xa0);
          obdWriteCommand(pOBD, 0xc0);
      } else {
          obdWriteCommand(pOBD, 0xa1);
          obdWriteCommand(pOBD, 0xc8);
      }
   }
} /* obdSetFlip() */
//
// Initializes the OLED controller into "page mode"
//
int obdI2CInit(OBDISP *pOBD, int iType, int iAddr, int bFlip, int bInvert, int bWire, int sda, int scl, int reset, int32_t iSpeed)
{
int rc = OLED_NOT_FOUND;
#ifndef MEMORY_ONLY
unsigned char uc[32];
uint8_t u8Len, *s;

  if (pOBD == NULL)
      return -1;
    pOBD->can_flip = 1; // assume can flip 180
    if (iType < 0 || iType >= LCD_COUNT)
        return -1; // invalid display type
  if (sda == scl) { // invalid situation, set to -1
    sda = scl = -1;
  }
  pOBD->type = iType;
  pOBD->flip = bFlip;
  pOBD->invert = bInvert;
  pOBD->wrap = 0; // default - disable text wrap
  pOBD->bbi2c.iSDA = sda;
  pOBD->bbi2c.iSCL = scl;
  pOBD->bbi2c.bWire = bWire;
  pOBD->com_mode = COM_I2C; // communication mode
#ifdef _LINUX_
  pOBD->bbi2c.iBus = sda;
#endif
  I2CInit(&pOBD->bbi2c, iSpeed); // on Linux, SDA = bus number, SCL = device address
#ifdef _LINUX_
  pOBD->oled_addr = (uint8_t)scl;
#else
  // Reset it
  if (reset != 0xff && reset != -1)
  {
    pinMode(reset, OUTPUT);
    digitalWrite(reset, HIGH);
    _delay(50);
    digitalWrite(reset, LOW);
    _delay(50);
    digitalWrite(reset, HIGH);
    _delay(10);
  }
  // find the device address if requested
  if (iAddr == -1 || iAddr == 0 || iAddr == 0xff) // find it
  {
    I2CTest(&pOBD->bbi2c, 0x3c);
    if (I2CTest(&pOBD->bbi2c, 0x3c))
       pOBD->oled_addr = 0x3c;
    else if (I2CTest(&pOBD->bbi2c, 0x3d))
       pOBD->oled_addr = 0x3d;
    else
       return rc; // no display found!
  }
  else
  {
    pOBD->oled_addr = iAddr;
    I2CTest(&pOBD->bbi2c, iAddr);
    if (!I2CTest(&pOBD->bbi2c, iAddr))
       return rc; // no display found
  }

#endif
  // Detect the display controller (SSD1306, SH1107 or SH1106)
  uint8_t u = 0;
  I2CReadRegister(&pOBD->bbi2c, pOBD->oled_addr, 0x00, &u, 1); // read the status register
  u &= 0x0f; // mask off power on/off bit
  if ((u == 0x7 || u == 0xf) && pOBD->type == OLED_128x128) // SH1107
  { // A single SSD1306 display returned 7, so only act on it if the
    // user specified that they're working with a 128x128 display
    rc = OLED_SH1107_3C;
    bFlip = !bFlip; // SH1107 seems to have this reversed from the usual direction
  }
  else if (u == 0x8) // SH1106
  {
    rc = OLED_SH1106_3C;
    pOBD->type = OLED_132x64; // needs to be treated a little differently
  }
  else if (u == 3 || u == 6 || u == 7) // 7=128x64(rare),6=128x64 display, 3=smaller
  {
    rc = OLED_SSD1306_3C;
  }
  if (pOBD->oled_addr == 0x3d)
     rc++; // return the '3D' version of the type

  if (iType == OLED_80x128)
  {
      s = (uint8_t *)oled80_initbuf;
      u8Len = sizeof(oled80_initbuf);
  }
  if (iType == OLED_128x32 || iType == OLED_96x16)
  {
      s = (uint8_t *)oled32_initbuf;
      u8Len = sizeof(oled32_initbuf);
  }
  else if (iType == OLED_128x128)
  {
      s = (uint8_t *)oled128_initbuf;
      u8Len = sizeof(oled128_initbuf);
  }
  else if (iType == OLED_72x40)
  {
      s = (uint8_t *)oled72_initbuf;
      u8Len = sizeof(oled72_initbuf);
  }
  else if (iType == OLED_64x128)
  {
      s = (uint8_t *)oled64x128_initbuf;
      u8Len = sizeof(oled64x128_initbuf);
      pOBD->can_flip = 0; // this display can't flip 180 (don't know why)
  }
  else // 132x64, 128x64, 64x48 and 64x32
  {
      s = (uint8_t *)oled64_initbuf;
      u8Len = sizeof(oled64_initbuf);
  }

    memcpy_P(uc, s, u8Len);
  RawWrite(pOBD, uc, u8Len);
  if (bInvert)
  {
    uc[0] = 0; // command
    uc[1] = 0xa7; // invert command
    RawWrite(pOBD,uc, 2);
  }
  if (bFlip) // rotate display 180
  {
    uc[0] = 0; // command
    uc[1] = 0xa0;
    RawWrite(pOBD,uc, 2);
    uc[1] = 0xc0;
    RawWrite(pOBD,uc, 2);
  }
  pOBD->native_width = pOBD->width = 128; // assume 128x64
  pOBD->native_height = pOBD->height = 64;
  if (iType == OLED_96x16)
  {
      pOBD->native_width = pOBD->width = 96;
      pOBD->native_height = pOBD->height = 16;
  }
  else if (iType == OLED_80x128)
  {
      pOBD->native_width = pOBD->width = 80;
      pOBD->native_height = pOBD->height = 128;
  }
  else if (iType == OLED_64x128)
  {
      pOBD->native_width = pOBD->width = 64;
      pOBD->native_height = pOBD->height = 128;
  }
  else if (iType == OLED_128x32)
      pOBD->native_height = pOBD->height = 32;
  else if (iType == OLED_128x128)
      pOBD->native_height = pOBD->height = 128;
  else if (iType == OLED_64x32)
  {
      pOBD->native_width = pOBD->width = 64;
      pOBD->native_height = pOBD->height = 32;
  }
  else if (iType == OLED_64x48)
  {
      pOBD->native_width = pOBD->width = 64;
      pOBD->native_height = pOBD->height = 48;
  }
  else if (iType == OLED_72x40)
  {
      pOBD->native_width = pOBD->width = 72;
      pOBD->native_height = pOBD->height = 40;
  }
#endif // MEMORY_ONLY
  return rc;
} /* obdI2CInit() */
//
// Sends a command to turn on or off the OLED display
//
void oledPower(OBDISP *pOBD, uint8_t bOn)
{
    if (bOn)
      obdWriteCommand(pOBD, 0xaf); // turn on OLED
    else
      obdWriteCommand(pOBD, 0xae); // turn off OLED
} /* oledPower() */

#ifdef _LINUX_
void _delay(int iDelay)
{
    usleep(iDelay * 1000);
} /* _delay() */
void delayMicroseconds(int iDelay)
{
    usleep(iDelay);
} /* delayMicroseconds() */
#else // Arduino
void _delay(int iDelay)
{
#ifdef HAL_ESP32_HAL_H_
// light sleep to save power
    esp_sleep_enable_timer_wakeup(iDelay * 1000);
    esp_light_sleep_start();
#else // any other platform
    delay(iDelay); // use the Arduino delay function
#endif // ESP32
}
#endif // _LINUX_
//
// Bit Bang the data on GPIO pins
//
void SPI_BitBang(OBDISP *pOBD, uint8_t *pData, int iLen, uint8_t iMOSIPin, uint8_t iSCKPin)
{
int i;
uint8_t c;

// We can access the GPIO ports much quicker on AVR by directly manipulating
// the port registers
#ifdef __AVR__
volatile uint8_t *outSCK, *outMOSI; // port registers for fast I/O
uint8_t port, bitSCK, bitMOSI; // bit mask for the chosen pins

    port = digitalPinToPort(iMOSIPin);
    outMOSI = portOutputRegister(port);
    bitMOSI = digitalPinToBitMask(iMOSIPin);
    port = digitalPinToPort(iSCKPin);
    outSCK = portOutputRegister(port);
    bitSCK = digitalPinToBitMask(iSCKPin);

#endif

   while (iLen)
   {
      c = *pData++;
      iLen--;
      if (pOBD->iDCPin == 0xff) // 3-wire SPI, write D/C bit first
      {
#ifdef __AVR__
          if (pOBD->mode == MODE_DATA)
             *outMOSI |= bitMOSI;
          else
             *outMOSI &= ~bitMOSI;
          *outSCK |= bitSCK; // toggle clock
          *outSCK &= ~bitSCK; // no delay needed on SPI devices since AVR is slow
#else
          digitalWrite(iMOSIPin, (pOBD->mode == MODE_DATA));
          digitalWrite(iSCKPin, HIGH);
          delayMicroseconds(1);
      digitalWrite(iSCKPin, LOW);
#endif
      }
#ifdef FUTURE
      if (c == 0 || c == 0xff) // quicker for all bits equal
      {
#ifdef __AVR__
         if (c & 1)
            *outMOSI |= bitMOSI;
         else
            *outMOSI &= ~bitMOSI;
         for (i=0; i<8; i++)
         {
            *outSCK |= bitSCK;
            *outSCK &= ~bitSCK;
         }
#else
         digitalWrite(iMOSIPin, (c & 1));
         for (i=0; i<8; i++)
         {
            digitalWrite(iSCKPin, HIGH);
            delayMicroseconds(1);
            digitalWrite(iSCKPin, LOW);
         }
#endif
      }
      else
#endif // FUTURE
      {
         for (i=0; i<8; i++)
         {
#ifdef __AVR__
            if (c & 0x80) // MSB first
               *outMOSI |= bitMOSI;
            else
               *outMOSI &= ~bitMOSI;
            *outSCK |= bitSCK;
            c <<= 1;
            *outSCK &= ~bitSCK;
#else
            digitalWrite(iMOSIPin,  (c & 0x80) != 0); // MSB first
             delayMicroseconds(1);
            digitalWrite(iSCKPin, HIGH);
            c <<= 1;
            delayMicroseconds(1);
            digitalWrite(iSCKPin, LOW);
            delayMicroseconds(1);
#endif
        }
      }
   }
} /* SPI_BitBang() */

// Sets the D/C pin to data or command mode
void obdSetDCMode(OBDISP *pOBD, int iMode)
{
  if (pOBD->iDCPin == 0xff) // 9-bit SPI
      pOBD->mode = (uint8_t)iMode;
  else // set the GPIO line
      digitalWrite(pOBD->iDCPin, (iMode == MODE_DATA));
} /* obdSetDCMode() */

static void obdWriteCommand2(OBDISP *pOBD, unsigned char c, unsigned char d)
{
unsigned char buf[4];

    if (pOBD->com_mode == COM_I2C) {// I2C device
        buf[0] = 0x00;
        buf[1] = c;
        buf[2] = d;
        RawWrite(pOBD, buf, 3);
    } else { // must be SPI
        obdWriteCommand(pOBD, c);
        obdWriteCommand(pOBD, d);
    }
} /* obdWriteCommand2() */

//
// Sets the brightness (0=off, 255=brightest)
//
void obdSetContrast(OBDISP *pOBD, unsigned char ucContrast)
{
  if (pOBD->type == LCD_HX1230)
  { // valid values are 0-31, so scale it
      ucContrast >>= 3;
      obdWriteCommand(pOBD, 0x80 + ucContrast);
  }
  else if (pOBD->type == LCD_NOKIA5110)
  {
      // we allow values of 0xb0-0xbf, so shrink the range
      ucContrast >>= 4;
      obdWriteCommand(pOBD, 0x21); // set advanced command mode
      obdWriteCommand(pOBD, 0xb0 | ucContrast);
      obdWriteCommand(pOBD, 0x20); // set simple command mode
  }
  else // OLEDs + UC1701
      obdWriteCommand2(pOBD, 0x81, ucContrast);
} /* obdSetContrast() */
static void EPDWaitBusy(OBDISP *pOBD)
{
#ifndef MEMORY_ONLY
int iTimeout = 0;

    if (pOBD->iLEDPin == 0xff || pOBD->iLEDPin == -1) {
        _delay(2000); // no busy pin, wait 1 second
        return;
    }
  while (1) {
//      if (pOBD->type == EPD29_128x296) {
//          obdWriteCommand(pOBD, UC8151_FLG);
//      }
     if (digitalRead(pOBD->iLEDPin) == pOBD->busy_idle)
         break;
     iTimeout += 100;
     if (iTimeout > (int)pOBD->iTimeout)
         break; // DEBUG - timeout
     _delay(100);
  }
  if (iTimeout > (int)pOBD->iTimeout)
#ifdef ARDUINO
     Serial.println("EPDWaitBusy() timed out");
#else
     printf("EPDWaitBusy() timed out\n");
#endif
#endif // MEMORY_ONLY
} /* EPDWaitBusy() */

void EPD42_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial)
{
uint8_t ucTemp[8];
    
    obdWriteCommand(pOBD, UC8151_PWR); // POWER SETTING
    ucTemp[0] = 0x03;   // VDS_EN, VDG_EN internal
    ucTemp[1] = 0x00;   // VCOM_HV, VGHL_LV=16V
    ucTemp[2] = 0x2b;   // VDH=11V
    ucTemp[3] = 0x2b;   // VDL=11V
    ucTemp[4] = 0xff;   // VDHR
    RawWriteData(pOBD, ucTemp, 5);
    obdWriteCommand(pOBD, UC8151_BTST); // boost soft start
    ucTemp[0] = 0x17;   // A
    ucTemp[1] = 0x17;   // B
    ucTemp[2] = 0x17;   // C
    RawWriteData(pOBD, ucTemp, 3);
    EPD213_CMD(pOBD, UC8151_PSR, 0x3f); // panel setting
    EPD213_CMD(pOBD, UC8151_PLL, 0x3a); // PLL setting 100HZ   29 150Hz 39 200HZ 31 171HZ
    obdWriteCommand(pOBD, UC8151_TRES); // resolution setting
    ucTemp[0] = (uint8_t)(pOBD->native_width >> 8);
    ucTemp[1] = (uint8_t)(pOBD->native_width);
    ucTemp[2] = (uint8_t)(pOBD->native_height >> 8);
    ucTemp[3] = (uint8_t)(pOBD->native_height);
    RawWriteData(pOBD, ucTemp, 4);
    EPD213_CMD(pOBD, UC8151_VDCS, 0x12); // vcom_DC setting = -0.1 + 18 * -0.05 = -1.0V from OTP, slightly better
    EPD213_CMD(pOBD, UC8151_CDI, 0xd7); // VCOM AND DATA INTERVAL SETTING
  //  ucTemp[0] = 0xd7;  // border floating to avoid flashing
    obdWriteCommand(pOBD, UC8151_PON);
    EPDWaitBusy(pOBD); // power on
    if (bPartial) {
        EPDSendCMDSequence(pOBD, epd42_init_sequence_partial);
    } else {
        EPDSendCMDSequence(pOBD, epd29_init_sequence_full);
    }
} /* EPD42_Begin() */

void EPD27_SetLut(OBDISP *pOBD, int bFast)
{
    if (bFast) {
        obdWriteCommand(pOBD, UC8151_LUT_VCOM); //vcom
        RawWriteData(pOBD, (uint8_t *)lut_fast_vcom_dc, 44);
       
        obdWriteCommand(pOBD, UC8151_LUT_WW);
        RawWriteData(pOBD, (uint8_t *)lut_fast_ww, 42);
       
        obdWriteCommand(pOBD, UC8151_LUT_BW);
        RawWriteData(pOBD, (uint8_t *)lut_fast_bw, 42);

        obdWriteCommand(pOBD, UC8151_LUT_WB);
        RawWriteData(pOBD, (uint8_t *)lut_fast_bb, 42);

        obdWriteCommand(pOBD, UC8151_LUT_BB);
        RawWriteData(pOBD, (uint8_t *)lut_fast_wb, 42);
    } else {
        obdWriteCommand(pOBD, UC8151_LUT_VCOM); //vcom
        RawWriteData(pOBD, (uint8_t *)lut_vcom_dc, 44);
       
        obdWriteCommand(pOBD, UC8151_LUT_WW);
        RawWriteData(pOBD, (uint8_t *)lut_ww, 42);
       
        obdWriteCommand(pOBD, UC8151_LUT_BW);
        RawWriteData(pOBD, (uint8_t *)lut_bw, 42);

        obdWriteCommand(pOBD, UC8151_LUT_WB);
        RawWriteData(pOBD, (uint8_t *)lut_bb, 42);

        obdWriteCommand(pOBD, UC8151_LUT_BB);
        RawWriteData(pOBD, (uint8_t *)lut_wb, 42);
    }
}

void EPD27_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial)
{
uint8_t ucTemp[8];
    
//    obdWriteCommand(pOBD, UC8151_PON);
//    EPDWaitBusy(pOBD);
    obdWriteCommand(pOBD, UC8151_PWR); // POWER SETTING
    ucTemp[0] = 0x03;   // VDS_EN, VDG_EN internal
    ucTemp[1] = 0x00;   // VCOM_HV, VGHL_LV=16V
    ucTemp[2] = 0x2b;   // VDH=11V
    ucTemp[3] = 0x2b;   // VDL=11V
    ucTemp[4] = 0x09;   // VDHR
    RawWriteData(pOBD, ucTemp, 5);
    obdWriteCommand(pOBD, UC8151_BTST); // boost soft start
    ucTemp[0] = 0x07;   // A
    ucTemp[1] = 0x07;   // B
    ucTemp[2] = 0x17;   // C
    RawWriteData(pOBD, ucTemp, 3);
    obdWriteCommand(pOBD, 0xf8); // power optimization
    ucTemp[0] = 0x60;
    ucTemp[1] = 0xa5;
    RawWriteData(pOBD, ucTemp, 2);
    obdWriteCommand(pOBD, 0xf8); // power optimization (again)
    ucTemp[0] = 0x89;
    ucTemp[1] = 0xa5;
    RawWriteData(pOBD, ucTemp, 2);
    obdWriteCommand(pOBD, 0xf8); // power optimization
    ucTemp[0] = 0x90;
    ucTemp[1] = 0x00;
    RawWriteData(pOBD, ucTemp, 2);
    obdWriteCommand(pOBD, 0xf8); // power optimization
    ucTemp[0] = 0x93;
    ucTemp[1] = 0x2A;
    RawWriteData(pOBD, ucTemp, 2);
    obdWriteCommand(pOBD, 0xf8); // power optimization
    ucTemp[0] = 0xa0;
    ucTemp[1] = 0xa5;
    RawWriteData(pOBD, ucTemp, 2);
    obdWriteCommand(pOBD, 0xf8); // power optimization
    ucTemp[0] = 0xa1;
    ucTemp[1] = 0xa0;
    RawWriteData(pOBD, ucTemp, 2);
    obdWriteCommand(pOBD, 0xf8); // power optimization
    ucTemp[0] = 0x73;
    ucTemp[1] = 0x41;
    RawWriteData(pOBD, ucTemp, 2);
    EPD213_CMD(pOBD, 0x16, 0); // partial refresh off
    obdWriteCommand(pOBD, UC8151_PON); // power on
    EPDWaitBusy(pOBD);
    EPD213_CMD(pOBD, UC8151_PSR, 0xaf); // panel setting
    EPD213_CMD(pOBD, UC8151_PLL, 0x3a); // PLL setting 100HZ   29 150Hz 39 200HZ 31 171HZ
    EPD213_CMD(pOBD, UC8151_VDCS, 0x12); // vcom_DC setting = -0.1 + 18 * -0.05 = -1.0V from OTP, slightly better
    _delay(2);
    
//    obdWriteCommand(pOBD, UC8151_TRES); // resolution setting
//    ucTemp[0] = (uint8_t)(pOBD->native_width >> 8);
//    ucTemp[1] = (uint8_t)(pOBD->native_width);
//    ucTemp[2] = (uint8_t)(pOBD->native_height >> 8);
//    ucTemp[3] = (uint8_t)(pOBD->native_height);
//    RawWriteData(pOBD, ucTemp, 4);
//    EPD213_CMD(pOBD, UC8151_CDI, 0x07); // VCOM AND DATA INTERVAL SETTING (white border)
    EPD27_SetLut(pOBD, 0); // fast mode not ready bPartial);
} /* EPD27_Begin() */

//
// Wake up the EPD controller
//
static void EPDWakeUp(OBDISP *pOBD)
{
uint8_t ucTemp[8];

    if (pOBD->type == EPD29_128x296) {
        obdWriteCommand(pOBD, UC8151_PON);
        return;
    }
    if (pOBD->type == EPD213_104x212 || pOBD->type == EPD213_122x250 || pOBD->type == EPD154_200x200 || pOBD->type == EPD154_152x152 || pOBD->type == EPD293_128x296) {
        if (pOBD->iRSTPin != 0xff) {
            digitalWrite(pOBD->iRSTPin, LOW);
            _delay(10);
            digitalWrite(pOBD->iRSTPin, HIGH);
            _delay(200);
        }
//        if (pOBD->type == EPD293_128x296) return;
        
        obdWriteCommand(pOBD, SSD1608_SW_RESET); // soft reset
        EPDWaitBusy(pOBD);
        return;
    }
    // The following is for the 4.2" 400x300 e-ink and 1.02"
  if (pOBD->iRSTPin != 0xff)
  {
    digitalWrite(pOBD->iRSTPin, LOW);
    _delay(10);
    digitalWrite(pOBD->iRSTPin, HIGH);
    _delay(10);
  }
    if (pOBD->type == EPD102_80x128) {
        return; // DEBUG
        EPD213_CMD(pOBD, UC8151_PSR, 0x4f); // panel setting
        obdWriteCommand(pOBD, 0x2a); // POWER SETTING
        ucTemp[0] = 0x00;   // VDS_EN, VDG_EN internal
        ucTemp[1] = 0x00;   // VCOM_HV, VGHL_LV=16V
        RawWriteData(pOBD, ucTemp, 2);
        obdWriteCommand(pOBD, UC8151_PON);
        EPDWaitBusy(pOBD); // power on
        EPD213_CMD(pOBD, UC8151_CDI, 0x97); // VCOM AND DATA INTERVAL SETTING
        return;
    }
} /* EPDWakeUp() */

static void EPDSleep(OBDISP *pOBD)
{

    if (pOBD->type == EPD29_128x296 || pOBD->type == EPD42R2_400x300) {
        obdWriteCommand(pOBD, UC8151_POF); // power off
        return;
    }
    if (pOBD->type == EPD29R_128x296 || pOBD->type == EPD154R_152x152 ||
        pOBD->type == EPD42R_400x300 || pOBD->type == EPD293_128x296) {
        EPD213_CMD(pOBD, 0x10, 0x01); // deep sleep
        return;
    }
    if (pOBD->type == EPD213_104x212 || pOBD->type == EPD213_122x250 || pOBD->type == EPD154_200x200 || pOBD->type == EPD154_152x152) {
        EPD213_CMD(pOBD, SSD1608_DEEP_SLEEP, 0x01); // enter deep sleep
	    return;
    }
  EPD213_CMD(pOBD, UC8151_CDI, 0x17); // border floating
  obdWriteCommand(pOBD, UC8151_POF); // power off
  EPDWaitBusy(pOBD);
  if (pOBD->iRSTPin != 0xff)
  {
    EPD213_CMD(pOBD, UC8151_DSLP, 0xa5); // deep sleep
  }
} /* EPDSleep() */
//
// Set the boundaries of the partial update area
// start/end x/y
//
void EPDSetPartialArea(OBDISP *pOBD, int sx, int sy, int ex, int ey)
{
uint8_t ucTemp[12];

  ucTemp[0] = 0x40; // data operation
  sx &= 0xFFF8; // byte boundary
  ex = (ex - 1) | 0x7; // byte boundary - 1
  obdWriteCommand(pOBD, 0x90); // partial window
  ucTemp[1] = (uint8_t)(sx / 256);
  ucTemp[2] = (uint8_t)(sx & 256);
  ucTemp[3] = (uint8_t)(ex / 256);
  ucTemp[4] = (uint8_t)(ex & 256);
  ucTemp[5] = (uint8_t)(sy / 256);
  ucTemp[6] = (uint8_t)(sy & 256);
  ucTemp[7] = (uint8_t)(ey / 256);
  ucTemp[8] = (uint8_t)(ey & 256);
  ucTemp[9] = 0x01; // gates scan inside and outside the partial area (don't see any difference)
  RawWrite(pOBD, ucTemp, 10);
  _delay(2);
  //IO.writeDataTransaction(0x00); // don't see any difference
  //return (7 + xe - x) / 8; // number of bytes to transfer
} /* EPDSetPartialArea() */

void EPD154_Finish(OBDISP *pOBD, int bPartial)
{
    if (bPartial) {
        EPD213_CMD(pOBD, SSD1608_DISP_CTRL2, (pOBD->type == EPD154_200x200 || pOBD->type == EPD154_152x152) ? 0xcf : 0x0c);
    } else {
        EPD213_CMD(pOBD, SSD1608_DISP_CTRL2, 0xc7);
    }
    obdWriteCommand(pOBD, SSD1608_MASTER_ACTIVATE);
    EPDWaitBusy(pOBD);
} /* EPD154_Finish() */

void EPD29_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial)
{
uint8_t ucLine[8];
    
    if (bPartial) {
        EPDSendCMDSequence(pOBD, epd29_init_sequence_partial);
        obdWriteCommand(pOBD, UC8151_PON); // power on
        obdWriteCommand(pOBD, UC8151_PTIN); // partial mode
        obdWriteCommand(pOBD, UC8151_PTL); // partial window
        ucLine[0] = 0; //y; X START
        ucLine[1] = pOBD->native_width-1; //(y + h - 1); X END
        ucLine[2] = 0; //(x >> 8); // Y START
        ucLine[3] = 0; //x & 0xff;
        ucLine[4] = (uint8_t)((pOBD->native_height-1) >> 8); //(x + w - 1) >> 8; Y END >> 8
        ucLine[5] = (uint8_t)((pOBD->native_height-1) & 0xff); //(x + w - 1) & 0xff;
        ucLine[6] = 0x28; // ?
        RawWriteData(pOBD, ucLine, 7); // boundaries
    } else {
        EPDSendCMDSequence(pOBD, epd29_init_sequence_full);
        obdWriteCommand(pOBD, UC8151_PTOU); // disable partial mode
    }
} /* EPD29_Begin() */

void EPD29_Finish(OBDISP *pOBD, int bPartial)
{
    obdWriteCommand(pOBD, UC8151_DSP); // stop data
    obdWriteCommand(pOBD, UC8151_DRF); // start display refresh
    _delay(10); // needed
    EPDWaitBusy(pOBD);
} /* EPD29_Finish() */
//
// Special case for some e-ink displays
//
static void EPDDumpFast(OBDISP *pOBD, uint8_t *pBuffer, int x, int y, int w, int h)
{
    if (pBuffer == NULL)
      pBuffer = pOBD->ucScreen;
    if (pBuffer == NULL)
        return;
    
    EPDWakeUp(pOBD);
    
    if (pOBD->type == EPD29R_128x296 || pOBD->type == EPD154R_152x152 || pOBD->type == EPD42R_400x300) {
        if (pOBD->iFlags & OBD_FULLUPDATE) {
            // Kludge alert - fool it into going into B&W mode to allow partial updates. Need to combine the black+red pixels and send them to the ALTRAM first
            uint8_t *s = pOBD->ucScreen;
            int iOffset = ((pOBD->native_width+7)/8) * pOBD->native_height; // full buffer size
            for (int i=0; i<iOffset; i++) {
                s[i] |= s[i+iOffset]; // combine red+black pixels
            }
            // do a fake full update first
            EPD213_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, false);
            EPDWriteImage(pOBD, SSD1608_WRITE_RAM, 0, 0, pOBD->width, pOBD->height, 0);
            EPDWriteImage(pOBD, SSD1608_WRITE_ALTRAM, 0, 0, pOBD->width, pOBD->height, 0); // send image to current and previous buffers
            return;
        }
        EPD213_Begin(pOBD, x, y, w, h, true);
        EPDWriteImage(pOBD, SSD1608_WRITE_RAM, x, y, w, h, 0);
        EPD154_Finish(pOBD, true);
    }
    if (pOBD->type == EPD29_128x296) {
        EPD29_Begin(pOBD, x, y, w, h, true);
        EPDWriteImage(pOBD, UC8151_DTM2, x, y, w, h, 0);
        EPD29_Finish(pOBD, true);
    } else if (pOBD->type == EPD213_104x212 || pOBD->type == EPD213_122x250) {
        EPD213_Begin(pOBD, x, y, w, h, true);
        EPDWriteImage(pOBD, SSD1608_WRITE_RAM, x, y, w, h, 0);
        EPD154_Finish(pOBD, true);
    } else if (pOBD->type == EPD154_200x200 || pOBD->type == EPD154_152x152) {
        EPD154_Begin(pOBD, x, y, w, h, true);
        EPDWriteImage(pOBD, SSD1608_WRITE_RAM, x, y, w, h, 0);
        EPD154_Finish(pOBD, true);
    } else if (pOBD->type == EPD293_128x296) {
        EPD293_Begin(pOBD, x, y, w, h, true);
        EPDWriteImage(pOBD, 0x24, 0, 0, pOBD->width, pOBD->height, 0);
        EPD213_CMD(pOBD, 0x22, 0xcf);
        obdWriteCommand(pOBD, 0x20); // refresh
        EPDWaitBusy(pOBD);
     //   delay(5000);
    }
    if (pOBD->type == EPD42_400x300) {
        EPD42_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, true);
        EPDWriteImage(pOBD, UC8151_DTM2, 0, 0, pOBD->width, pOBD->height, 0);
        obdWriteCommand(pOBD, UC8151_DRF);
    }
    if (pOBD->type == EPD27_176x264) {
        EPD27_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, true);
        EPDWriteImage(pOBD, UC8151_DTM2, 0, 0, pOBD->width, pOBD->height, 0);
        obdWriteCommand(pOBD, UC8151_DRF);
    }
    EPDSleep(pOBD);
} /* EPDDumpFast() */

void EPD213_CMD(OBDISP *pOBD, uint8_t cmd, uint8_t param)
{
	obdWriteCommand(pOBD, cmd);
	RawWriteData(pOBD, &param, 1);
} /* EPD213_CMD() */

void EPD213_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial)
{
uint8_t ucLine[8];

    // set luts
    obdWriteCommand(pOBD, SSD1608_WRITE_LUT);
//    if (pOBD->type == EPD213_104x212) {
//       RawWriteData(pOBD, (uint8_t *)epd213_lut, sizeof(epd213_lut));
//    } else { // 122x250
        if (bPartial)
            RawWriteData(pOBD, (uint8_t *)epd213_lut2_partial, sizeof(epd213_lut2_partial));
        else
            RawWriteData(pOBD, (uint8_t *)epd213_lut2_full, sizeof(epd213_lut2_full));
//    }

    obdWriteCommand(pOBD, 0x44); // set RAM X start/end
    if (pOBD->iOrientation == 0 || pOBD->iOrientation == 180) {
        ucLine[0] = x/8;
        ucLine[1] = (x+w-1)/8;
    } else { // 90/270
        ucLine[0] = y/8;
        ucLine[1] = (y+h-1)/8;
    }
    RawWriteData(pOBD, ucLine, 2);
    obdWriteCommand(pOBD, 0x45); // set RAM Y start/end
    if (pOBD->iOrientation == 0 || pOBD->iOrientation == 180) {
        ucLine[0] = (uint8_t)y;
        ucLine[1] = (uint8_t)(y>>8);
        ucLine[2] = (uint8_t)(y+h-1);
        ucLine[3] = (uint8_t)((y+h-1)>>8);
    } else { // 90/270
        ucLine[0] = (uint8_t)x;
        ucLine[1] = (uint8_t)(x>>8);
        ucLine[2] = (uint8_t)(x+w-1);
        ucLine[3] = (uint8_t)((x+w-1)>>8);
    }
    RawWriteData(pOBD, ucLine, 4);
    EPD213_CMD(pOBD, 0x4e, 0x00); // set RAM X pointer start
    ucLine[0] = 0x00;
    ucLine[1] = 0x00;
    obdWriteCommand(pOBD, 0x4f); // set RAM Y pointer start
    RawWriteData(pOBD, ucLine, 2);
    if (bPartial) {
        EPD213_CMD(pOBD, SSD1608_WRITE_VCOM, 0x26); // VCOM
        EPDWaitBusy(pOBD);
        obdWriteCommand(pOBD, 0x37); // ?
        memset(ucLine, 0, 7);
        ucLine[4] = 0x40;
        RawWriteData(pOBD, ucLine, 7);
        
        EPD213_CMD(pOBD, 0x22, 0xc0);
        obdWriteCommand(pOBD, 0x20);
        EPDWaitBusy(pOBD);
        EPD213_CMD(pOBD, 0x3c, 0x01); // border waveform
    } else {
        EPD213_CMD(pOBD, SSD1608_WRITE_VCOM, 0x55); // VCOM
        EPDWaitBusy(pOBD);
    }
} /* EPD213_Begin() */

void EPD154_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial)
{
uint8_t ucLine[16];

    obdWriteCommand(pOBD, SSD1608_WRITE_LUT);
    if (bPartial) {
        // send partial update lut
        RawWriteData(pOBD, (uint8_t *)lut154_partial_update, sizeof(lut154_partial_update));
        EPD213_CMD(pOBD, 0x3f, 0x2); // ?
        EPD213_CMD(pOBD, SSD1608_GATE_VOLTAGE, 0x17);
        ucLine[0] = 0x41;
        ucLine[1] = 0xb0;
        ucLine[2] = 0x32;
        obdWriteCommand(pOBD, SSD1608_SOURCE_VOLTAGE);
        RawWriteData(pOBD, ucLine, 3);
        EPD213_CMD(pOBD, SSD1608_WRITE_VCOM, 0x28); // VCOM
        
        obdWriteCommand(pOBD, 0x37); // ?
        memset(ucLine, 0, 10);
        ucLine[5] = 0x40;
        RawWriteData(pOBD, ucLine, 10);
        
        EPD213_CMD(pOBD, 0x3c, 0x80); // VCOM?
        EPD213_CMD(pOBD, 0x22, 0xc0);
        obdWriteCommand(pOBD, 0x20);
        EPDWaitBusy(pOBD);
    } else {
        // send full update lut
        RawWriteData(pOBD, (uint8_t *)lut154_full_update, sizeof(lut154_full_update));
        EPD213_CMD(pOBD, 0x3f, 0x22); // ?
        EPD213_CMD(pOBD, SSD1608_GATE_VOLTAGE, 0x17);
        ucLine[0] = 0x41;
        ucLine[1] = 0x00;
        ucLine[2] = 0x32;
        obdWriteCommand(pOBD, SSD1608_SOURCE_VOLTAGE);
        RawWriteData(pOBD, ucLine, 3);
        EPD213_CMD(pOBD, SSD1608_WRITE_VCOM, 0x20); // VCOM
    }
    if (pOBD->iFlags & OBD_FLIPV)
        EPD213_CMD(pOBD, SSD1608_DATA_MODE, 0x1);
    else
        EPD213_CMD(pOBD, SSD1608_DATA_MODE, 0x3);
    obdWriteCommand(pOBD, SSD1608_SET_RAMXPOS); // set RAM X start/end
    ucLine[0] = x/8;
    ucLine[1] = (x+w-1)/8;
    RawWriteData(pOBD, ucLine, 2);
    obdWriteCommand(pOBD, SSD1608_SET_RAMYPOS); // set RAM Y start/end
    if (pOBD->iFlags & OBD_FLIPV) {
        ucLine[0] = (uint8_t)(y+h-1);
        ucLine[1] = (uint8_t)((y+h-1)>>8);
        ucLine[2] = (uint8_t)y;
        ucLine[3] = (uint8_t)(y>>8);
    } else {
        ucLine[0] = (uint8_t)y;
        ucLine[1] = (uint8_t)(y>>8);
        ucLine[2] = (uint8_t)(y+h-1);
        ucLine[3] = (uint8_t)((y+h-1)>>8);
    }
    RawWriteData(pOBD, ucLine, 4);
    EPD213_CMD(pOBD, SSD1608_SET_RAMXCOUNT, 0); // set RAM X pointer start
    obdWriteCommand(pOBD, SSD1608_SET_RAMYCOUNT); // set RAM Y pointer start
    if (pOBD->iFlags & OBD_FLIPV) {
        ucLine[0] = y+h-1;
    } else {
        ucLine[0] = 0x00;
    }
    ucLine[1] = 0x00;
    RawWriteData(pOBD, ucLine, 2);
} /* EPD154_Begin() */
void EPD8951_REG(OBDISP *pOBD, uint16_t addr, uint16_t data)
{
uint8_t ucTemp[4];
    
    EPDWaitBusy(pOBD);
    ucTemp[0] = 0x60; // cmd intro 6000h
    ucTemp[1] = 0x00;
    ucTemp[2] = 0x00; // reg write 0011h
    ucTemp[3] = 0x11;
    RawWriteData(pOBD, ucTemp, 4);
    EPDWaitBusy(pOBD);
    ucTemp[0] = ucTemp[1] = 0; // 0000 data intro
    ucTemp[2] = (uint8_t)(addr >> 8);
    ucTemp[3] = (uint8_t)(addr & 0xff);
    RawWriteData(pOBD, ucTemp, 4);
    ucTemp[2] = (uint8_t)(data >> 8);
    ucTemp[3] = (uint8_t)(data & 0xff);
    RawWriteData(pOBD, ucTemp, 4);

} /* EPD8951_REG() */

void EPD8951_ARGS(OBDISP *pOBD, uint16_t u16CMD, uint16_t *pArgs, int iCount)
{
    uint8_t ucTemp[4];
    EPDWaitBusy(pOBD);
    ucTemp[0] = 0x60;
    ucTemp[1] = 0x00; // 6000 cmd
    ucTemp[2] = (uint8_t)(u16CMD >> 8);
    ucTemp[3] = (uint8_t)(u16CMD & 0xff);
    RawWriteData(pOBD, ucTemp, 4);
    
    for (int i=0; i<iCount; i++) {
        EPDWaitBusy(pOBD);
        ucTemp[0] = 0x00;
        ucTemp[1] = 0x00; // 0000 data
        ucTemp[2] = (uint8_t)(pArgs[i] >> 8);
        ucTemp[3] = (uint8_t)(pArgs[i] & 0xff);
        RawWriteData(pOBD, ucTemp, 4);
    }
} /* EPD8951_ARGS() */

void EPD8951_CMD(OBDISP *pOBD, int iCMD)
{
uint8_t ucTemp[4];
    
    EPDWaitBusy(pOBD);
    ucTemp[0] = 0x60;
    ucTemp[1] = 0x00; // cmd prefix = 6000h
    ucTemp[2] = (uint8_t)(iCMD >> 8);
    ucTemp[3] = (uint8_t)(iCMD & 0xff);
    RawWriteData(pOBD, ucTemp, 4);
} /* EPD8951_CMD() */

void EPD8951_WORD(OBDISP *pOBD, uint16_t u16Word)
{
uint8_t ucTemp[4];
    
    EPDWaitBusy(pOBD);
    ucTemp[0] = 0x00;
    ucTemp[1] = 0x00; // cmd prefix = 6000h
    ucTemp[2] = (uint8_t)(u16Word >> 8);
    ucTemp[3] = (uint8_t)(u16Word & 0xff);
    RawWriteData(pOBD, ucTemp, 4);
} /* EPD8951_WORD() */

//
// Write EPD Image data
//
static void EPDWriteImage(OBDISP *pOBD, uint8_t ucCMD, int x, int y, int w, int h, int bRed)
{
uint8_t ucLine[128]; // send the data one line at a time
int tx, ty;
uint8_t *s, *d, ucSrcMask, ucDstMask, uc;
uint8_t *pBuffer, ucInvert = 0;
    
    pBuffer = pOBD->ucScreen;
    if (bRed) {// use second bit plane
        pBuffer += pOBD->width * ((pOBD->height+7)/8);
        ucInvert = 0xff; // red logic is inverted
    }
    if (ucCMD) {
        obdWriteCommand(pOBD, ucCMD); // start write
    }
  // Convert the bit direction and write the data to the EPD
  ucLine[0] = ucLine[1] = 0; // prefix for IT8951
  if (pOBD->iOrientation == 180) {
      for (ty=y+h-1; ty>=y; ty--) {
     ucSrcMask = 1 << (ty & 7);
     ucDstMask = 0x80;
     uc = 0xff;
     d = ucLine+2;
     s = &pBuffer[(ty>>3) * pOBD->width];
     for (tx=x+w-1; tx>=x; tx--) {
            if (s[tx] & ucSrcMask)
           uc &= ~ucDstMask;
        ucDstMask >>= 1;
        if (ucDstMask == 0) {
           *d++ = (uc ^ ucInvert);
           uc = 0xff;
           ucDstMask = 0x80;
        }
     } // for tx
     *d++ = uc; // store final partial byte
     if (pOBD->type == EPD47_540x960)
         RawWriteData(pOBD, ucLine, 2 + ((w+7)/8));
     else
         RawWriteData(pOBD, ucLine+2, (w+7)/8);
      } // for ty
  } else if (pOBD->iOrientation == 0) {
      for (ty=y; ty<y+h; ty++) {
         ucSrcMask = 1 << (ty & 7);
         ucDstMask = 0x80;
         uc = 0xff;
         d = ucLine+2;
         s = &pBuffer[(ty >> 3) * pOBD->width];
         for (tx=x; tx<x+w; tx++) {
            if (s[tx] & ucSrcMask) // src pixel set
               uc &= ~ucDstMask;
            ucDstMask >>= 1;
            if (ucDstMask == 0) { // completed byte
               *d++ = (uc ^ ucInvert);
               uc = 0xff;
               ucDstMask = 0x80;
            }
         } // for tx
        *d++ = (uc ^ ucInvert); // store final partial byte
        if (pOBD->type == EPD47_540x960)
            RawWriteData(pOBD, ucLine, 2+((w+7)/8));
        else
            RawWriteData(pOBD, ucLine+2, (w+7)/8);
      } // for ty
  } else if (pOBD->iOrientation == 90) {
      for (tx=x; tx<x+w; tx++) {
         d = ucLine+2;
     // need to pick up and reassemble every pixel
     ucDstMask = 0x80;
     uc = 0xff;
     ucSrcMask = 1 << ((y+h-1) & 7);
     for (ty=y+h-1; ty>=y; ty--) {
            s = &pBuffer[tx + (ty>>3) * pOBD->width];
        if (s[0] & ucSrcMask) uc &= ~ucDstMask;
        ucDstMask >>= 1;
        ucSrcMask >>= 1;
            if (ucDstMask == 0) {
               *d++ = (uc ^ ucInvert);
           ucDstMask = 0x80;
           uc = 0xff;
        }
        if (ucSrcMask == 0)
           ucSrcMask = 0x80; // bottom up
     } // for ty
      *d++ = (uc ^ ucInvert); // store final partial byte
          if (pOBD->type == EPD47_540x960)
              RawWriteData(pOBD, ucLine, 2+((h+7)/8));
         else
             RawWriteData(pOBD, ucLine+2, (h+7)/8);
      } // for tx
  } else if (pOBD->iOrientation == 270) {
      for (tx=x+w-1; tx>=x; tx--) {
      d = ucLine+2;
      // reassemble every pixel
      ucDstMask = 0x80;
      uc = 0xff;
      ucSrcMask = 1 << (y & 7);
      for (ty=y; ty<y+h; ty++) {
          s = &pBuffer[tx + (ty>>3) * pOBD->width];
          if (s[0] & ucSrcMask) uc &= ~ucDstMask;
          ucDstMask >>= 1;
          ucSrcMask <<= 1;
          if (ucDstMask == 0) {
         *d++ = (uc ^ ucInvert);
         ucDstMask = 0x80;
         uc = 0xff;
          }
          if (ucSrcMask == 0)
         ucSrcMask = 1;
      } // for ty
      *d++ = (uc ^ ucInvert); // store final partial byte
      if (pOBD->type == EPD47_540x960)
          RawWriteData(pOBD, ucLine, 2+((h+7)/8));
      else
          RawWriteData(pOBD, ucLine+2, (h+7)/8);
      } // for x
  } // 270
} /* EPDWriteImage() */

const uint8_t epd293_init_sequence_full[] =
{
//    0x01, SSD1608_SW_RESET,
//    BUSY_WAIT,
    0x02, 0x74, 0x54,
    0x02, 0x7e, 0x3b,
    0x03, 0x2b, 0x04, 0x63,
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f,
    0x04, 0x01, 0x27, 0x01, 0x00,
    0x02, 0x11, 0x03,
    0x03, 0x44, 0x00, 0x0f,
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01,
    0x02, 0x3c, 0xc0,
    0x02, 0x18, 0x80,
    0x02, 0x22, 0xb1,
    0x01, 0x20,
    BUSY_WAIT,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    0x00 // end of table
}; /* epd293_init_sequence_full[] */

const uint8_t epd293_init_sequence_part[] = 
{
//    0x01, SSD1608_SW_RESET,
//    BUSY_WAIT,
    0x02, 0x74, 0x54,
    0x02, 0x7e, 0x3b,
    0x03, 0x2b, 0x04, 0x63,
    0x05, 0x0c, 0x8f, 0x8f, 0x8f, 0x3f,
    0x04, 0x01, 0x27, 0x01, 0x00,
    0x02, 0x11, 0x03,
    0x03, 0x44, 0x00, 0x0f,
    0x05, 0x45, 0x00, 0x00, 0x27, 0x01,
    0x02, 0x3c, 0x01,
    0x02, 0x18, 0x80,
    0x02, 0x22, 0xb1,
    0x01, 0x20,
    BUSY_WAIT,
    0x01, 0x1b,
    0x02, 0x0d, 0xff,
    0x02, 0x4e, 0x00,
    0x03, 0x4f, 0x00, 0x00,
    0x00 // end of table
}; /* epd293_init_sequence_full[] */

const uint8_t epd293_lut[] =
{
    0x22,  0x11, 0x10, 0x00, 0x10, 0x00, 0x00, 0x11, 0x88, 0x80, 0x80, 0x80, 0x00, 0x00, 0x6A, 0x9B,
    0x9B, 0x9B, 0x9B, 0x00, 0x00, 0x6A, 0x9B, 0x9B, 0x9B, 0x9B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x18, 0x04, 0x16, 0x01, 0x0A, 0x0A, 0x0A, 0x0A, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x08, 0x3C, 0x07, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void EPD293_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial)
{
    uint8_t ucLine[128];
    
    if (bPartial) {
        EPDSendCMDSequence(pOBD, epd293_init_sequence_part);
        // send LUT
        obdWriteCommand(pOBD, 0x32);
        memcpy_P(ucLine, epd293_lut_partial, sizeof(epd293_lut_partial));
        RawWriteData(pOBD, ucLine, sizeof(epd293_lut_partial));
    } else {
        EPDSendCMDSequence(pOBD, epd293_init_sequence_full);
    }
} /* EPD293_Begin() */

void EPD29R_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial)
{
    if (bPartial) {
    } else {
        if (pOBD->native_width == 152) {
            EPDSendCMDSequence(pOBD, epd29r_init_sequence_152);
        } else if (pOBD->native_width == 400) {
            EPDSendCMDSequence(pOBD, epd42r_init_sequence);
        } else {
            EPDSendCMDSequence(pOBD, epd29r_init_sequence);
        }
    }
} /* EPD29R_Begin() */

void EPD47_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial)
{
    EPD8951_CMD(pOBD, 0x0001); // IT8951_TCON_SYS_RUN
    EPD8951_REG(pOBD, 0x0004, 0x0001); // enable pack write
    // set vcom to -2.3v
    EPD8951_CMD(pOBD, 0x0039);
    EPD8951_WORD(pOBD, 0x0001);
    EPD8951_WORD(pOBD, 2300);
    if (bPartial) {
        
    } else {
        uint16_t u16Args[5];
        // set target memory area
        EPD8951_REG(pOBD, 0x000a, 0x0012); // _dev_memaddr_h
        EPD8951_REG(pOBD, 0x0008, 0x36e0); // _dev_memaddr_l
        // set image area
        u16Args[0] = 0; // little endian, 2bpp, no rotation
        u16Args[1] = 0; // start x
        u16Args[2] = 0; // start y
        u16Args[3] = pOBD->native_width;
        u16Args[4] = pOBD->native_height;
        EPD8951_ARGS(pOBD, 0x0021, u16Args, 5); // IT8951_TCON_LD_IMG_AREA
        EPD8951_CMD(pOBD, 0x20); // IT8951_TCON_LD_IMG
        EPD8951_WORD(pOBD, 0); // default settings (endian, format, rotation)
    }
} /* EPD47_Begin() */

void EPD102_Begin(OBDISP *pOBD, int x, int y, int w, int h, int bPartial)
{
    uint8_t ucLine[128];
        
        if (bPartial) {
            EPDSendCMDSequence(pOBD, epd29_init_sequence_partial);
            obdWriteCommand(pOBD, UC8151_PON); // power on
            obdWriteCommand(pOBD, UC8151_PTIN); // partial mode
            obdWriteCommand(pOBD, UC8151_PTL); // partial window
            ucLine[0] = 0; //y; X START
            ucLine[1] = 127; //(y + h - 1); X END
            ucLine[2] = 0; //(x >> 8); // Y START
            ucLine[3] = 0; //x & 0xff;
            ucLine[4] = 1; //(x + w - 1) >> 8; Y END >> 8
            ucLine[5] = 39; //(x + w - 1) & 0xff;
            ucLine[6] = 0x28; // ?
            RawWriteData(pOBD, ucLine, 7); // boundaries
        } else {
            EPDSendCMDSequence(pOBD, epd102_init_sequence_full);
            return;
            // send LUTs
            obdWriteCommand(pOBD, UC8151_LUT_WB);
            memcpy_P(ucLine, epd102_lut_w_full, sizeof(epd102_lut_w_full));
            RawWriteData(pOBD, ucLine, sizeof(epd102_lut_w_full));
            obdWriteCommand(pOBD, UC8151_LUT_BB);
            memcpy_P(ucLine, epd102_lut_b_full, sizeof(epd102_lut_b_full));
            RawWriteData(pOBD, ucLine, sizeof(epd102_lut_b_full));
      //      obdWriteCommand(pOBD, UC8151_PTOU); // disable partial mode
            obdWriteCommand(pOBD, UC8151_PON); // power on
            EPDWaitBusy(pOBD);
        }
} /* EPD102_Begin() */
//
// Special case for e-ink displays
//
static void EPDDumpBuffer(OBDISP *pOBD, uint8_t *pBuffer, int bRefresh)
{
    
    pOBD->iFlags |= OBD_FULLUPDATE; // mark it for later
  EPDWakeUp(pOBD);
    if (pOBD->type == EPD42R2_400x300) {
        EPDSendCMDSequence(pOBD, epd42r2_init_sequence_full);
        EPDWriteImage(pOBD, 0x10, 0, 0, pOBD->width, pOBD->height, 0); // black/white plane
        EPDWriteImage(pOBD, 0x13, 0, 0, pOBD->width, pOBD->height, 1); // red plane
        if (bRefresh) {
            obdWriteCommand(pOBD, 0x12); // master activation
        }
    }
    if (pOBD->type == EPD47_540x960) {
        uint16_t u16Args[7];
        EPD47_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, false);
        EPDWriteImage(pOBD, 0, 0, 0, pOBD->width, pOBD->height, 0);
        EPD8951_CMD(pOBD, 0x0022); // LCD_IMG_END
        u16Args[0] = 0; // x
        u16Args[1] = 0; // y
        u16Args[2] = pOBD->native_width;
        u16Args[3] = pOBD->native_height;
        u16Args[4] = 0; // full update mode (2 seconds)
        u16Args[5] = 0x36e0; // _dev_memaddr_l
        u16Args[6] = 0x0012; // _dev_memaddr_h
        EPD8951_ARGS(pOBD, 0x0037, u16Args, 7); // IT8951_I80_CMD_DPY_BUF_AREA
    }
    if (pOBD->type == EPD293_128x296) {
        uint8_t ucTemp[4];
        EPD293_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, false);
        EPDWriteImage(pOBD, 0x26, 0, 0, pOBD->width, pOBD->height, 0);
        EPD213_CMD(pOBD, 0x4e, 0x00);
        ucTemp[0] = ucTemp[1] = 0;
        obdWriteCommand(pOBD, 0x4f);
        RawWriteData(pOBD, ucTemp, 2);
        EPDWriteImage(pOBD, 0x24, 0, 0, pOBD->width, pOBD->height, 0);
        if (bRefresh) {
            EPD213_CMD(pOBD, 0x22, 0xc7);
            obdWriteCommand(pOBD, 0x20); // refresh
        }
    }
    if (pOBD->type == EPD102_80x128) {
        EPD102_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, false);
        EPDWriteImage(pOBD, 0x10, 0, 0, pOBD->width, pOBD->height, 0);
        EPDWriteImage(pOBD, 0x13, 0, 0, pOBD->width, pOBD->height, 0); // send image to current and previous buffers
        obdWriteCommand(pOBD, 0x12); // refresh
        _delay(10); // needed
    }
  if (pOBD->type == EPD29_128x296) {
      EPD29_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, false);
      EPDWriteImage(pOBD, UC8151_DTM2, 0, 0, pOBD->width, pOBD->height, 0); // send buffer
      if (bRefresh) {
          EPD29_Finish(pOBD, false);
      }
  }
    if (pOBD->type == EPD27_176x264) {
        EPD27_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, false);
        obdWriteCommand(pOBD, UC8151_DTM1);
        for (int i=0; i<5808; i++) { // all FF
            uint8_t uc = 0xff;
            RawWriteData(pOBD, &uc, 1);
        }
//        EPDWriteImage(pOBD, UC8151_DTM1, 0, 0, pOBD->width, pOBD->height, 0);
        EPDWriteImage(pOBD, UC8151_DTM2, 0, 0, pOBD->width, pOBD->height, 0);
        if (bRefresh) {
            obdWriteCommand(pOBD, UC8151_DRF);
        }
    }
    if (pOBD->type == EPD42_400x300) {
        EPD42_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, false);
        EPDWriteImage(pOBD, UC8151_DTM1, 0, 0, pOBD->width, pOBD->height, 0);
        EPDWriteImage(pOBD, UC8151_DTM2, 0, 0, pOBD->width, pOBD->height, 0);
        if (bRefresh) {
            obdWriteCommand(pOBD, UC8151_DRF);
        }
    }
    if (pOBD->type == EPD29R_128x296) { // BLACK/WHITE/RED
        uint8_t ucTemp[4];
        EPD29R_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, false);
        EPDWriteImage(pOBD, 0x26, 0, 0, pOBD->width, pOBD->height, 1); // red plane
        EPD213_CMD(pOBD, 0x4e, 0x00); // RAM X counter
        obdWriteCommand(pOBD, 0x4f);
        ucTemp[0] = (uint8_t)(pOBD->native_height-1);
        ucTemp[1] = (uint8_t)((pOBD->native_height-1)>>8);
        RawWriteData(pOBD, ucTemp, 2);
        EPDWriteImage(pOBD, 0x24, 0, 0, pOBD->width, pOBD->height, 0); // black/white plane
        if (bRefresh) {
            EPD213_CMD(pOBD, 0x22, 0xc7); // update control
            obdWriteCommand(pOBD, 0x20); // master activation
        }
    }
    if (pOBD->type == EPD213_104x212 || pOBD->type == EPD213_122x250) {
        EPD213_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, false);
        EPDWriteImage(pOBD, SSD1608_WRITE_RAM, 0, 0, pOBD->width, pOBD->height, 0);
        EPDWriteImage(pOBD, SSD1608_WRITE_ALTRAM, 0, 0, pOBD->width, pOBD->height, 0); // send image to current and previous buffers
        if (bRefresh) {
            EPD154_Finish(pOBD, false);
        }
    } else if (pOBD->type == EPD154_200x200 || pOBD->type == EPD154_152x152) {
        EPD154_Begin(pOBD, 0, 0, pOBD->width, pOBD->height, false); // x, y, w, h
        EPDWriteImage(pOBD, SSD1608_WRITE_RAM, 0, 0, pOBD->width, pOBD->height, 0);
        EPDWriteImage(pOBD, SSD1608_WRITE_ALTRAM, 0, 0, pOBD->width, pOBD->height, 0); // send image to current and previous buffers
        if (bRefresh) {
            EPD154_Finish(pOBD, false);
        }
    }
  EPDWaitBusy(pOBD);
  EPDSleep(pOBD);
} /* EPDDumpBuffer() */
//
// Special case for ST7302
//
void obdST7302SetPos(OBDISP *pOBD, int x, int y)
{
uint8_t ucTemp[4];
    
    obdWriteCommand(pOBD, 0x2a); // Column set
    ucTemp[0] = 0x19 + (y/12); // start x (we treat it as y)
    ucTemp[1] = 0x27; // end x
    RawWriteData(pOBD, ucTemp, 2);
    obdWriteCommand(pOBD, 0x2b); // Row set
    ucTemp[0] = (x/2); // start y (we treat it as x)
    ucTemp[1] = 0x80; // end y
    RawWriteData(pOBD, ucTemp, 2);
    obdWriteCommand(pOBD, 0x2c); // memory write
} /* obdST7302SetPos() */
//
// Special case for ST7302
//
static void ST7302DumpBuffer(OBDISP *pOBD, uint8_t *pBuffer)
{
uint8_t ucPixels[40];
int x, y, iPitch, count;
uint8_t ucMask, uc1, *s, *d;
    
    iPitch = pOBD->width;
// Shift out the image in pairs of lines
    ucPixels[0] = 0x40;
    switch (pOBD->iOrientation) {
        case 0:
        case 180:
            for (x = 0; x < pOBD->width; x += 2) { // a pair of columns at a time
                d = ucPixels+1;
                ucMask = 1;
                uc1 = 0;
                count = 0;
                if (pOBD->iOrientation == 180) {
                    ucMask = 1<<((pOBD->height-1) & 7);
                    s = &pBuffer[x + ((pOBD->height-1)>>3)*iPitch];
                    for (y = pOBD->height-1; y >= 0; y--) {
                        uc1 <<= 2;
                        if (s[0] & ucMask) uc1 |= 1;
                        if (s[1] & ucMask) uc1 |= 2;
                        count++;
                        if (count == 4) { // finish the byte
                            *d++ = uc1;
                            count = 0;
                            uc1 = 0;
                        }
                        ucMask >>= 1;
                        if (ucMask == 0) {
                            ucMask = 0x80;
                            s -= iPitch;
                        } // next line
                    } // for y
                    if (count) {
                        uc1 <<= (count*2);
                        *d++ = uc1;
                    }
                } else {
                    s = &pBuffer[x];
                    uc1 = 0;
                    for (y=0; y < pOBD->height+3; y++) {
                        uc1 <<= 2;
                        if (s[0] & ucMask) uc1 |= 2;
                        if (s[1] & ucMask) uc1 |= 1;
                        count++;
                        if (count == 4) { // finish the byte
                            *d++ = uc1;
                            count = 0;
                            uc1 = 0;
                        }
                        ucMask <<= 1;
                        if (ucMask == 0) {
                            ucMask = 1;
                            s += iPitch;
                        } // next line
                    } // for y
                } // flipped
                obdSetPosition(pOBD, (pOBD->flip) ? 248-x : x, 0, 1);
                RawWrite(pOBD, ucPixels, 3 + (int)(d - ucPixels));
            } // for x
            break;
        case 90:
        case 270: // x/y swapped
            if (pOBD->iOrientation == 270) {
                for (y=pOBD->height-2; y >= 0; y-=2) {
                    ucMask = 1<<(y & 7);
                    s = &pBuffer[(y>>3)*pOBD->width];
                    count = 0;
                    d = ucPixels+1;
                    uc1 = 0;
                    for (x=0; x<pOBD->width; x++) {
                        uc1 <<= 2;
                        if (s[x] & ucMask) uc1 |= 1;
                        if (s[x] & (ucMask << 1)) uc1 |= 2;
                        count++;
                        if (count == 4) { // finish the byte
                            *d++ = uc1;
                            count = 0;
                        }
                    } // for x
                    obdSetPosition(pOBD, pOBD->height - 2 - y, 0, 1);
                    RawWrite(pOBD, ucPixels, 3 + (int)(d - ucPixels));
                } // for y
            } else { // 90
                for (y=0; y < pOBD->height; y+=2) {
                    ucMask = 1<<(y & 7);
                    s = &pBuffer[(y>>3)*pOBD->width];
                    count = 0;
                    d = ucPixels+1;
                    uc1 = 0;
                    for (x=pOBD->width-1; x>=0; x--) {
                        uc1 <<= 2;
                        if (s[x] & ucMask) uc1 |= 2;
                        if (s[x] & (ucMask << 1)) uc1 |= 1;
                        count++;
                        if (count == 4) { // finish the byte
                            *d++ = uc1;
                            count = 0;
                        }
                    } // for x
                    obdSetPosition(pOBD, y, 0, 1);
                    RawWrite(pOBD, ucPixels, 3 + (int)(d - ucPixels));
                } // for y
            } // flipped
            break;
    } // switch
} /* ST7302DumpBuffer() */
//
// Special case for Sharp Memory LCD
//
static void SharpDumpBuffer(OBDISP *pOBD, uint8_t *pBuffer)
{
int x, y;
uint8_t c, ucInvert, *s, *d, ucStart;
uint8_t ucLineBuf[56];
int iPitch = pOBD->width / 8;
static uint8_t ucVCOM = 0;
int iBit;
uint8_t ucMask;

  ucInvert = (pOBD->invert) ? 0x00 : 0xff;
  digitalWrite(pOBD->iCSPin, HIGH); // active high
 
    ucLineBuf[0] = 0;
  ucStart = 0x80; // write command
  if (ucVCOM)
    ucStart |= 0x40; // VCOM bit
  ucLineBuf[1] = ucStart;
  // this code assumes I2C, so the first byte is ignored
  RawWrite(pOBD, ucLineBuf, 2); // write command(01) + vcom(02)
  ucVCOM = !ucVCOM; // need to toggle this each transaction

 // We need to flip and invert the image in code because the Sharp memory LCD
 // controller only has the simplest of commands for data writing
  if (pOBD->flip)
  {
     for (y=0; y<pOBD->height; y++) // we have to write the memory in the wrong direction
     {
         ucMask = 0x80 >> (y & 7);
        s = &pBuffer[pOBD->width - 1 + (pOBD->width * ((pOBD->height - 1 - y) >> 3))]; // point to last line first
        d = &ucLineBuf[2];
        ucLineBuf[1] = pgm_read_byte(&ucMirror[y+1]); // current line number
        for (x=0; x<pOBD->width/8; x++)
        {
           c = ucInvert; // we need to brute-force invert it
            for (iBit=7; iBit>=0; iBit--)
            {
                if (s[0] & ucMask)
                    c ^= (1 << iBit);
                s--;
            }
           *d++ = c;
        } // for y
        // write this line to the display
        ucLineBuf[iPitch+2] = 0; // end of line
        RawWrite(pOBD, ucLineBuf, iPitch+3);
     } // for x
  }
  else // normal orientation
  {
     for (y=0; y<pOBD->height; y++) // we have to write the memory in the wrong direction
     {
        ucMask = 1 << (y & 7);
        s = &pBuffer[pOBD->width * (y >> 3)]; // point to last line first
        d = &ucLineBuf[2];
        
        ucLineBuf[1] = pgm_read_byte(&ucMirror[y+1]); // current line number
        for (x=0; x<pOBD->width/8; x++)
        {
            c = ucInvert;
            for (iBit=7; iBit>=0; iBit--)
            {
                if (s[0] & ucMask)
                    c ^= (1 << iBit);
                s++;
            }
           *d++ = c;
        } // for y
        // write this line to the display
        ucLineBuf[iPitch+2] = 0; // end of line
        RawWrite(pOBD, ucLineBuf, iPitch+3);
     } // for x
  }
  ucLineBuf[1] = 0;
  RawWrite(pOBD, ucLineBuf, 2); // final transfer
  digitalWrite(pOBD->iCSPin, LOW); // de-activate
} /* SharpDumpBuffer() */
//
// Dump a screen faster to an e-ink display
//
void obdDumpFast(OBDISP *pOBD, int startx, int starty, int width, int height)
{
   if (pOBD->type >= EPD42_400x300) // it's an e-ink panel
      EPDDumpFast(pOBD, NULL, startx, starty, width, height);
   else // use the default full screen dump function
      obdDumpBuffer(pOBD, NULL);
    pOBD->iFlags &= ~OBD_FULLUPDATE; // mark it for later
} /* obdDumpFast() */
//
// Dump a screen's worth of data directly to the display
// Try to speed it up by comparing the new bytes with the existing buffer
//
void obdDumpBuffer_2(OBDISP *pOBD, uint8_t *pBuffer, int bRefresh)
{
int x, y;
int iLines;
    
    if (pOBD->type == LCD_VIRTUAL) { // wrong function for this type of display
        return;
    }
    if (pBuffer == NULL) { // dump the internal buffer if none is given
        pBuffer = pOBD->ucScreen;
    }
    if (pBuffer == NULL) {
        return; // no backbuffer and no provided buffer
    }
    
  if (pOBD->type >= EPD42_400x300) // all e-ink panels
  {
     EPDDumpBuffer(pOBD, pBuffer, bRefresh);
     return;
  }
  if (pOBD->type == LCD_ST7302) // special case for ST7302
  {
      ST7302DumpBuffer(pOBD, pBuffer);
      return;
  }
  if (pOBD->type >= SHARP_144x168) // special case for Sharp Memory LCDs
  {
    SharpDumpBuffer(pOBD, pBuffer);
    return;
  }
  iLines = (pOBD->native_height+7) >> 3;
    // 0/180 we can send the 8 lines of pixels straight through
    if (pOBD->iOrientation == 0 || pOBD->iOrientation == 180) {
        if (pOBD->iOrientation == 0 || (pOBD->iOrientation == 180 && pOBD->can_flip)) {
            for (y=0; y<iLines; y++) {
                obdSetPosition(pOBD, 0, y*8, 1);
                obdWriteDataBlock(pOBD, pBuffer, pOBD->width, 1);
                pBuffer += pOBD->width;
            }
        } else { // have to manually do 180 flipped
            for (y=iLines-1; y>=0; y--) {
                obdSetPosition(pOBD, 0, y*8, 1);
                for (int x=0; x<pOBD->width; x++) {
                    u8Cache[pOBD->width-1-x] = ucMirror[pBuffer[x]];
                }
                obdWriteDataBlock(pOBD, u8Cache, pOBD->width, 1);
                pBuffer += pOBD->width;
            }
        }
     return;
  } else { // must be 90/270
      // Capture the pixels 'sideways' and send a line at a time
     if (pOBD->iOrientation == 90 || (pOBD->iOrientation == 270 && pOBD->can_flip)) {
          for (x=0; x<pOBD->width; x+=8) {
              uint8_t *s, *d, uc, ucMask, ucTemp[132];
              int j;
              d = ucTemp;
              *d++ = 0x40;
              s = &pBuffer[x + ((pOBD->height-7)>>3)*pOBD->width];
              ucMask = 0x80;
              for (y=0; y<pOBD->height; y++) {
                  uc = 0;
                  for (j=7; j>=0; j--) {
                      uc <<= 1;
                      if (s[j] & ucMask) uc |= 1;
                  } // for j
                  *d++ = uc;
                  ucMask >>= 1;
                  if (ucMask == 0) {
                      ucMask = 0x80;
                      s -= pOBD->width;
                  }
              } // for y
              obdSetPosition(pOBD, 0, x, 1);
              RawWrite(pOBD, ucTemp, pOBD->height + 1);
          } // for x
      } else { // 270 degrees and can't flip 180
          for (x=pOBD->width-8; x>=0; x-=8) {
              uint8_t j, *s, *d, uc, ucMask, ucTemp[132];
              d = ucTemp;
              *d++ = 0x40;
              s = &pBuffer[x];
              ucMask = 1;
              for (y=0; y<pOBD->height; y++) {
                  uc = 0;
                  for (j=0; j<8; j++) {
                      uc <<= 1;
                      if (s[j] & ucMask) uc |= 1;
                  } // for j
                  *d++ = uc;
                  ucMask <<= 1;
                  if (ucMask == 0) {
                      ucMask = 1;
                      s += pOBD->width;
                  }
              } // for y
              obdSetPosition(pOBD, 0, pOBD->width-8-x, 1);
              RawWrite(pOBD, ucTemp, pOBD->height + 1);
          } // for x
      }
  } // 90/270 degrees rotated
} /* obdDumpBuffer_2() */

void obdDumpBuffer(OBDISP *pOBD, uint8_t *pBuffer)
{
    obdDumpBuffer_2(pOBD, pBuffer, true);
}

//
// Menu functions are not (yet) supported on Linux
//
#ifndef _LINUX_
// A valid CW or CCW move returns 1 or -1, invalid returns 0.
static int obdMenuReadRotary(SIMPLEMENU *sm) {
static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};
uint8_t c;
int rc = 0;


  sm->prevNextCode <<= 2;
  if (digitalRead(sm->u8Dn) == sm->iPressed)
     sm->prevNextCode |= 0x02;
  if (digitalRead(sm->u8Up) == sm->iPressed)
     sm->prevNextCode |= 0x01;
  sm->prevNextCode &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[sm->prevNextCode]) {
      sm->store <<= 4;
      sm->store |= sm->prevNextCode;
      c = sm->store & 0xff;
      //if (store==0xd42b) return 1;
      //if (store==0xe817) return -1;
      if ((c & 0xf) == 2) rc = -1;
      else if ((c & 0xf) == 1) rc = 1;
   }
//   Serial.printf("store = 0x%04x, val = %d\n", sm->store, rc);
   return rc;
} /* obdMenuReadRotary() */

//
// Initialize the simple menu structure
//
int obdMenuInit(OBDISP *pOBD, SIMPLEMENU *sm, char **pText, int iFontSize, int bCenter, int btnUp, int btnDn, int btnEnter, int iPressedState, int bIsRotary)
{
  int iLen;
  if (sm == NULL || pText == NULL) return 0;
  sm->pOBD = pOBD;
  sm->u8Up = btnUp; // pin numbers of the action buttons
  sm->u8Dn = btnDn; // or rotary A line
  sm->u8Enter = btnEnter; // or rotary B line
  sm->bIsRotary = bIsRotary;
  sm->u8BtnState = 0; // no active buttons to start
  sm->iPressed = iPressedState; // active state of a pressed button
  sm->bCenter = bCenter;
  sm->iFontSize = iFontSize;
  sm->pMenuText = pText;
  sm->iMenuIndex = 0; // start at first item
  sm->iDispX = 128; // DEBUG
  sm->iDispY = 64; // DEBUG
  sm->bOneButton = (btnDn == -1 && btnEnter == -1); // only 1 button defined
  sm->pfnCallback = NULL;
  sm->prevNextCode = 0;
  sm->store = 0;
  iLen = 0;
  while (pText[iLen] != NULL) {
    iLen++;
  }
  sm->iMenuLen = iLen-1; // don't count the title text
  return 1; // success
} /* obdMenuInit() */

//
// Get the text and centering position for
// a specific menu item
// returns the X position
//
static int obdMenuGetItem(SIMPLEMENU *sm, int iItem, char *szText)
{
  int x, cx, len;

  if (iItem > sm->iMenuLen)
     return -1; // invalid request

  if (sm->iFontSize == FONT_6x8)
     cx = 6;
  else if (sm->iFontSize == FONT_8x8)
     cx = 8;
  else cx = 16;
  strcpy(szText, sm->pMenuText[iItem]);
  if (sm->pfnCallback && iItem > 0) // don't add callback for menu title
  {
    strcat(szText, " ");
    strcat(szText, (*sm->pfnCallback)(iItem-1));
  }
  x = 0;
  if (sm->bCenter || iItem == 0) // always center the menu title
  {
    len = strlen(szText);
     x = (sm->iDispX - (len * cx)) / 2;
  }
  return x;
} /* obdMenuGetItem() */

//
// Erase the display and show the given menu
//
void obdMenuShow(SIMPLEMENU *sm, int iItem)
{
int i, x, iCount, iStart = 0;
int iFirst, iLast;
char szTemp[64];

  iCount = (sm->iDispY / 8) - 1; // DEBUG - number of visible menu lines
  iFirst = iLast = iItem;
  if (iItem == -1) // show the entire menu
  {
    obdFill(sm->pOBD, 0, 0);
    x = obdMenuGetItem(sm, 0, szTemp); // get the title text
    obdMenuShowItem(sm->pOBD, x, 0, szTemp, 0, 0, sm->iFontSize, 0); // show title
    iFirst = 0;
    iLast = iCount-1;
  }
  if (sm->iMenuIndex >= iCount) // needs to scroll up
     iStart = sm->iMenuIndex - (iCount-1);
  if (sm->iMenuIndex < 0 || sm->iMenuIndex+iCount > sm->iMenuLen) { // invalid
     sm->iMenuIndex = 0;
     iStart = 0;
  }

  for (i=iFirst; i<=iLast && i+iStart < sm->iMenuLen; i++) // draw the visible menu lines
  {
    x = obdMenuGetItem(sm, i + iStart + 1, szTemp);
    if (x >= 0) // display if valid
       obdMenuShowItem(sm->pOBD, x, i+1, szTemp, (i+iStart == sm->iMenuIndex), (iFirst==iLast), sm->iFontSize, (iFirst==iLast));
  }
  if (iItem == -1) // now the display it in one shot
    obdDumpBuffer(sm->pOBD, NULL);
} /* obdMenuShow() */

//
// Set a callback function to return custom info/status
// for each menu item
//
void obdMenuSetCallback(SIMPLEMENU *sm, SIMPLECALLBACK pfnCallBack)
{
  if (sm != NULL)
    sm->pfnCallback = pfnCallBack;
} /* obdMenuSetCallback() */
//
// Display the text of a single menu item
// optionally erases what's under it to prevent left-over text when the length changes
//
void obdMenuShowItem(OBDISP *pOBD, int x, int y, char *szText, int bInvert, int bErase, int iFontSize, int bRender)
{
  static char *szBlank = (char *)"                      ";
  if (bErase)
    obdWriteString(pOBD, 0, 0, y, szBlank, iFontSize, 0, bRender); // erase old info
  obdWriteString(pOBD, 0, x, y, szText, iFontSize, bInvert, bRender);
} /* obdMenuShowItem() */

//
// Flash a menu item when it is selected
//
static void obdMenuFlash(SIMPLEMENU *sm, int iItem)
{
int x, y, i, iCount;
char szTemp[64];

  iCount = (sm->iDispY / 8) - 1; // DEBUG - number of visible menu lines
  y = iItem+1;
  if (y > iCount) // max bottom line
     y = iCount;

  x = obdMenuGetItem(sm, iItem+1, szTemp);
  if (x < 0) return; // invalid request

  for (i=0; i<3; i++)
  {
    obdMenuShowItem(sm->pOBD, x, y, szTemp, 0, 0, sm->iFontSize, 1); // show non-inverted
    _delay(200);
    obdMenuShowItem(sm->pOBD, x, y, szTemp, 1, 0, sm->iFontSize, 1); // show inverted
    _delay(200);
  }
} /* obdMenuFlash() */

//
// Change the menu index incrementally
// redraws the minimum amount of screen to show the new info
// (this prevents flicker/flash and saves battery life)
// returns the new menu index
//
int obdMenuDelta(SIMPLEMENU *sm, int iDelta)
{
  int i, x, iNewIndex, iCount;
  int iStart1, iStart2;
  char szTemp[64];

  if (iDelta == 0) return sm->iMenuIndex; // nothing to do
  
  iNewIndex = sm->iMenuIndex + iDelta;
  if (!sm->bOneButton && (iNewIndex < 0 || iNewIndex >= sm->iMenuLen)) // no change possible, exit
     return sm->iMenuIndex; // unchanged
  // If we are using a single button, wrap around the ends
  if (iNewIndex < 0) iNewIndex = (sm->iMenuLen - 1);
  else if (iNewIndex > sm->iMenuLen-1) iNewIndex = 0;
     
  iCount = (sm->iDispY / 8) - 1; // DEBUG - number of visible menu lines
  iStart1 = iStart2 = 0;
  if (sm->iMenuIndex > iCount-1)
     iStart1 = sm->iMenuIndex - (iCount-1);
  if (iNewIndex > iCount-1) // needs to scroll up
     iStart2 = iNewIndex - (iCount-1);
  if (iStart1 != iStart2) // need to redraw all items
  {
    for (i=0; i<iCount; i++)
    {
      x = obdMenuGetItem(sm, i+iStart2+1, szTemp);
      if (x >= 0)
         obdMenuShowItem(sm->pOBD, x, i+1, szTemp, (i+iStart2 == iNewIndex), 1, sm->iFontSize, 0);
    }
    obdDumpBuffer(sm->pOBD, NULL);
  }
  else // need to redraw only the new and old items
  {
      i = sm->iMenuIndex - iStart1;
      x = obdMenuGetItem(sm, sm->iMenuIndex+1, szTemp);
      if (x >= 0)
         obdMenuShowItem(sm->pOBD, x, i+1, szTemp, 0, 0, sm->iFontSize, 1);
      i = iNewIndex - iStart2;
      x = obdMenuGetItem(sm, iNewIndex+1, szTemp);
      if (x >= 0)
         obdMenuShowItem(sm->pOBD, x, i+1, szTemp, 1, 0, sm->iFontSize, 1);
  }
  sm->iMenuIndex = iNewIndex;
  return iNewIndex;
} /* obdMenuDelta() */

//
// With the given setup, check for button presses
// and act accordingly
// returns -1 for normal interactions and the menu item index if the user presses the ENTER button
//
// time in milliseconds for a long press
#define MENU_LONG_PRESS 600
int obdMenuRun(SIMPLEMENU *sm)
{
uint8_t buttons = 0;
unsigned long ul;
int iDelta, rc = -1;

  if (sm->bIsRotary) { // read the rotary encoder
    if (digitalRead(sm->u8Enter) == sm->iPressed) {
       buttons |= 1; // pressed
       if (buttons != sm->u8BtnState)
          rc = sm->iMenuIndex; // user pressed ENTER, return current menu index
    } else { // check for rotary encoder activity
      iDelta = obdMenuReadRotary(sm);
      obdMenuDelta(sm, iDelta);
    }
    sm->u8BtnState = buttons;
  } else {
// check the button states
    if (digitalRead(sm->u8Up) == sm->iPressed)
       buttons |= 1;
    if (buttons != sm->u8BtnState) // something changed
    {
      if (sm->bOneButton) // different logic for a single button system
      {
        if (sm->u8BtnState == 0 && buttons == 1) // up button just pressed
        {
           sm->ulPressTime = millis(); // record the press time
        }
        if (sm->u8BtnState == 1 && buttons == 0) // up button just released
        {
          ul = millis() - sm->ulPressTime;
          if (ul < MENU_LONG_PRESS) // short press = navigate menu
            obdMenuDelta(sm, 1);
          else // treat it like a long press
            rc = sm->iMenuIndex; // action
        }
      }
      else // 3 button setup (UP/DOWN/ENTER)
      {
        if (digitalRead(sm->u8Dn) == sm->iPressed)
           buttons |= 2;
        if (digitalRead(sm->u8Enter) == sm->iPressed)
           rc = sm->iMenuIndex; // user pressed ENTER, return current menu index
        if ((sm->u8BtnState & 1) == 0 && (buttons & 1) == 1) // Up button pressed
        {
          obdMenuDelta(sm, -1);
        }
        if ((sm->u8BtnState & 2) == 0 && (buttons & 2) == 2) // Down button pressed
        {
          obdMenuDelta(sm, 1);
        }
      }
      sm->u8BtnState = buttons; // save the latest state
    }
  }
  if (rc != -1) // selected
     obdMenuFlash(sm, sm->iMenuIndex);
  return rc;
} /* obdMenuRun() */
#endif // !_LINUX_
// wrapper/adapter functions to make the code work on Linux
#ifdef _LINUX_
static uint8_t pgm_read_byte(const uint8_t *ptr)
{
  return *ptr;
}
static int16_t pgm_read_word(const uint8_t *ptr)
{
  return ptr[0] + (ptr[1]<<8);
}
#if !defined( MEMORY_ONLY ) // && !defined( __BITBANG_I2C__ )
int I2CReadRegister(BBI2C *pI2C, uint8_t addr, uint8_t reg, uint8_t *pBuf, int iLen)
{
int rc;
  rc = write(pI2C->file_i2c, &reg, 1);
  rc = read(pI2C->file_i2c, pBuf, iLen);
  return (rc > 0);
}
int I2CRead(BBI2C *pI2C, uint8_t addr, uint8_t *pBuf, int iLen)
{
int rc;
  rc = read(pI2C->file_i2c, pBuf, iLen);
  return (rc > 0);
}
void I2CInit(BBI2C *pI2C, uint32_t iSpeed)
{
char filename[32];

  sprintf(filename, "/dev/i2c-%d", pI2C->iSDA); // I2C bus number passed in SDA pin
  if ((pI2C->file_i2c = open(filename, O_RDWR)) < 0)
      return;// 1;
  if (ioctl(pI2C->file_i2c, I2C_SLAVE, pI2C->iSCL) < 0) // set slave address
  {
     close(pI2C->file_i2c);
     pI2C->file_i2c = 0;
      return; // 1;
  }
    return; // 0;
}
#endif // MEMORY_ONLY
// Write raw (unfiltered) bytes directly to I2C or SPI
static void RawWrite(OBDISP *pOBD, unsigned char *pData, int iLen)
{
#ifndef MEMORY_ONLY
  if (pOBD->com_mode == COM_I2C) {// I2C device
      write(pOBD->bbi2c.file_i2c, pData, iLen);
  } else { // must be SPI
      obdSetDCMode(pOBD, (pData[0] == 0) ? MODE_COMMAND : MODE_DATA);
      if (pOBD->iCSPin != 0xff && pOBD->type != SHARP_144x168 && pOBD->type != SHARP_400x240)
         digitalWrite(pOBD->iCSPin, LOW);
      AIOWriteSPI(pOBD->bbi2c.file_i2c, &pData[1], iLen-1);
      if (pOBD->iCSPin != 0xff && pOBD->type != SHARP_144x168 && pOBD->type != SHARP_400x240)
         digitalWrite(pOBD->iCSPin, HIGH);
      //obdSetDCMode(pOBD, MODE_DATA);
  }
#endif // MEMORY_ONLY
} /* RawWrite() */
static void RawWriteData(OBDISP *pOBD, unsigned char *pData, int iLen)
{
#ifndef MEMORY_ONLY
  if (pOBD->com_mode == COM_I2C) {// I2C device
      write(pOBD->bbi2c.file_i2c, pData, iLen);
  } else { // must be SPI
      obdSetDCMode(pOBD, MODE_DATA);
    if (pOBD->iFlags & OBD_CS_EVERY_BYTE) {
       for (int i=0; i<iLen; i++) {
          digitalWrite(pOBD->iCSPin, LOW);
	  AIOWriteSPI(pOBD->bbi2c.file_i2c, &pData[i], 1);
	  digitalWrite(pOBD->iCSPin, HIGH);
       }
    } else {
      if (pOBD->iCSPin != 0xff && pOBD->type != SHARP_144x168 && pOBD->type != SHARP_400x240)
         digitalWrite(pOBD->iCSPin, LOW);
      AIOWriteSPI(pOBD->bbi2c.file_i2c, pData, iLen);
      if (pOBD->iCSPin != 0xff &&pOBD->type != SHARP_144x168 && pOBD->type != SHARP_400x240)
         digitalWrite(pOBD->iCSPin, HIGH);
    }
      //obdSetDCMode(pOBD, MODE_DATA);
  }
#endif // MEMORY_ONLY
} /* RawWriteData() */
#else // Arduino
static void RawWrite(OBDISP *pOBD, unsigned char *pData, int iLen)
{
#if !defined( __AVR_ATtiny85__ )
  if (pOBD->com_mode == COM_SPI) // we're writing to SPI, treat it differently
  {
    if (pOBD->iDCPin != 0xff)
    {
      digitalWrite(pOBD->iDCPin, (pData[0] == 0) ? LOW : HIGH); // data versus command
    }
    if (pOBD->iCSPin != 0xff && pOBD->type != SHARP_144x168 && pOBD->type != SHARP_400x240)
    {
        digitalWrite(pOBD->iCSPin, LOW);
    }
#ifdef HAL_ESP32_HAL_H_
   {
   uint8_t ucTemp[1024];
       if (pOBD->bBitBang)
           SPI_BitBang(pOBD, &pData[1], iLen-1, pOBD->iMOSIPin, pOBD->iCLKPin);
       else
           mySPI->transferBytes(&pData[1], ucTemp, iLen-1);
   }
#else
    for (int i=1; i<iLen; i++) {
        if (pOBD->bBitBang)
            SPI_BitBang(pOBD, &pData[i], 1, pOBD->iMOSIPin, pOBD->iCLKPin);
        else
            mySPI->transfer(pData[i]);
    }
#endif
    if (pOBD->iCSPin != 0xff && pOBD->type != SHARP_144x168 && pOBD->type != SHARP_400x240)
      digitalWrite(pOBD->iCSPin, HIGH);
  }
  else // must be I2C
#endif // !ATtiny85
  {
    if (pOBD->bbi2c.bWire && iLen > 32) // Hardware I2C has write length limits
    {
       iLen--; // don't count the 0x40 byte the first time through
       while (iLen >= 31) // max 31 data byes + data introducer
       {
          I2CWrite(&pOBD->bbi2c, pOBD->oled_addr, pData, 32);
          iLen -= 31;
          pData += 31;
          pData[0] = 0x40;
       }
       if (iLen) iLen++; // If remaining data, add the last 0x40 byte
    }
    if (iLen) // if any data remaining
    {
      I2CWrite(&pOBD->bbi2c, pOBD->oled_addr, pData, iLen);
    }
  } // I2C
} /* RawWrite() */
static void RawWriteData(OBDISP *pOBD, unsigned char *pData, int iLen)
{
#if !defined( __AVR_ATtiny85__ )
  if (pOBD->com_mode == COM_SPI) // we're writing to SPI, treat it differently
  {
    if (pOBD->iDCPin != 0xff)
      digitalWrite(pOBD->iDCPin, HIGH); // data mode
    if (pOBD->iCSPin != 0xff)
      digitalWrite(pOBD->iCSPin, LOW);
//#ifdef HAL_ESP32_HAL_H_
//   {
//   uint8_t ucTemp[1024];
//        mySPI->transferBytes(pData, ucTemp, iLen);
//   }
//#else
    if (pOBD->iFlags & OBD_CS_EVERY_BYTE) {
        for (int i=0; i<iLen; i++) {
            digitalWrite(pOBD->iCSPin, LOW);
            mySPI->transfer(pData[i]);
            digitalWrite(pOBD->iCSPin, HIGH);
        }
      } else { // no need for CS on every byte
          for (int i=0; i<iLen; i++) {
              mySPI->transfer(pData[i]);
          }
      }
//#endif
    if (pOBD->iCSPin != 0xff && pOBD->type != SHARP_144x168 && pOBD->type != SHARP_400x240)
      digitalWrite(pOBD->iCSPin, HIGH);
  }
  else // must be I2C
#endif // !ATtiny85
  {
    if (pOBD->bbi2c.bWire && iLen > 32) // Hardware I2C has write length limits
    {
       iLen--; // don't count the 0x40 byte the first time through
       while (iLen >= 31) // max 31 data byes + data introducer
       {
          I2CWrite(&pOBD->bbi2c, pOBD->oled_addr, pData, 32);
          iLen -= 31;
          pData += 31;
          pData[0] = 0x40;
       }
       if (iLen) iLen++; // If remaining data, add the last 0x40 byte
    }
    if (iLen) // if any data remaining
    {
      I2CWrite(&pOBD->bbi2c, pOBD->oled_addr, pData, iLen);
    }
  } // I2C
} /* RawWrite() */
#endif // _LINUX_

// Send a single byte command to the OLED controller
void obdWriteCommand(OBDISP *pOBD, unsigned char c)
{
#ifndef MEMORY_ONLY
unsigned char buf[4];

  if (pOBD->com_mode == COM_I2C) {// I2C device
      buf[0] = 0x00; // command introducer
      buf[1] = c;
      RawWrite(pOBD, buf, 2);
  } else { // must be SPI
      obdSetDCMode(pOBD, MODE_COMMAND);
      if (pOBD->iCSPin != 0xff)
          digitalWrite(pOBD->iCSPin, LOW);
#ifdef _LINUX_
      AIOWriteSPI(pOBD->bbi2c.file_i2c, &c, 1);
#else
      if (pOBD->bBitBang)
          SPI_BitBang(pOBD, &c, 1, pOBD->iMOSIPin, pOBD->iCLKPin);
      else
          mySPI->transfer(c);
#endif
      if (pOBD->iCSPin != 0xff)
          digitalWrite(pOBD->iCSPin, HIGH);
      obdSetDCMode(pOBD, MODE_DATA);
  }
#endif // MEMORY_ONLY
} /* obdWriteCommand() */

//
// Byte operands for compressing the data
// The first 2 bits are the type, followed by the counts
#define OP_MASK 0xc0
#define OP_SKIPCOPY 0x00
#define OP_COPYSKIP 0x40
#define OP_REPEATSKIP 0x80
#define OP_REPEAT 0xc0
//
// Write a block of flash memory to the display
//
void oledWriteFlashBlock(OBDISP *pOBD, uint8_t *s, int iLen)
{
int j;
int iWidthMask = pOBD->width -1;
int iSizeMask = ((pOBD->width * pOBD->height)/8) - 1;
int iWidthShift = (pOBD->width == 128) ? 7:6; // assume 128 or 64 wide
uint8_t ucTemp[128];

     while (((pOBD->iScreenOffset & iWidthMask) + iLen) >= pOBD->width) // if it will hit the page end
     {
        j = pOBD->width - (pOBD->iScreenOffset & iWidthMask); // amount we can write in one shot
        memcpy_P(ucTemp, s, j);
        obdWriteDataBlock(pOBD, ucTemp, j, 1);
        s += j;
        iLen -= j;
        pOBD->iScreenOffset = (pOBD->iScreenOffset + j) & iSizeMask;
        obdSetPosition(pOBD, pOBD->iScreenOffset & iWidthMask, (pOBD->iScreenOffset >> iWidthShift), 1);
     } // while it needs some help
  memcpy_P(ucTemp, s, iLen);
  obdWriteDataBlock(pOBD, ucTemp, iLen, 1);
  pOBD->iScreenOffset = (pOBD->iScreenOffset + iLen) & iSizeMask;
} /* oledWriteFlashBlock() */

//
// Create a virtual display of any size
// The memory buffer must be provided at the time of creation
//
void obdCreateVirtualDisplay(OBDISP *pOBD, int width, int height, uint8_t *buffer)
{
  if (pOBD != NULL && buffer != NULL)
  {
    pOBD->width = width;
    pOBD->height = height;
    pOBD->type = LCD_VIRTUAL;
    pOBD->ucScreen = buffer;
    pOBD->iCursorX = pOBD->iCursorY = 0;
    pOBD->iScreenOffset = 0;
  }
} /* obdCreateVirtualDisplay() */
//
// Scroll the internal buffer by 1 scanline (up/down)
// width is in pixels, lines is group of 8 rows
//
int obdScrollBuffer(OBDISP *pOBD, int iStartCol, int iEndCol, int iStartRow, int iEndRow, int bUp)
{
    uint8_t b, *s;
    int col, row;
    int iPitch;

    if (iStartCol < 0 || iStartCol >= pOBD->width || iEndCol < 0 || iEndCol > pOBD->width || iStartCol > iEndCol) // invalid
        return -1;
    if (iStartRow < 0 || iStartRow >= (pOBD->height/8) || iEndRow < 0 || iEndRow >= (pOBD->height/8) || iStartRow > iEndRow)
        return -1;
    iPitch = pOBD->width;
    if (bUp)
    {
        for (row=iStartRow; row<=iEndRow; row++)
        {
            s = &pOBD->ucScreen[(row * iPitch) + iStartCol];
            for (col=iStartCol; col<=iEndCol; col++)
            {
                b = *s;
                b >>= 1; // scroll pixels 'up'
                if (row < iEndRow)
                    b |= (s[iPitch] << 7); // capture pixel of row below, except for last row
                *s++ = b;
            } // for col
        } // for row
    } // up
    else // down
    {
        for (row=iEndRow; row>=iStartRow; row--)
        {
            s = &pOBD->ucScreen[(row * iPitch)+iStartCol];
            for (col=iStartCol; col<=iEndCol; col++)
            {
                b = *s;
                b <<= 1; // scroll down
                if (row > iStartRow)
                    b |= (s[-iPitch] >> 7); // capture pixel of row above
                *s++ = b;
            } // for col
        } // for row
    }
    return 0;
} /* obdScrollBuffer() */
//
// Return the number of bytes accumulated as commands
//
int obdGetCommandLen(OBDISP *pOBD)
{
   if (pOBD != NULL && pOBD->type == DISPLAY_COMMANDS)
       return pOBD->iScreenOffset;
   else
       return 0;
} /* obdGetCommandLen() */
//
// Write a single byte into the command queue
//
static void obdWriteCmdByte(OBDISP *pOBD, uint8_t u8)
{
uint8_t *d = pOBD->ucScreen;
int i = pOBD->iScreenOffset;
      
    d[i++] = u8;
    pOBD->iScreenOffset = i;

} /* obdWriteCmdByte() */
//
// Write an unsigned integer into the command queue
// use integer expansion to write small values as 1 byte
// and larger values as 2 bytes
//
static void obdWriteCmdInt(OBDISP *pOBD, int iValue)
{
uint8_t *d = pOBD->ucScreen;
int i = pOBD->iScreenOffset;

    if (iValue <= 127) {
        d[i++] = (uint8_t)iValue;
    } else {
        d[i++] = (uint8_t)(iValue | 0x80); // store lower 7 bits
        d[i++] = (uint8_t)(iValue >> 7); // store next 8 bits
    }
    pOBD->iScreenOffset = i;

} /* obdWriteCmdInt() */
//
// Read an unsigned integer from the command queue
// use integer expansion to store small values as 1 byte
// and larger values as 2 bytes
//
static uint8_t * obdReadCmdInt(uint8_t *pData, int *iValue)
{
int i;
    
    i = *pData++;
    if (i & 0x80) { // two byte integer
        i &= 0x7f;
        i |= (pData[0] << 7);
        pData++;
    }
    *iValue = i;
    return pData;
} /* obdReadCmdInt() */
//
// Set the current custom font pointers for playing back
// bytewise commands
//
void obdSetCustomFont(OBDISP *pOBD, GFXfont *pFont, uint8_t ucFont)
{
    if (pOBD != NULL && pFont != NULL && ucFont >= FONT_CUSTOM0 && ucFont <= FONT_CUSTOM2) {
        pOBD->pFont[ucFont-FONT_CUSTOM0] = (void *)pFont;
    }
} /* obdSetCustomFont() */ 
//
// Execute a set of bytewise command bytes
// and execute the drawing instructions on the current display/buffer
// Optionally render on backbuffer or physical display
//
void obdExecCommands(uint8_t *pData, int iLen, OBDISP *pOBD, int bRender)
{
uint8_t *s, *pEnd;
uint8_t uc, ucColor=1, ucFill, ucFont;
int x1, y1, x2, y2;
int iTextLen, iPitch;
uint8_t ucTemp[64];

  if (pData == NULL || pOBD == NULL) return;

  s = pData; // source of the command data
  pEnd = &s[iLen];
  while (s < pEnd-1) {
      uc = *s++;
     switch (uc & 0xf) { // lower 4 bits hold command
        case OBD_FILL:
           obdFill(pOBD, s[0], bRender);
           s++;
           break;
        case OBD_DRAWTEXT:
           iTextLen = *s++;
             ucColor = (uc >> 7); // invert flag
             ucFont = (uc >> 4) & 7; // font size
           if (pEnd - s >= iTextLen+3) {
               s = obdReadCmdInt(s, &x1); // col
               s = obdReadCmdInt(s, &y1); // row
              memcpy(ucTemp, s, iTextLen);
              ucTemp[iTextLen] = 0; // terminate the string
              s += iTextLen;
               if (ucFont >= FONT_CUSTOM0) { // up to 3 custom fonts
                   if (pOBD->pFont[ucFont-FONT_CUSTOM0] != NULL) {
                       obdWriteStringCustom(pOBD, (GFXfont *)pOBD->pFont[ucFont-FONT_CUSTOM0], x1, y1, (char *)ucTemp, ucColor);
                   }
               } else {
                   obdWriteString(pOBD, 0, x1, y1, (char *)ucTemp, ucFont, ucColor, bRender);
               }
           } else {
              return; // something went wrong!
           }           
           break;
        case OBD_DRAWLINE:
           if (pEnd - s >= 4) {
               ucColor = (uc >> 4) & 1;
               s = obdReadCmdInt(s, &x1);
               s = obdReadCmdInt(s, &y1);
               s = obdReadCmdInt(s, &x2);
               s = obdReadCmdInt(s, &y2);
              obdDrawLine(pOBD, x1, y1, x2, y2, ucColor, bRender);
           }
           break;
        case OBD_DRAWRECT:
           if (pEnd - s >= 4) {
               ucColor = (uc >> 4) & 1;
               ucFill = (uc >> 5) & 1;
               s = obdReadCmdInt(s, &x1);
               s = obdReadCmdInt(s, &y1);
               s = obdReadCmdInt(s, &x2);
               s = obdReadCmdInt(s, &y2);
              obdRectangle(pOBD, x1, y1, x2, y2, ucColor, ucFill);
           }
           break;
        case OBD_DRAWELLIPSE:
           if (pEnd - s >= 4) {
               ucColor = (uc >> 4) & 1;
               ucFill = (uc >> 5) & 1;
               s = obdReadCmdInt(s, &x1);
               s = obdReadCmdInt(s, &y1);
               s = obdReadCmdInt(s, &x2);
               s = obdReadCmdInt(s, &y2);
              obdEllipse(pOBD, x1, y1, x2, y2, ucColor, ucFill);
           }
           break;
        case OBD_DRAWSPRITE:
           if (pEnd - s >= 8) {
               ucColor = (uc >> 4) & 1;
               s = obdReadCmdInt(s, &x1); // width / height
               s = obdReadCmdInt(s, &y1);
               s = obdReadCmdInt(s, &x2); // destination x/y
               s = obdReadCmdInt(s, &y2);
              iPitch = (x1 + 7) >> 3;
              if (pEnd - s >= (iPitch * y1)) { // enough to hold the data
                 obdDrawSprite(pOBD, s, x1, y1, iPitch, x2, y2, ucColor);
                 s += (iPitch * y1);
              } else {
                 return; // error!
              }  
           }
           break;
        default:
           return; // invalid command!
     }
  }
} /* obdParseCommands() */
//
// Send commands to position the "cursor" (aka memory write address)
// to the given row and column
//
void obdSetPosition(OBDISP *pOBD, int x, int y, int bRender)
{
unsigned char buf[4];
int iPitch = pOBD->width;

    if (pOBD->type == LCD_ST7302) { // special case for ST7302
        obdST7302SetPos(pOBD, x, y);
    }
    y >>= 3; // DEBUG - since we address the display by lines of 8 pixels
  pOBD->iScreenOffset = (y*iPitch)+x;

  if (pOBD->type == LCD_VIRTUAL || pOBD->type >= SHARP_144x168)
    return; // nothing to do
  if (!bRender)
      return; // don't send the commands to the OLED if we're not rendering the graphics now
    obdCachedFlush(pOBD, bRender); // flush any cached data first
  if (pOBD->type == LCD_NOKIA5110)
  {
      obdWriteCommand(pOBD, 0x40 | y);
      obdWriteCommand(pOBD, 0x80 | x);
      return;
  }
  if (pOBD->type == OLED_80x128) // visible portion starts at column 24, row 0
  {
    x += 24;
  }
  else if (pOBD->type == OLED_64x32) // visible display starts at column 32, row 4
  {
    x += 32; // display is centered in VRAM, so this is always true
    if (pOBD->flip == 0) // non-flipped display starts from line 4
       y += 4;
  }
  else  if (pOBD->type == OLED_64x48) {
      x += 32;
      if (!pOBD->flip)
          y += 2; // 128x64 internal RAM
  }
  else if (pOBD->type == LCD_UC1701 && pOBD->flip)
  {
    x += 4;
  }
  else if (pOBD->type == OLED_132x64) // SH1106 has 128 pixels centered in 132
  {
    x += 2;
  }
  else if (pOBD->type == OLED_96x16) // visible display starts at line 2
  { // mapping is a bit strange on the 96x16 OLED
    if (pOBD->flip)
      x += 32;
    else
      y += 2;
  }
  else if (pOBD->type == OLED_72x40) // starts at x=28,y=3
  {
    x += 28;
    if (!pOBD->flip)
    {
      y += 3;
    }
  }
    if (pOBD->com_mode == COM_I2C) { // I2C device
      buf[0] = 0x00; // command introducer
      buf[1] = 0xb0 | y; // set page to Y
      buf[2] = x & 0xf; // lower column address
      buf[3] = 0x10 | (x >> 4); // upper column addr
      RawWrite(pOBD, buf, 4);
    } else { // SPI mode
        obdWriteCommand(pOBD, 0xb0 | y); // set Y
        obdWriteCommand(pOBD, 0x10 | (x >> 4)); // set X MSB
        obdWriteCommand(pOBD, 0x00 | (x & 0xf)); // set X LSB
    }
} /* obdSetPosition() */

//
// Write a block of pixel data to the OLED
// Length can be anything from 1 to 1024 (whole display)
//
void obdWriteDataBlock(OBDISP *pOBD, unsigned char *ucBuf, int iLen, int bRender)
{
unsigned char ucTemp[196];
int iPitch, iBufferSize;
int iRedOffset = 0;
    
  iPitch = pOBD->width;
  iBufferSize = iPitch * ((pOBD->height+7) / 8);
    if (pOBD->iFG == OBD_RED) {
        iRedOffset = pOBD->width * ((pOBD->height+7)/8);
    }
// Keep a copy in local buffer
if (pOBD->ucScreen && (iLen + pOBD->iScreenOffset) <= iBufferSize)
{
  memcpy(&pOBD->ucScreen[iRedOffset + pOBD->iScreenOffset], ucBuf, iLen);
  pOBD->iScreenOffset += iLen;
  // wrap around ?
  if (pOBD->iScreenOffset >= iBufferSize)
    pOBD->iScreenOffset -= iBufferSize;
}
if (pOBD->type == LCD_VIRTUAL || pOBD->type >= SHARP_144x168)
  return; // nothing else to do
#ifndef MEMORY_ONLY
// Copying the data has the benefit in SPI mode of not letting
// the original data get overwritten by the SPI.transfer() function
  if (bRender)
  {
      if (pOBD->com_mode == COM_SPI) // SPI/Bit Bang
      {
          if (pOBD->iCSPin != 0xff)
              digitalWrite(pOBD->iCSPin, LOW);
#ifdef _LINUX_
	  AIOWriteSPI(pOBD->bbi2c.file_i2c, ucBuf, iLen);
#else // Arduino
          if (pOBD->bBitBang) // Bit Bang
            SPI_BitBang(pOBD, ucBuf, iLen, pOBD->iMOSIPin, pOBD->iCLKPin);
          else
            mySPI->transfer(ucBuf, iLen);
#endif // _LINUX_
        if (pOBD->iCSPin != 0xff)
          digitalWrite(pOBD->iCSPin, HIGH);
      }
      else // I2C
      {
          ucTemp[0] = 0x40; // data command
          memcpy(&ucTemp[1], ucBuf, iLen);
          RawWrite(pOBD, ucTemp, iLen+1);
      }
  }
#endif // MEMORY_ONLY
} /* obdWriteDataBlock() */
//
// Write a repeating byte to the display
//
void obdRepeatByte(OBDISP *pOBD, uint8_t b, int iLen)
{
int j;
int iWidthMask = pOBD->width -1;
int iWidthShift = (pOBD->width == 128) ? 7:6; // assume 128 or 64 pixels wide
int iSizeMask = ((pOBD->width * pOBD->height)/8) -1;
uint8_t ucTemp[128];

     memset(ucTemp, b, (iLen > 128) ? 128:iLen);
     while (((pOBD->iScreenOffset & iWidthMask) + iLen) >= pOBD->width) // if it will hit the page end
     {
        j = pOBD->width - (pOBD->iScreenOffset & iWidthMask); // amount we can write in one shot
        obdWriteDataBlock(pOBD, ucTemp, j, 1);
        iLen -= j;
        pOBD->iScreenOffset = (pOBD->iScreenOffset + j) & iSizeMask;
        obdSetPosition(pOBD, pOBD->iScreenOffset & iWidthMask, (pOBD->iScreenOffset >> iWidthShift), 1);
     } // while it needs some help
  obdWriteDataBlock(pOBD, ucTemp, iLen, 1);
  pOBD->iScreenOffset += iLen;
} /* obdRepeatByte() */

//
// Play a frame of animation data
// The animation data is assumed to be encoded for a full frame of the display
// Given the pointer to the start of the compressed data,
// it returns the pointer to the start of the next frame
// Frame rate control is up to the calling program to manage
// When it finishes the last frame, it will start again from the beginning
//
uint8_t * obdPlayAnimFrame(OBDISP *pOBD, uint8_t *pAnimation, uint8_t *pCurrent, int iLen)
{
uint8_t *s;
int i, j;
unsigned char b, bCode;
int iBufferSize = (pOBD->width * pOBD->height)/8; // size in bytes of the display devce
int iWidthMask, iWidthShift;

  iWidthMask = pOBD->width - 1;
  iWidthShift = (pOBD->width == 128) ? 7:6; // 128 or 64 pixels wide
  if (pCurrent == NULL || pCurrent > pAnimation + iLen)
     return NULL; // invalid starting point

  s = (uint8_t *)pCurrent; // start of animation data
  i = 0;
  obdSetPosition(pOBD, 0,0,1);
  while (i < iBufferSize) // run one frame
  {
    bCode = pgm_read_byte(s++);
    switch (bCode & OP_MASK) // different compression types
    {
      case OP_SKIPCOPY: // skip/copy
        if (bCode == OP_SKIPCOPY) // big skip
        {
           b = pgm_read_byte(s++);
           i += b + 1;
           obdSetPosition(pOBD, i & iWidthMask, (i >> iWidthShift), 1);
        }
        else // skip/copy
        {
          if (bCode & 0x38)
          {
            i += ((bCode & 0x38) >> 3); // skip amount
            obdSetPosition(pOBD, i & iWidthMask, (i >> iWidthShift), 1);
          }
          if (bCode & 7)
          {
             oledWriteFlashBlock(pOBD, s, bCode & 7);
             s += (bCode & 7);
             i += bCode & 7;
          }
       }
       break;
     case OP_COPYSKIP: // copy/skip
       if (bCode == OP_COPYSKIP) // big copy
       {
         b = pgm_read_byte(s++);
         j = b + 1;
         oledWriteFlashBlock(pOBD, s, j);
         s += j;
         i += j;
       }
       else
       {
         j = ((bCode & 0x38) >> 3);
         if (j)
         {
           oledWriteFlashBlock(pOBD, s, j);
           s += j;
           i += j;
         }
         if (bCode & 7)
         {
           i += (bCode & 7); // skip
           obdSetPosition(pOBD, i & iWidthMask, (i >> iWidthShift), 1);
         }
       }
       break;
     case OP_REPEATSKIP: // repeat/skip
       j = (bCode & 0x38) >> 3; // repeat count
       b = pgm_read_byte(s++);
       obdRepeatByte(pOBD, b, j);
       i += j;
       if (bCode & 7)
       {
         i += (bCode & 7); // skip amount
         obdSetPosition(pOBD, i & iWidthMask, (i >> iWidthShift), 1);
       }
       break;
                  
     case OP_REPEAT:
       j = (bCode & 0x3f) + 1;
       b = pgm_read_byte(s++);
       obdRepeatByte(pOBD, b, j);
       i += j;
       break;
    } // switch on code type
  } // while rendering a frame
  if (s >= pAnimation + iLen) // we've hit the end, restart from the beginning
     s = pAnimation;
  return s; // return pointer to start of next frame
} /* obdPlayAnimFrame() */
//
// Draw a sprite of any size in any position
// If it goes beyond the left/right or top/bottom edges
// it's trimmed to show the valid parts
// This function requires a back buffer to be defined
// The priority color (0 or 1) determines which color is painted
// when a 1 is encountered in the source image.
//
void obdDrawSprite(OBDISP *pOBD, uint8_t *pSprite, int cx, int cy, int iPitch, int x, int y, uint8_t iPriority)
{
    int tx, ty, dx, dy, iStartX;
    uint8_t *s, *d, uc, pix, ucSrcMask, ucDstMask;
    int iLocalPitch;
    iLocalPitch = pOBD->width;

    if (pOBD == NULL) return;
    if (pOBD->type == DISPLAY_COMMANDS) { // encode this as a command sequence
       int iLocalPitch = (cx+7)>>3;
        obdWriteCmdByte(pOBD, OBD_DRAWSPRITE | ((iPriority & 1) << 4));
        obdWriteCmdInt(pOBD, cx);
        obdWriteCmdInt(pOBD, cy);
        obdWriteCmdInt(pOBD, x);
        obdWriteCmdInt(pOBD, y);
        d = pOBD->ucScreen;
        tx = pOBD->iScreenOffset;
       s = pSprite;
       for (ty=0; ty<cy; ty++) { // copy only the part we want to the output
           memcpy(&d[tx], s, iLocalPitch);
           s += iPitch;
           tx += iLocalPitch;
       }
       pOBD->iScreenOffset = tx; // store new length
       return; // done
    }

    if (x+cx < 0 || y+cy < 0 || x >= pOBD->width || y >= pOBD->height || pOBD->ucScreen == NULL)
        return; // no backbuffer or out of bounds
    dy = y; // destination y
    if (y < 0) // skip the invisible parts
    {
        cy += y;
        y = -y;
        pSprite += (y * iPitch);
        dy = 0;
    }
    if (y + cy > pOBD->height)
        cy = pOBD->height - y;
    iStartX = 0;
    dx = x;
    if (x < 0)
    {
        cx += x;
        x = -x;
        iStartX = x;
        dx = 0;
    }
    if (x + cx > pOBD->width)
        cx = pOBD->width - x;
    for (ty=0; ty<cy; ty++)
    {
        s = &pSprite[(iStartX >> 3)];
        d = &pOBD->ucScreen[(dy>>3) * iLocalPitch + dx];
        ucSrcMask = 0x80 >> (iStartX & 7);
        pix = *s++;
        ucDstMask = 1 << (dy & 7);
        if (iPriority) // priority color is 1
        {
          for (tx=0; tx<cx; tx++)
          {
            uc = d[0];
            if (pix & ucSrcMask) // set pixel in source, set it in dest
              d[0] = (uc | ucDstMask);
            d++; // next pixel column
            ucSrcMask >>= 1;
            if (ucSrcMask == 0) // read next byte
            {
                ucSrcMask = 0x80;
                pix = *s++;
            }
          } // for tx
        } // priorty color 1
        else
        {
          for (tx=0; tx<cx; tx++)
          {
            uc = d[0];
            if (pix & ucSrcMask) // clr pixel in source, clr it in dest
              d[0] = (uc & ~ucDstMask);
            d++; // next pixel column
            ucSrcMask >>= 1;
            if (ucSrcMask == 0) // read next byte
            {
                ucSrcMask = 0x80;
                pix = *s++;
            }
          } // for tx
        } // priority color 0
        dy++;
        pSprite += iPitch;
    } // for ty
} /* obdDrawSprite() */
//
// Draw a 16x16 tile in any of 4 rotated positions
// Assumes input image is laid out like "normal" graphics with
// the MSB on the left and 2 bytes per line
// On AVR, the source image is assumed to be in FLASH memory
// The function can draw the tile on byte boundaries, so the x value
// can be from 0 to width-16 and y can be from 0 to (height/8)-2
//
void obdDrawTile(OBDISP *pOBD, const uint8_t *pTile, int x, int y, int iRotation, int bInvert, int bRender)
{
    uint8_t ucTemp[32]; // prepare LCD data here
    uint8_t i, j, k, iOffset, ucMask, uc, ucPixels;
    uint8_t bFlipX=0, bFlipY=0;
    
    if (x < 0 || y < 0 || y > (pOBD->height/8)-2 || x > pOBD->width-16)
        return; // out of bounds
    if (pTile == NULL) return; // bad pointer; really? :(
    if (iRotation == ANGLE_180 || iRotation == ANGLE_270 || iRotation == ANGLE_FLIPX)
        bFlipX = 1;
    if (iRotation == ANGLE_180 || iRotation == ANGLE_270 || iRotation == ANGLE_FLIPY)
        bFlipY = 1;
    
    memset(ucTemp, 0, sizeof(ucTemp)); // we only set white pixels, so start from black
    if (iRotation == ANGLE_0 || iRotation == ANGLE_180 || iRotation == ANGLE_FLIPX || iRotation == ANGLE_FLIPY)
    {
        for (j=0; j<16; j++) // y
        {
            for (i=0; i<16; i+=8) // x
            {
                ucPixels = pgm_read_byte((uint8_t*)pTile++);
                ucMask = 0x80; // MSB is the first source pixel
                for (k=0; k<8; k++)
                {
                    if (ucPixels & ucMask) // translate the pixel
                    {
                        if (bFlipY)
                            uc = 0x80 >> (j & 7);
                        else
                            uc = 1 << (j & 7);
                        iOffset = i+k;
                        if (bFlipX) iOffset = 15-iOffset;
                        iOffset += (j & 8)<<1; // top/bottom half of output
                        if (bFlipY)
                            iOffset ^= 16;
                        ucTemp[iOffset] |= uc;
                    }
                    ucMask >>= 1;
                } // for k
            } // for i
        } // for j
    }
    else // rotated 90/270
    {
        for (j=0; j<16; j++) // y
        {
            for (i=0; i<16; i+=8) // x
            {
                ucPixels = pgm_read_byte((uint8_t*)pTile++);
                ucMask = 0x80; // MSB is the first source pixel
                for (k=0; k<8; k++)
                {
                    if (ucPixels & ucMask) // translate the pixel
                    {
                        if (bFlipY)
                            uc = 0x80 >> k;
                        else
                            uc = 1 << k;
                        iOffset = 15-j;
                        if (bFlipX) iOffset = 15-iOffset;
                        iOffset += i<<1; // top/bottom half of output
                        if (bFlipY)
                            iOffset ^= 16;
                        ucTemp[iOffset] |= uc;
                    }
                    ucMask >>= 1;
                } // for k
            } // for i
        } // for j
    }
    if (bInvert) InvertBytes(ucTemp, 32);
    // Send the data to the display
    obdSetPosition(pOBD, x, y, bRender);
    obdWriteDataBlock(pOBD, ucTemp, 16, bRender); // top half
    obdSetPosition(pOBD, x,y+1, bRender);
    obdWriteDataBlock(pOBD, &ucTemp[16], 16, bRender); // bottom half
} /* obdDrawTile() */

// Set (or clear) an individual pixel
// The local copy of the frame buffer is used to avoid
// reading data from the display controller
int obdSetPixel(OBDISP *pOBD, int x, int y, unsigned char ucColor, int bRender)
{
int i;
unsigned char uc, ucOld;
int iPitch, iSize;

  iPitch = pOBD->width;
  iSize = iPitch * ((pOBD->height+7)/8);

  i = ((y >> 3) * iPitch) + x;
  if (i < 0 || i > iSize-1) // off the screen
    return -1;
  obdSetPosition(pOBD, x, y, bRender);

  if (pOBD->ucScreen)
    uc = ucOld = pOBD->ucScreen[i];
#ifndef MEMORY_ONLY
  else if (pOBD->type == OLED_132x64 || pOBD->type == OLED_128x128) // SH1106/SH1107 can read data
  {
    uint8_t ucTemp[3];
     ucTemp[0] = 0x80; // one command
     ucTemp[1] = 0xE0; // read_modify_write
     ucTemp[2] = 0xC0; // one data
     RawWrite(pOBD, ucTemp, 3);

     // read a dummy byte followed by the data byte we want
     I2CRead(&pOBD->bbi2c, pOBD->oled_addr, ucTemp, 2);
     uc = ucOld = ucTemp[1]; // first byte is garbage
  }
#endif // MEMORY_ONLY
  else
     uc = ucOld = 0;

  uc &= ~(0x1 << (y & 7));
  if (ucColor)
  {
    uc |= (0x1 << (y & 7));
  }
  if (uc != ucOld) // pixel changed
  {
//    obdSetPosition(x, y>>3);
    if (pOBD->ucScreen)
    {
      obdWriteDataBlock(pOBD, &uc, 1, bRender);
      pOBD->ucScreen[i] = uc;
    }
    else if (pOBD->type == OLED_132x64 || pOBD->type == OLED_128x128) // end the read_modify_write operation
    {
      uint8_t ucTemp[4];
      ucTemp[0] = 0xc0; // one data
      ucTemp[1] = uc;   // actual data
      ucTemp[2] = 0x80; // one command
      ucTemp[3] = 0xEE; // end read_modify_write operation
      RawWrite(pOBD, ucTemp, 4);
    }
  }
  return 0;
} /* obdSetPixel() */

//
// Invert font data
//
void InvertBytes(uint8_t *pData, uint8_t bLen)
{
uint8_t i;
   for (i=0; i<bLen; i++)
   {
      *pData = ~(*pData);
      pData++;
   }
} /* InvertBytes() */

//
// Load a 1-bpp Windows bitmap
// Pass the pointer to the beginning of the BMP file
// First pass version assumes a full screen bitmap
//
int obdLoadBMP(OBDISP *pOBD, uint8_t *pBMP, int dx, int dy, int bInvert)
{
int16_t i16, cx, cy;
int iOffBits; // offset to bitmap data
int iPitch, iDestPitch;
uint8_t x, y, b=0, *s, *d;
uint8_t dst_mask, src_mask;
uint8_t bFlipped = false;

  iDestPitch = pOBD->width;
  i16 = pgm_read_word(pBMP);
  if (i16 != 0x4d42) // must start with 'BM'
     return -1; // not a BMP file
  cx = pgm_read_word(pBMP + 18);
  if (cx + dx > pOBD->width) // must fit on the display
     return -1;
  cy = pgm_read_word(pBMP + 22);
  if (cy < 0) cy = -cy;
  else bFlipped = true;
  if (cy + dy > pOBD->height) // must fit on the display
     return -1;
  i16 = pgm_read_word(pBMP + 28);
  if (i16 != 1) // must be 1 bit per pixel
     return -1;
  iOffBits = pgm_read_word(pBMP + 10);
  iPitch = (((cx+7)>>3) + 3) & 0xfffc; // must be DWORD aligned
  if (bFlipped)
  {
    iOffBits += ((cy-1) * iPitch); // start from bottom
    iPitch = -iPitch;
  }

  for (y=0; y<cy; y++)
  {
     dst_mask = 1 << ((y+dy) & 7);
     if (pOBD->ucScreen)
        d = &pOBD->ucScreen[(((y+dy)>>3)*iDestPitch)+dx];
     else
     {
        d = u8Cache;
        if ((y & 7) == 0)
           memset(u8Cache, 0, sizeof(u8Cache));
     }
     s = &pBMP[iOffBits + (y*iPitch)];
     src_mask = 0;
     for (x=0; x<cx; x++)
     {
        if (src_mask == 0) // need to load the next byte
        {
           b = pgm_read_byte(s++);
           src_mask = 0x80; // MSB on left
        }
        if (b & src_mask)
        {
           if (bInvert)
              d[0] &= ~dst_mask;
           else
              d[0] |= dst_mask;
        } else {
            if (bInvert)
               d[0] |= dst_mask;
            else
               d[0] &= ~dst_mask;
        }
        d++;
        src_mask >>= 1;
     } // for x
     if (pOBD->ucScreen == NULL && ((y & 7) == 7 || y == cy-1)) // dump to LCD
     {
       obdSetPosition(pOBD, dx, (y+dy)>>3, 1);
       obdWriteDataBlock(pOBD, u8Cache, cx, 1);
     }
  } // for y
  return 0;
} /* obdLoadBMP() */
//
// Set the current cursor position
// The column represents the pixel column (0-127)
// The row represents the text row (0-7)
//
void obdSetCursor(OBDISP *pOBD, int x, int y)
{
  pOBD->iCursorX = x;
  pOBD->iCursorY = y;
} /* obdSetCursor() */
//
// Advance to the next line
//
void obdNextLine(OBDISP *pOBD)
{
   pOBD->iCursorX = 0;
   pOBD->iCursorY++;
} /* obdNextLine() */
//
// Turn text wrap on or off for the oldWriteString() function
//
void obdSetTextWrap(OBDISP *pOBD, int bWrap)
{
  pOBD->wrap = bWrap;
} /* obdSetTextWrap() */
//
// Draw a string with a fractional scale in both dimensions
// the scale is a 16-bit integer with and 8-bit fraction and 8-bit mantissa
// To draw at 1x scale, set the scale factor to 256. To draw at 2x, use 512
// The output must be drawn into a memory buffer, not directly to the display
//
int obdScaledString(OBDISP *pOBD, int x, int y, char *szMsg, int iSize, int iColor, int iXScale, int iYScale, int iRotation)
{
uint32_t row, col, dx, dy;
uint32_t sx, sy;
uint8_t c, uc, color, *d;
const uint8_t *s;
uint8_t ucTemp[16];
int tx, ty, bit, iFontOff;
int iPitch, iRedOffset = 0;
int iFontWidth;

   if (iXScale == 0 || iYScale == 0 || szMsg == NULL || pOBD == NULL || pOBD->ucScreen == NULL || x < 0 || y < 0 || x >= pOBD->width-1 || y >= pOBD->height-1)
      return -1; // invalid display structure
   if (iSize != FONT_8x8 && iSize != FONT_6x8)
      return -1; // only on the small fonts (for now)
   iFontWidth = (iSize == FONT_6x8) ? 6:8;
   s = (iSize == FONT_6x8) ? ucSmallFont : ucFont;
   iPitch = pOBD->width;
    if (pOBD->iFG == OBD_RED) {
        // use the second half of the image buffer
        iRedOffset = pOBD->width * ((pOBD->height+7)/8);
    }
   dx = (iFontWidth * iXScale) >> 8; // width of each character
   dy = (8 * iYScale) >> 8; // height of each character
   sx = 65536 / iXScale; // turn the scale into an accumulator value
   sy = 65536 / iYScale;
   while (*szMsg) {
      c = *szMsg++; // debug - start with normal font
      iFontOff = (int)(c-32) * (iFontWidth-1);
      // we can't directly use the pointer to FLASH memory, so copy to a local buffer
      ucTemp[0] = 0; // first column is blank
      memcpy_P(&ucTemp[1], &s[iFontOff], iFontWidth-1);
      if (iColor == OBD_WHITE) InvertBytes(ucTemp, iFontWidth);
      col = 0;
      for (tx=0; tx<(int)dx; tx++) {
         row = 0;
         uc = ucTemp[col >> 8];
         for (ty=0; ty<(int)dy; ty++) {
            int nx=0, ny=0;
            bit = row >> 8;
            color = (uc & (1 << bit)); // set or clear the pixel
            switch (iRotation) {
               case ROT_0:
                  nx = x + tx;
                  ny = y + ty;
                  break;
               case ROT_90:
                  nx = x - ty;
                  ny = y + tx;
                  break;
               case ROT_180:
                  nx = x - tx;
                  ny = y - ty;
                  break;
               case ROT_270:
                  nx = x + ty;
                  ny = y - tx;
                  break;
            } // switch on rotation direction
            // plot the pixel if it's within the image boundaries
            if (nx >= 0 && ny >= 0 && nx < pOBD->width && ny < pOBD->height) {
               d = &pOBD->ucScreen[(ny >> 3) * iPitch + nx + iRedOffset];
               if (color)
                  d[0] |= (1 << (ny & 7));
               else
                  d[0] &= ~(1 << (ny & 7));
            }
            row += sy; // add fractional increment to source row of character
         } // for ty
         col += sx; // add fractional increment to source column
      } // for tx
      // update the 'cursor' position
      switch (iRotation) {
         case ROT_0:
            x += dx;
            break;
         case ROT_90:
            y += dx;
            break;
         case ROT_180:
            x -= dx;
            break;
         case ROT_270:
            y -= dx;
            break;
      } // switch on rotation
   } // while (*szMsg)
   return 0;
} /* obdScaledString() */

//
// Draw a string of normal (8x8), small (6x8) or large (16x32) characters
// At the given col+row
//
int obdWriteString(OBDISP *pOBD, int iScroll, int x, int y, char *szMsg, int iSize, int iColor, int bRender)
{
int i, iFontOff, iLen, iFontSkip;
unsigned char c, *s, ucTemp[40];

  if (pOBD == NULL) return -1;
  if (pOBD->type == DISPLAY_COMMANDS) { // encode this as a command sequence
      uint8_t *d = pOBD->ucScreen;
      iLen = (int)strlen(szMsg);
      obdWriteCmdByte(pOBD, OBD_DRAWTEXT | ((iColor & 1) << 7) | ((iSize & 7) << 4));
      obdWriteCmdByte(pOBD, (uint8_t) iLen);
      obdWriteCmdInt(pOBD, x);
      obdWriteCmdInt(pOBD, y);
      i = pOBD->iScreenOffset;
      memcpy(&d[i], szMsg, iLen);
      i += iLen;
      pOBD->iScreenOffset = i; // store new length
      return 0; // done
  }

    if (x == -1 || y == -1) // use the cursor position
    {
        x = pOBD->iCursorX; y = pOBD->iCursorY;
    } else {
        pOBD->iCursorX = x; pOBD->iCursorY = y;
    }
    if (pOBD->iCursorX >= pOBD->width || pOBD->iCursorY >= pOBD->height)
       return -1; // can't draw off the display

    obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY, bRender);
    if (iSize == FONT_8x8) // 8x8 font
    {
       i = 0;
       iFontSkip = iScroll & 7; // number of columns to initially skip
       while (pOBD->iCursorX < pOBD->width && szMsg[i] != 0 && pOBD->iCursorY < pOBD->height)
       {
         if (iScroll < 8) // only display visible characters
         {
             c = (unsigned char)szMsg[i];
             iFontOff = (int)(c-32) * 7;
             // we can't directly use the pointer to FLASH memory, so copy to a local buffer
             ucTemp[0] = 0; // first column is blank
             memcpy_P(&ucTemp[1], &ucFont[iFontOff], 7);
             if (iColor == OBD_WHITE) InvertBytes(ucTemp, 8);
             iLen = 8 - iFontSkip;
             if (pOBD->iCursorX + iLen > pOBD->width) // clip right edge
                 iLen = pOBD->width - pOBD->iCursorX;
             obdCachedWrite(pOBD, &ucTemp[iFontSkip], iLen, bRender);
//             obdWriteDataBlock(pOBD, &ucTemp[iFontSkip], iLen, bRender); // write character pattern
             pOBD->iCursorX += iLen;
             if (pOBD->iCursorX >= pOBD->width-7 && pOBD->wrap) // word wrap enabled?
             {
               obdCachedFlush(pOBD, bRender);
               pOBD->iCursorX = 0; // start at the beginning of the next line
               pOBD->iCursorY+=8;
               obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY, bRender);
             }
             iFontSkip = 0;
         }
         iScroll -= 8;
         i++;
       } // while
       obdCachedFlush(pOBD, bRender); // write any remaining data
       return 0;
    } // 8x8
#ifndef __AVR__
    else if (iSize == FONT_16x32)
    {
      i = 0;
      iFontSkip = iScroll & 15; // number of columns to initially skip
      while (pOBD->iCursorX < pOBD->width && pOBD->iCursorY < pOBD->height && szMsg[i] != 0)
      {
          if (iScroll < 16) // if characters are visible
          {
              s = (unsigned char *)&ucBigFont[(unsigned char)(szMsg[i]-32)*64];
              iLen = 16 - iFontSkip;
              if (pOBD->iCursorX + iLen > pOBD->width) // clip right edge
                  iLen = pOBD->width - pOBD->iCursorX;
              // we can't directly use the pointer to FLASH memory, so copy to a local buffer
              obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY, bRender);
              memcpy_P(ucTemp, s, 16);
              if (iColor == OBD_WHITE) InvertBytes(ucTemp, 16);
              obdWriteDataBlock(pOBD, &ucTemp[iFontSkip], iLen, bRender); // write character pattern
              obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY+8, bRender);
              memcpy_P(ucTemp, s+16, 16);
              if (iColor == OBD_WHITE) InvertBytes(ucTemp, 16);
                 obdWriteDataBlock(pOBD, &ucTemp[iFontSkip], iLen, bRender); // write character pattern
              obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY+16, bRender);
              memcpy_P(ucTemp, s+32, 16);
              if (iColor == OBD_WHITE) InvertBytes(ucTemp, 16);
                 obdWriteDataBlock(pOBD, &ucTemp[iFontSkip], iLen, bRender); // write character pattern
              obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY+24, bRender);
              memcpy_P(ucTemp, s+48, 16);
              if (iColor == OBD_WHITE) InvertBytes(ucTemp, 16);
                 obdWriteDataBlock(pOBD, &ucTemp[iFontSkip], iLen, bRender); // write character pattern
              pOBD->iCursorX += iLen;
              if (pOBD->iCursorX >= pOBD->width-15 && pOBD->wrap) // word wrap enabled?
              {
                pOBD->iCursorX = 0; // start at the beginning of the next line
                pOBD->iCursorY+=32;
              }
              iFontSkip = 0;
          } // if character visible from scrolling
          iScroll -= 16;
          i++;
       } // while
       return 0;
    } // 16x32
#endif // !__AVR__
    else if (iSize == FONT_16x16) // 8x8 stretched to 16x16
    {
      i = 0;
      iFontSkip = iScroll & 15; // number of columns to initially skip
      while (pOBD->iCursorX < pOBD->width && pOBD->iCursorY < pOBD->height && szMsg[i] != 0)
      {
// stretch the 'normal' font instead of using the big font
          if (iScroll < 16) // if characters are visible
          {
              int tx, ty;
              c = szMsg[i] - 32;
              unsigned char uc1, uc2, ucMask, *pDest;
              s = (unsigned char *)&ucFont[(int)c*7];
              ucTemp[0] = 0; // first column is blank
              memcpy_P(&ucTemp[1], s, 7);
              if (iColor == OBD_WHITE)
                  InvertBytes(ucTemp, 8);
              // Stretch the font to double width + double height
              memset(&ucTemp[8], 0, 32); // write 32 new bytes
              for (tx=0; tx<8; tx++)
              {
                  ucMask = 3;
                  pDest = &ucTemp[8+tx*2];
                  uc1 = uc2 = 0;
                  c = ucTemp[tx];
                  for (ty=0; ty<4; ty++)
                  {
                      if (c & (1 << ty)) // a bit is set
                          uc1 |= ucMask;
                      if (c & (1 << (ty + 4)))
                          uc2 |= ucMask;
                      ucMask <<= 2;
                  }
                  pDest[0] = uc1;
                  pDest[1] = uc1; // double width
                  pDest[16] = uc2;
                  pDest[17] = uc2;
              }
              iLen = 16 - iFontSkip;
              if (pOBD->iCursorX + iLen > pOBD->width) // clip right edge
                  iLen = pOBD->width - pOBD->iCursorX;
              obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY, bRender);
              obdWriteDataBlock(pOBD, &ucTemp[8+iFontSkip], iLen, bRender);
              obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY+8, bRender);
              obdWriteDataBlock(pOBD, &ucTemp[24+iFontSkip], iLen, bRender);
              pOBD->iCursorX += iLen;
              if (pOBD->iCursorX >= pOBD->width-15 && pOBD->wrap) // word wrap enabled?
              {
                pOBD->iCursorX = 0; // start at the beginning of the next line
                pOBD->iCursorY += 16;
                obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY, bRender);
              }
              iFontSkip = 0;
          } // if characters are visible
          iScroll -= 16;
          i++;
      } // while
      return 0;
    } // 16x16
    else if (iSize == FONT_12x16) // 6x8 stretched to 12x16
    {
      i = 0;
      iFontSkip = iScroll % 12; // number of columns to initially skip
      while (pOBD->iCursorX < pOBD->width && pOBD->iCursorY < pOBD->height && szMsg[i] != 0)
      {
// stretch the 'normal' font instead of using the big font
          if (iScroll < 12) // if characters are visible
          {
              int tx, ty;
              c = szMsg[i] - 32;
              unsigned char uc1, uc2, ucMask, *pDest;
              s = (unsigned char *)&ucSmallFont[(int)c*5];
              ucTemp[0] = 0; // first column is blank
              memcpy_P(&ucTemp[1], s, 6);
              if (iColor == OBD_WHITE)
                  InvertBytes(ucTemp, 6);
              // Stretch the font to double width + double height
              memset(&ucTemp[6], 0, 24); // write 24 new bytes
              for (tx=0; tx<6; tx++)
              {
                  ucMask = 3;
                  pDest = &ucTemp[6+tx*2];
                  uc1 = uc2 = 0;
                  c = ucTemp[tx];
                  for (ty=0; ty<4; ty++)
                  {
                      if (c & (1 << ty)) // a bit is set
                          uc1 |= ucMask;
                      if (c & (1 << (ty + 4)))
                          uc2 |= ucMask;
                      ucMask <<= 2;
                  }
                  pDest[0] = uc1;
                  pDest[1] = uc1; // double width
                  pDest[12] = uc2;
                  pDest[13] = uc2;
              }
              // smooth the diagonal lines
              for (tx=0; tx<5; tx++)
              {
                  uint8_t c0, c1, ucMask2;
                  c0 = ucTemp[tx];
                  c1 = ucTemp[tx+1];
                  pDest = &ucTemp[6+tx*2];
                  ucMask = 1;
                  ucMask2 = 2;
                  for (ty=0; ty<7; ty++)
                  {
                      if (((c0 & ucMask) && !(c1 & ucMask) && !(c0 & ucMask2) && (c1 & ucMask2)) || (!(c0 & ucMask) && (c1 & ucMask) && (c0 & ucMask2) && !(c1 & ucMask2)))
                      {
                          if (ty < 3) // top half
                          {
                              if (iColor == OBD_WHITE) {
                                pDest[1] &= ~(1 << ((ty * 2)+1));
                                pDest[2] &= ~(1 << ((ty * 2)+1));
                                pDest[1] &= ~(1 << ((ty+1) * 2));
                                pDest[2] &= ~(1 << ((ty+1) * 2));
                              } else {
                                pDest[1] |= (1 << ((ty * 2)+1));
                                pDest[2] |= (1 << ((ty * 2)+1));
                                pDest[1] |= (1 << ((ty+1) * 2));
                                pDest[2] |= (1 << ((ty+1) * 2));
                              }
                          }
                          else if (ty == 3) // on the border
                          {
                              if (iColor == OBD_WHITE) {
                                pDest[1] &= ~0x80; pDest[2] &= ~0x80;
                                pDest[13] &= ~1; pDest[14] &= ~1;
                              } else {
                                pDest[1] |= 0x80; pDest[2] |= 0x80;
                                pDest[13] |= 1; pDest[14] |= 1;
                              }
                          }
                          else // bottom half
                          {
                              if (iColor == OBD_WHITE) {
                                pDest[13] &= ~(1 << (2*(ty-4)+1));
                                pDest[14] &= ~(1 << (2*(ty-4)+1));
                                pDest[13] &= ~(1 << ((ty-3) * 2));
                                pDest[14] &= ~(1 << ((ty-3) * 2));
                              } else {
                                pDest[13] |= (1 << (2*(ty-4)+1));
                                pDest[14] |= (1 << (2*(ty-4)+1));
                                pDest[13] |= (1 << ((ty-3) * 2));
                                pDest[14] |= (1 << ((ty-3) * 2));
                              }
                          }
                      }
                      else if (!(c0 & ucMask) && (c1 & ucMask) && (c0 & ucMask2) && !(c1 & ucMask2))
                      {
                          if (ty < 4) // top half
                          {
                              if (iColor == OBD_WHITE) {
                                pDest[1] &= ~(1 << ((ty * 2)+1));
                                pDest[2] &= ~(1 << ((ty+1) * 2));
                              } else {
                                pDest[1] |= (1 << ((ty * 2)+1));
                                pDest[2] |= (1 << ((ty+1) * 2));
                              }
                          }
                          else
                          {
                              if (iColor == OBD_WHITE) {
                                pDest[13] &= ~(1 << (2*(ty-4)+1));
                                pDest[14] &= ~(1 << ((ty-3) * 2));
                              } else {
                                pDest[13] |= (1 << (2*(ty-4)+1));
                                pDest[14] |= (1 << ((ty-3) * 2));
                              }
                          }
                      }
                      ucMask <<= 1; ucMask2 <<= 1;
                  }
              }
              iLen = 12 - iFontSkip;
              if (pOBD->iCursorX + iLen > pOBD->width) // clip right edge
                  iLen = pOBD->width - pOBD->iCursorX;
              obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY, bRender);
              obdWriteDataBlock(pOBD, &ucTemp[6+iFontSkip], iLen, bRender);
              obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY+8, bRender);
              obdWriteDataBlock(pOBD, &ucTemp[18+iFontSkip], iLen, bRender);
              pOBD->iCursorX += iLen;
              if (pOBD->iCursorX >= pOBD->width-11 && pOBD->wrap) // word wrap enabled?
              {
                pOBD->iCursorX = 0; // start at the beginning of the next line
                pOBD->iCursorY += 16;
                obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY, bRender);
              }
              iFontSkip = 0;
          } // if characters are visible
          iScroll -= 12;
          i++;
      } // while
      return 0;
    } // 12x16
    else if (iSize == FONT_6x8)
    {
       i = 0;
       iFontSkip = iScroll % 6;
       while (pOBD->iCursorX < pOBD->width && pOBD->iCursorY < pOBD->height && szMsg[i] != 0)
       {
           if (iScroll < 6) // if characters are visible
           {
               c = szMsg[i] - 32;
               // we can't directly use the pointer to FLASH memory, so copy to a local buffer
               ucTemp[0] = 0; // first column is blank
               memcpy_P(&ucTemp[1], &ucSmallFont[(int)c*5], 5);
               if (iColor == OBD_WHITE) InvertBytes(ucTemp, 6);
               iLen = 6 - iFontSkip;
               if (pOBD->iCursorX + iLen > pOBD->width) // clip right edge
                   iLen = pOBD->width - pOBD->iCursorX;
               obdCachedWrite(pOBD, &ucTemp[iFontSkip], iLen, bRender);
//               obdWriteDataBlock(pOBD, &ucTemp[iFontSkip], iLen, bRender); // write character pattern
               pOBD->iCursorX += iLen;
               iFontSkip = 0;
               if (pOBD->iCursorX >= pOBD->width-5 && pOBD->wrap) // word wrap enabled?
               {
                 obdCachedFlush(pOBD, bRender);
                 pOBD->iCursorX = 0; // start at the beginning of the next line
                 pOBD->iCursorY +=8;
                 obdSetPosition(pOBD, pOBD->iCursorX, pOBD->iCursorY, bRender);
               }
           } // if characters are visible
         iScroll -= 6;
         i++;
       }
      obdCachedFlush(pOBD, bRender); // write any remaining data
      return 0;
    } // 6x8
  return -1; // invalid size
} /* obdWriteString() */
//
// Get the width of text in a custom font
//
void obdGetStringBox(GFXfont *pFont, char *szMsg, int *width, int *top, int *bottom)
{
int cx = 0;
unsigned int c, i = 0;
GFXglyph *pGlyph;
int miny, maxy;

   if (width == NULL || top == NULL || bottom == NULL || pFont == NULL || szMsg == NULL) return; // bad pointers
   miny = 100; maxy = 0;
   while (szMsg[i]) {
      c = szMsg[i++];
      if (c < pFont->first || c > pFont->last) // undefined character
         continue; // skip it
      c -= pFont->first; // first char of font defined
      pGlyph = &pFont->glyph[c];
      cx += pGlyph->xAdvance;
      if (pGlyph->yOffset < miny) miny = pGlyph->yOffset;
      if (pGlyph->height+pGlyph->yOffset > maxy) maxy = pGlyph->height+pGlyph->yOffset;
   }
   *width = cx;
   *top = miny;
   *bottom = maxy;
} /* obdGetStringBox() */

//
// Draw a string of characters in a custom font
// A back buffer must be defined
//
int obdWriteStringCustom(OBDISP *pOBD, GFXfont *pFont, int x, int y, char *szMsg, uint8_t ucColor)
{
int i, end_y, dx, dy, tx, ty, iBitOff;
unsigned int c;
uint8_t *s, *d, bits, ucMask, ucClr, uc;
GFXfont font;
GFXglyph glyph, *pGlyph;
int iPitch, iRedOffset = 0;
 
   if (pOBD == NULL || pFont == NULL || pOBD->ucScreen == NULL)
      return -1;
    if (x == -1)
        x = pOBD->iCursorX;
    if (y == -1)
        y = pOBD->iCursorY;
    if (ucColor == OBD_RED) {
        // use the second half of the image buffer
        iRedOffset = pOBD->width * ((pOBD->height+7)/8);
    }
  if (pOBD->type == DISPLAY_COMMANDS) { // encode this as a command sequence
      uint8_t *d = pOBD->ucScreen;
      dx = (int)strlen(szMsg);
     // The font pointer is really the integer font index
      i = (int)pFont;
      if (i >= FONT_CUSTOM0) { // must be a valid index
          obdWriteCmdByte(pOBD, OBD_DRAWTEXT | ((ucColor & 1) << 7) | ((i & 7) << 4));
          obdWriteCmdByte(pOBD, (uint8_t) dx);
          obdWriteCmdInt(pOBD, x);
          obdWriteCmdInt(pOBD, y);
          i = pOBD->iScreenOffset;
          memcpy(&d[i], szMsg, dx);
          i += dx;
          pOBD->iScreenOffset = i; // store new length
      }
      return 0; // done
  }
   iPitch = pOBD->width;
   // in case of running on Harvard architecture, get copy of data from FLASH
   memcpy_P(&font, pFont, sizeof(font));
   pGlyph = &glyph;

   i = 0;
   while (szMsg[i] && x < pOBD->width)
   {
      c = szMsg[i++];
      if (c < font.first || c > font.last) // undefined character
         continue; // skip it
      c -= font.first; // first char of font defined
      memcpy_P(&glyph, &font.glyph[c], sizeof(glyph));
      dx = x + pGlyph->xOffset; // offset from character UL to start drawing
      dy = y + pGlyph->yOffset;
      s = font.bitmap + pGlyph->bitmapOffset; // start of bitmap data
      // Bitmap drawing loop. Image is MSB first and each pixel is packed next
      // to the next (continuing on to the next character line)
      iBitOff = 0; // bitmap offset (in bits)
      bits = uc = 0; // bits left in this font byte
      end_y = dy + pGlyph->height;
      if (dy < 0) { // skip these lines
          iBitOff += (pGlyph->width * (-dy));
          dy = 0;
      }
      for (ty=dy; ty<end_y && ty < pOBD->height; ty++) {
         ucMask = 1<<(ty & 7); // destination bit number for this line
         ucClr = (ucColor) ? ucMask : 0;
         d = &pOBD->ucScreen[iRedOffset + (ty >> 3) * iPitch + dx]; // internal buffer dest
         for (tx=0; tx<pGlyph->width; tx++) {
            if (uc == 0) { // need to read more font data
               tx += bits; // skip any remaining 0 bits
               uc = pgm_read_byte(&s[iBitOff>>3]); // get more font bitmap data
               bits = 8 - (iBitOff & 7); // we might not be on a byte boundary
               iBitOff += bits; // because of a clipped line
               uc <<= (8-bits);
               if (tx >= pGlyph->width) {
                  while(tx >= pGlyph->width && ty < end_y) { // rolls into next line(s)
                     tx -= pGlyph->width;
                     ty++;
                  }
                  if (ty >= end_y || ty >= pOBD->height) { // we're past the end
                     tx = pGlyph->width;
                     continue; // exit this character cleanly
                  }
                  // need to recalculate mask and offset in case Y changed
                  ucMask = 1<<(ty & 7); // destination bit number for this line
                  ucClr = (ucColor) ? ucMask : 0;
                  d = &pOBD->ucScreen[iRedOffset + (ty >> 3) * iPitch + dx]; // internal buffer dest
               }
            } // if we ran out of bits
            if (uc & 0x80 && (dx+tx) < pOBD->width) { // set pixel
               if (ucClr)
                  d[tx] |= ucMask;
               else
                  d[tx] &= ~ucMask;
            }
            bits--; // next bit
            uc <<= 1;
         } // for x
      } // for y
      x += pGlyph->xAdvance; // width of this character
   } // while drawing characters
    pOBD->iCursorX = x;
    pOBD->iCursorY = y;
   return 0;
} /* obdWriteStringCustom() */

//
// Render a sprite/rectangle of pixels from a provided buffer to the display.
// The row values refer to byte rows, not pixel rows due to the memory
// layout of OLEDs and LCDs.
// returns 0 for success, -1 for invalid parameter
//
int obdDrawGFX(OBDISP *pOBD, uint8_t *pBuffer, int iSrcCol, int iSrcRow, int iDestCol, int iDestRow, int iWidth, int iHeight, int iSrcPitch)
{
    int y;
    
    if (iSrcCol < 0 || iSrcCol >= pOBD->width || iSrcRow < 0 || iSrcRow > (pOBD->height/8)-1 || iDestCol < 0 || iDestCol >= pOBD->width || iDestRow < 0 || iDestRow >= (pOBD->height >> 3) || iSrcPitch <= 0)
        return -1; // invalid
    
    for (y=iSrcRow; y<iSrcRow+iHeight; y++)
    {
        uint8_t *s = &pBuffer[(y * iSrcPitch)+iSrcCol];
        obdSetPosition(pOBD, iDestCol, iDestRow, 1);
        obdWriteDataBlock(pOBD, s, iWidth, 1);
        pBuffer += iSrcPitch;
        iDestRow++;
    } // for y
    return 0;
} /* obdDrawGFX() */
//
// Fill the frame buffer with a byte pattern
// e.g. all off (0x00) or all on (0xff)
//
void obdFill(OBDISP *pOBD, unsigned char ucData, int bRender)
{
uint8_t y;
uint8_t iCols, iLines;

   if (pOBD == NULL) return;
   if (pOBD->type == DISPLAY_COMMANDS) { // encode this as a command sequence
       obdWriteCmdByte(pOBD, OBD_FILL);
       obdWriteCmdByte(pOBD, ucData);
       return;
   }

  pOBD->iCursorX = pOBD->iCursorY = 0;
  if (pOBD->type == LCD_VIRTUAL || pOBD->type >= SHARP_144x168) // pure memory, handle it differently
  {
      if (pOBD->ucScreen) {
          int iSize = pOBD->width * ((pOBD->height+7)/8);
          if (pOBD->iFlags & OBD_3COLOR) iSize *= 2;
        memset(pOBD->ucScreen, ucData, iSize);
      }
     return;
  }
  if (pOBD->iOrientation == 0 || pOBD->iOrientation == 180) {
    iLines = (pOBD->height+7) >> 3;
    iCols = pOBD->width;
  } else { // rotated
    iLines = (pOBD->width+7) >> 3;
    iCols = pOBD->height;
  }
  memset(u8Cache, ucData, iCols);
 
  if (bRender) { // write to the physical display if render = true
    for (y=0; y<iLines; y++)
    {
      obdSetPosition(pOBD, 0,y*8, bRender); // set to (0,Y)
      obdWriteDataBlock(pOBD, u8Cache, iCols, bRender);
    } // for y
  }
    if (pOBD->ucScreen) {
        memset(pOBD->ucScreen, ucData, (pOBD->width * pOBD->height)/8);
    }
} /* obdFill() */

//
// Provide or revoke a back buffer for your OLED graphics
// This allows you to manage the RAM used by ss_oled on tiny
// embedded platforms like the ATmega series
// Pass NULL to revoke the buffer. Make sure you provide a buffer
// large enough for your display (e.g. 128x64 needs 1K - 1024 bytes)
//
void obdSetBackBuffer(OBDISP *pOBD, uint8_t *pBuffer)
{
  if (pOBD == NULL || pBuffer == NULL) return;

  pOBD->ucScreen = pBuffer;
  pOBD->iScreenOffset = 0;
  if (pOBD->type >= LCD_COUNT) // invalid type, set to command output
    pOBD->type = DISPLAY_COMMANDS;
} /* obdSetBackBuffer() */

void obdAllocBuffer(OBDISP *pOBD)
{
  pOBD->ucScreen = (uint8_t *)malloc(pOBD->width * ((pOBD->height+7)>>3));
  pOBD->iScreenOffset = 0;
} /* obdAllocBuffer() */

void obdDrawLine(OBDISP *pOBD, int x1, int y1, int x2, int y2, uint8_t ucColor, int bRender)
{
  int temp;
  int dx = x2 - x1;
  int dy = y2 - y1;
  int error;
  uint8_t *p, *pStart, mask, bOld, bNew;
  int xinc, yinc;
  int y, x;
  int iPitch = pOBD->width;
  int iRedOffset = 0;

  if (pOBD == NULL) return;
  if (pOBD->type == DISPLAY_COMMANDS) { // encode this as a command sequence
      obdWriteCmdByte(pOBD, OBD_DRAWLINE | ((ucColor & 1) << 4));
      obdWriteCmdInt(pOBD, x1);
      obdWriteCmdInt(pOBD, y1);
      obdWriteCmdInt(pOBD, x2);
      obdWriteCmdInt(pOBD, y2);
      return; // done
  }

  if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 || x1 >= pOBD->width || x2 >= pOBD->width || y1 >= pOBD->height || y2 >= pOBD->height)
     return;
  if (ucColor == OBD_RED) {
        // use the second half of the image buffer
        iRedOffset = pOBD->width * ((pOBD->height+7)/8);
    }

  if(abs(dx) > abs(dy)) {
    // X major case
    if(x2 < x1) {
      dx = -dx;
      temp = x1;
      x1 = x2;
      x2 = temp;
      temp = y1;
      y1 = y2;
      y2 = temp;
    }

    y = y1;
    dy = (y2 - y1);
    error = dx >> 1;
    yinc = 1;
    if (dy < 0)
    {
      dy = -dy;
      yinc = -1;
    }
    p = pStart = &pOBD->ucScreen[iRedOffset + x1 + ((y >> 3) * iPitch)]; // point to current spot in back buffer
    mask = 1 << (y & 7); // current bit offset
    for(x=x1; x1 <= x2; x1++) {
      if (ucColor)
        *p++ |= mask; // set pixel and increment x pointer
      else
        *p++ &= ~mask;
      error -= dy;
      if (error < 0)
      {
        error += dx;
        if (yinc > 0)
           mask <<= 1;
        else
           mask >>= 1;
        if (mask == 0) // we've moved outside the current row, write the data we changed
        {
           obdSetPosition(pOBD, x, y, bRender);
           obdWriteDataBlock(pOBD, pStart,  (int)(p-pStart), bRender); // write the row we changed
           x = x1+1; // we've already written the byte at x1
           y1 = y+yinc;
           p += (yinc > 0) ? iPitch : -iPitch;
           pStart = p;
           mask = 1 << (y1 & 7);
        }
        y += yinc;
      }
    } // for x1
   if (p != pStart) // some data needs to be written
   {
     obdSetPosition(pOBD, x, y, bRender);
     obdWriteDataBlock(pOBD, pStart, (int)(p-pStart), bRender);
   }
  }
  else {
    // Y major case
    if(y1 > y2) {
      dy = -dy;
      temp = x1;
      x1 = x2;
      x2 = temp;
      temp = y1;
      y1 = y2;
      y2 = temp;
    }

    p = &pOBD->ucScreen[iRedOffset + x1 + ((y1 >> 3) * iPitch)]; // point to current spot in back buffer
    bOld = bNew = p[0]; // current data at that address
    mask = 1 << (y1 & 7); // current bit offset
    dx = (x2 - x1);
    error = dy >> 1;
    xinc = 1;
    if (dx < 0)
    {
      dx = -dx;
      xinc = -1;
    }
    for(x = x1; y1 <= y2; y1++) {
      if (ucColor)
        bNew |= mask; // set the pixel
      else
        bNew &= ~mask;
      error -= dx;
      mask <<= 1; // y1++
      if (mask == 0) // we're done with this byte, write it if necessary
      {
        if (bOld != bNew)
        {
          p[0] = bNew; // save to RAM
          obdSetPosition(pOBD, x, y1, bRender);
          obdWriteDataBlock(pOBD, &bNew, 1, bRender);
        }
        p += iPitch; // next line
        bOld = bNew = p[0];
        mask = 1; // start at LSB again
      }
      if (error < 0)
      {
        error += dy;
        if (bOld != bNew) // write the last byte we modified if it changed
        {
          p[0] = bNew; // save to RAM
          obdSetPosition(pOBD, x, y1, bRender);
          obdWriteDataBlock(pOBD, &bNew, 1, bRender);
        }
        p += xinc;
        x += xinc;
        bOld = bNew = p[0];
      }
    } // for y
    if (bOld != bNew) // write the last byte we modified if it changed
    {
      p[0] = bNew; // save to RAM
      obdSetPosition(pOBD, x, y2, bRender);
      obdWriteDataBlock(pOBD, &bNew, 1, bRender);
    }
  } // y major case
} /* obdDrawLine() */

//
// For drawing ellipses, a circle is drawn and the x and y pixels are scaled by a 16-bit integer fraction
// This function draws a single pixel and scales its position based on the x/y fraction of the ellipse
//
static void DrawScaledPixel(OBDISP *pOBD, int iCX, int iCY, int x, int y, int32_t iXFrac, int32_t iYFrac, uint8_t ucColor)
{
    uint8_t *d, ucMask;
    int iPitch;
    int iRedOffset = 0;

    iPitch = pOBD->width;
    if (ucColor == OBD_RED) {
          // use the second half of the image buffer
          iRedOffset = pOBD->width * ((pOBD->height+7)/8);
      }
    if (iXFrac != 0x10000) x = ((x * iXFrac) >> 16);
    if (iYFrac != 0x10000) y = ((y * iYFrac) >> 16);
    x += iCX; y += iCY;
    if (x < 0 || x >= pOBD->width || y < 0 || y >= pOBD->height)
        return; // off the screen
    d = &pOBD->ucScreen[iRedOffset + ((y >> 3)*iPitch) + x];
    ucMask = 1 << (y & 7);
    if (ucColor)
        *d |= ucMask;
    else
        *d &= ~ucMask;
} /* DrawScaledPixel() */
//
// For drawing filled ellipses
//
static void DrawScaledLine(OBDISP *pOBD, int iCX, int iCY, int x, int y, int32_t iXFrac, int32_t iYFrac, uint8_t ucColor)
{
    int iLen, x2;
    uint8_t *d, ucMask;
    int iPitch;
    int iRedOffset = 0;

    if (ucColor == OBD_RED) {
          // use the second half of the image buffer
          iRedOffset = pOBD->width * ((pOBD->height+7)/8);
      }
    iPitch = pOBD->width;
    if (iXFrac != 0x10000) x = ((x * iXFrac) >> 16);
    if (iYFrac != 0x10000) y = ((y * iYFrac) >> 16);
    iLen = x*2;
    x = iCX - x; y += iCY;
    x2 = x + iLen;
    if (y < 0 || y >= pOBD->height)
        return; // completely off the screen
    if (x < 0) x = 0;
    if (x2 >= pOBD->width) x2 = pOBD->width-1;
    iLen = x2 - x + 1; // new length
    d = &pOBD->ucScreen[iRedOffset + ((y >> 3)*iPitch) + x];
    ucMask = 1 << (y & 7);
    if (ucColor) // white
    {
        for (; iLen > 0; iLen--)
            *d++ |= ucMask;
    }
    else // black
    {
        for (; iLen > 0; iLen--)
            *d++ &= ~ucMask;
    }
} /* DrawScaledLine() */
//
// Draw the 8 pixels around the Bresenham circle
// (scaled to make an ellipse)
//
static void BresenhamCircle(OBDISP *pOBD, int iCX, int iCY, int x, int y, int32_t iXFrac, int32_t iYFrac, uint8_t ucColor, uint8_t bFill)
{
    if (bFill) // draw a filled ellipse
    {
        // for a filled ellipse, draw 4 lines instead of 8 pixels
        DrawScaledLine(pOBD, iCX, iCY, x, y, iXFrac, iYFrac, ucColor);
        DrawScaledLine(pOBD, iCX, iCY, x, -y, iXFrac, iYFrac, ucColor);
        DrawScaledLine(pOBD, iCX, iCY, y, x, iXFrac, iYFrac, ucColor);
        DrawScaledLine(pOBD, iCX, iCY, y, -x, iXFrac, iYFrac, ucColor);
    }
    else // draw 8 pixels around the edges
    {
        DrawScaledPixel(pOBD, iCX, iCY, x, y, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pOBD, iCX, iCY, -x, y, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pOBD, iCX, iCY, x, -y, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pOBD, iCX, iCY, -x, -y, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pOBD, iCX, iCY, y, x, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pOBD, iCX, iCY, -y, x, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pOBD, iCX, iCY, y, -x, iXFrac, iYFrac, ucColor);
        DrawScaledPixel(pOBD, iCX, iCY, -y, -x, iXFrac, iYFrac, ucColor);
    }
} /* BresenhamCircle() */

//
// Draw an outline or filled ellipse
//
void obdEllipse(OBDISP *pOBD, int iCenterX, int iCenterY, int32_t iRadiusX, int32_t iRadiusY, uint8_t ucColor, uint8_t bFilled)
{
    int32_t iXFrac, iYFrac;
    int iRadius, iDelta, x, y;
    
    if (pOBD == NULL || pOBD->ucScreen == NULL)
        return; // must have back buffer defined
  if (pOBD->type == DISPLAY_COMMANDS) { // encode this as a command sequence
      obdWriteCmdByte(pOBD, OBD_DRAWELLIPSE | ((ucColor & 1) << 4) | ((bFilled & 1) << 5));
      obdWriteCmdInt(pOBD, iCenterX);
      obdWriteCmdInt(pOBD, iCenterY);
      obdWriteCmdInt(pOBD, iRadiusX);
      obdWriteCmdInt(pOBD, iRadiusY);
      return; // done
  }

    if (iRadiusX <= 0 || iRadiusY <= 0) return; // invalid radii
    
    if (iRadiusX > iRadiusY) // use X as the primary radius
    {
        iRadius = iRadiusX;
        iXFrac = 65536;
        iYFrac = (iRadiusY * 65536) / iRadiusX;
    }
    else
    {
        iRadius = iRadiusY;
        iXFrac = (iRadiusX * 65536) / iRadiusY;
        iYFrac = 65536;
    }
    iDelta = 3 - (2 * iRadius);
    x = 0; y = iRadius;
    while (x <= y)
    {
        BresenhamCircle(pOBD, iCenterX, iCenterY, x, y, iXFrac, iYFrac, ucColor, bFilled);
        x++;
        if (iDelta < 0)
        {
            iDelta += (4*x) + 6;
        }
        else
        {
            iDelta += 4 * (x-y) + 10;
            y--;
        }
    }
} /* obdEllipse() */
//
// Draw an outline or filled rectangle
//
void obdRectangle(OBDISP *pOBD, int x1, int y1, int x2, int y2, uint8_t ucColor, uint8_t bFilled)
{
    uint8_t *d, ucMask, ucMask2;
    int tmp, iOff;
    int iPitch;
    int iRedOffset = 0;

    if (ucColor == OBD_RED) {
          // use the second half of the image buffer
          iRedOffset = pOBD->width * ((pOBD->height+7)/8);
      }

    if (pOBD == NULL || pOBD->ucScreen == NULL)
        return; // only works with a back buffer
  if (pOBD->type == DISPLAY_COMMANDS) { // encode this as a command sequence
      obdWriteCmdByte(pOBD, OBD_DRAWRECT | ((ucColor & 1) << 4) | ((bFilled & 1) << 5));
      obdWriteCmdInt(pOBD, x1);
      obdWriteCmdInt(pOBD, y1);
      obdWriteCmdInt(pOBD, x2);
      obdWriteCmdInt(pOBD, y2);
      return; // done
  }

    if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 ||
       x1 >= pOBD->width || y1 >= pOBD->height || x2 >= pOBD->width || y2 >= pOBD->height) return; // invalid coordinates
    iPitch = pOBD->width;
    // Make sure that X1/Y1 is above and to the left of X2/Y2
    // swap coordinates as needed to make this true
    if (x2 < x1)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y2 < y1)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    if (bFilled)
    {
        int x, y, iMiddle;
        iMiddle = (y2 >> 3) - (y1 >> 3);
        ucMask = 0xff << (y1 & 7);
        if (iMiddle == 0) // top and bottom lines are in the same row
            ucMask &= (0xff >> (7-(y2 & 7)));
        d = &pOBD->ucScreen[iRedOffset + (y1 >> 3)*iPitch + x1];
        // Draw top
        for (x = x1; x <= x2; x++)
        {
            if (ucColor)
                *d |= ucMask;
            else
                *d &= ~ucMask;
            d++;
        }
        if (iMiddle > 1) // need to draw middle part
        {
            ucMask = (ucColor) ? 0xff : 0x00;
            for (y=1; y<iMiddle; y++)
            {
                d = &pOBD->ucScreen[iRedOffset + (y1 >> 3)*iPitch + x1 + (y*iPitch)];
                for (x = x1; x <= x2; x++)
                    *d++ = ucMask;
            }
        }
        if (iMiddle >= 1) // need to draw bottom part
        {
            ucMask = 0xff >> (7-(y2 & 7));
            d = &pOBD->ucScreen[iRedOffset + (y2 >> 3)*iPitch + x1];
            for (x = x1; x <= x2; x++)
            {
                if (ucColor)
                    *d++ |= ucMask;
                else
                    *d++ &= ~ucMask;
            }
        }
    }
    else // outline
    {
      // see if top and bottom lines are within the same byte rows
        d = &pOBD->ucScreen[iRedOffset + (y1 >> 3)*iPitch + x1];
        if ((y1 >> 3) == (y2 >> 3))
        {
            ucMask2 = 0xff << (y1 & 7);  // L/R end masks
            ucMask = 1 << (y1 & 7);
            ucMask |= 1 << (y2 & 7);
            ucMask2 &= (0xff >> (7-(y2  & 7)));
            if (ucColor)
            {
                *d++ |= ucMask2; // start
                x1++;
                for (; x1 < x2; x1++)
                    *d++ |= ucMask;
                if (x1 <= x2)
                    *d++ |= ucMask2; // right edge
            }
            else
            {
                *d++ &= ~ucMask2;
                x1++;
                for (; x1 < x2; x1++)
                    *d++ &= ~ucMask;
                if (x1 <= x2)
                    *d++ &= ~ucMask2; // right edge
            }
        }
        else
        {
            int y;
            // L/R sides
            iOff = (x2 - x1);
            ucMask = 1 << (y1 & 7);
            for (y=y1; y <= y2; y++)
            {
                if (ucColor) {
                    *d |= ucMask;
                    d[iOff] |= ucMask;
                } else {
                    *d &= ~ucMask;
                    d[iOff] &= ~ucMask;
                }
                ucMask <<= 1;
                if  (ucMask == 0) {
                    ucMask = 1;
                    d += iPitch;
                }
            }
            // T/B sides
            ucMask = 1 << (y1 & 7);
            ucMask2 = 1 << (y2 & 7);
            x1++;
            d = &pOBD->ucScreen[iRedOffset + (y1 >> 3)*iPitch + x1];
            iOff = (y2 >> 3) - (y1 >> 3);
            iOff *= iPitch;
            for (; x1 < x2; x1++)
            {
                if (ucColor) {
                    *d |= ucMask;
                    d[iOff] |= ucMask2;
                } else {
                    *d &= ~ucMask;
                    d[iOff] &= ~ucMask2;
                }
                d++;
            }
        }
    } // outline
} /* obdRectangle() */
//
// Copy the current bitmap buffer from its native form (LSB_FIRST, VERTICAL_BYTES) to the requested form
// returns 0 for success, -1 for error
// Constants to be combined for the iFlags parameter:
// Output format options -
// OBD_LSB_FIRST     0x001
// OBD_MSB_FIRST     0x002
// OBD_VERT_BYTES    0x004
// OBD_HORZ_BYTES    0x008
// Orientation Options -
// OBD_ROTATE_90     0x010
// OBD_FLIP_VERT     0x020
// OBD_FLIP_HORZ     0x040
// Polarity Options -
// OBD_INVERT        0x080
int obdCopy(OBDISP *pOBD, int iFlags, uint8_t *pDestination)
{
int i, x, y, iPitch, iSize;
int xStart, xEnd, yStart, yEnd, yDst, xDst, dx, dy;
uint8_t ucSrcMask, ucDstMask, *s, *d;
    
    iPitch = pOBD->width;

    if (pDestination == NULL || pOBD == NULL || pOBD->ucScreen == NULL)
        return -1;
    // calculate output buffer size
    if (iFlags & OBD_HORZ_BYTES) {
        if (iFlags & OBD_ROTATE_90)
            iSize = ((pOBD->height + 7)>>3) * pOBD->width;
        else
            iSize = ((pOBD->width + 7)>>3) * pOBD->height;
    } else {
        if (iFlags & OBD_ROTATE_90)
            iSize = (pOBD->height * ((pOBD->width+7)>>3));
        else
            iSize = (pOBD->width * ((pOBD->height+7)>>3));
    }
    memset(pDestination, 0, iSize); // start with 0 in dest
    // Prepare vars for walking through the source image
    if (iFlags & OBD_ROTATE_90) {
        if (iFlags & OBD_FLIP_HORZ) {
            dy = 1;
            yStart = 0;
            yEnd = pOBD->height;
        } else {
            dy = -1;
            yStart = pOBD->height-1;
            yEnd = -1;
        }
        if (iFlags & OBD_FLIP_VERT) {
            dx = -1;
            xStart = pOBD->width-1;
            xEnd = -1;
        } else {
            dx = 1;
            xStart = 0;
            xEnd = pOBD->width;
        }
    } else { // not rotated
        if (iFlags & OBD_FLIP_HORZ) {
            dx = -1;
            xStart = pOBD->width-1;
            xEnd = -1;
        } else {
            dx = 1;
            xStart = 0;
            xEnd = pOBD->width;
        }
        if (iFlags & OBD_FLIP_VERT) {
            dy = -1;
            yStart = pOBD->height-1;
            yEnd = -1;
        } else {
            dy = 1;
            yStart = 0;
            yEnd = pOBD->height;
        }
    }
    // Due to the possible number of permutations, the different loops
    // are more generic and handle flips/bit-direction with a more general
    // approach which moves individual pixels even when a more efficient
    // method is possible. More cycles, but able to do EVERYTHING
// Separate output by byte orientation
    // Vertical bytes here
    if (iFlags & OBD_VERT_BYTES)
    {
        if (iFlags & OBD_ROTATE_90)
        {
            xDst = 0;
            for (y=yStart; y!=yEnd; y += dy, xDst++) {
                ucSrcMask = (1 << (y & 7));
                yDst = 0;
                s = &pOBD->ucScreen[(y >> 3) * iPitch];
                d = &pDestination[xDst];
                for (x=xStart; x!=xEnd; x += dx, yDst++) {
                    if (s[x] & ucSrcMask) {// set pixel, copy to dest
                        if (iFlags & OBD_LSB_FIRST)
                            d[(yDst >> 3)*pOBD->height] |= (1 << (yDst & 7));
                        else
                            d[(yDst >> 3)*pOBD->height] |= (0x80 >> (yDst & 7));
                    }
                } // for x
            } // for y
        } // rotate 90
        else // normally oriented
        {
            yDst = 0;
            for (y=yStart; y!=yEnd; y += dy, yDst++) {
                ucSrcMask = (1 << (y & 7));
                if (iFlags & OBD_LSB_FIRST)
                    ucDstMask = (1 << (y & 7));
                else
                    ucDstMask = (0x80 >> (y & 7));
                xDst = 0;
                s = &pOBD->ucScreen[(y >> 3) * iPitch];
                d = &pDestination[(yDst >> 3) * iPitch];
                for (x=xStart; x!=xEnd; x += dx, xDst++) {
                    if (s[x] & ucSrcMask) // set pixel, copy to dest
                        d[xDst] |= ucDstMask;
                } // for x
            } // for y
        } // normal orientation
    } // vertical bytes
    else // Horizontal bytes here
    {
       if (iFlags & OBD_ROTATE_90)
       {
           int iDstPitch = (pOBD->height + 7)>>3; // dest bytes per line
           xDst = 0;
           for (y=yStart; y!=yEnd; y += dy, xDst++) {
               ucSrcMask = (1 << (y & 7));
               yDst = 0;
               s = &pOBD->ucScreen[(y >> 3) * iPitch];
               d = &pDestination[xDst >> 3];
               ucDstMask = (iFlags & OBD_LSB_FIRST) ? (1 << (xDst & 7)) : (0x80 >> (xDst & 7));
               for (x=xStart; x!=xEnd; x += dx, yDst++) {
                   if (s[x] & ucSrcMask) // set pixel, copy to dest
                       d[yDst * iDstPitch] |= ucDstMask;
               } // for x
           } // for y
       } // rotate 90
       else // normally oriented
       {
           int iDstPitch = (pOBD->width + 7)>>3; // dest bytes per line
           yDst = 0;
           for (y=yStart; y!=yEnd; y += dy, yDst++) {
               ucSrcMask = (1 << (y & 7));
               xDst = 0;
               s = &pOBD->ucScreen[(y >> 3) * iPitch];
               d = &pDestination[yDst * iDstPitch];
               ucDstMask = (iFlags & OBD_LSB_FIRST) ? 0x1 : 0x80;
               for (x=xStart; x!=xEnd; x += dx, xDst++) {
                   if (s[x] & ucSrcMask) // set pixel, copy to dest
                       d[(xDst>>3)] |= ucDstMask;
                   if (iFlags & OBD_LSB_FIRST)
                       ucDstMask <<= 1;
                   else
                       ucDstMask >>= 1;
                   if (ucDstMask == 0)
                       ucDstMask = (iFlags & OBD_LSB_FIRST) ? 0x1 : 0x80;
               } // for x
           } // for y
       } // normal orientation
   }
    // Invert all pixels?
    if (iFlags & OBD_INVERT) {
        for (i=0; i<iSize; i++)
        pDestination[i] = 255 - pDestination[i];
    }
    return 0;
} /* obdCopy() */
