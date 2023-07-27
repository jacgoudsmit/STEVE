/****************************************************************************
SteveDisplay.h
(C) 2023 Jac Goudsmit
MIT License.

This file declares a struct for storing display parameters.
****************************************************************************/

#pragma once

/////////////////////////////////////////////////////////////////////////////
// STRUCT FOR DISPLAY PARAMETERS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// This struct is used to describe the hardware parameters for a 
// particular LCD display panel.
struct SteveDisplay
{
public:
  //-------------------------------------------------------------------------
  // Parameter values to set the pin driving strength (HOSTCMD_PINDRIVE)
  enum PINDRIVE_STRENGTH // 2 bits
  {
    // [DS2 p18][DS3 p17][DS4 p16]
    PINDRIVE_STRENGTH_LOW               = 0x00,         // 5 mA  (EVE3/EVE4: 1.2 mA for some pins)
    PINDRIVE_STRENGTH_MEDIUM            = 0x01,         // 10 mA (EVE3/EVE4: 2.4 mA for some pins)
    PINDRIVE_STRENGTH_HIGH              = 0x02,         // 15 mA (EVE3/EVE4: 3.6 mA for some pins)
    PINDRIVE_STRENGTH_MAXIMUM           = 0x03,         // 20 mA (EVE3/EVE4: 4.8 mA for some pins)
  };

public:
  //-------------------------------------------------------------------------
  // Parameter values for pins to apply drive strength or power down state
  // (HOSTCMD_PINDRIVE and HOSTCMD_PD_STATE)
  enum PINS // 6 bits
  {
    // [DS2 p18][DS3 p16][DS4 p15]
    PINS_GPIO0                          = 0x00,
    PINS_GPIO1                          = 0x01,
    PINS_GPIO2                          = 0x02,
    PINS_GPIO3                          = 0x03,
    //0x04-0x07 reserved
    PINS_DISP                           = 0x08,
    PINS_DE                             = 0x09,
    PINS_VSYNC_HSYNC                    = 0x0A,
    PINS_PCLK                           = 0x0B,
    PINS_BACKLIGHT                      = 0x0C,
    PINS_RGB                            = 0x0D,
    PINS_AUDIO_L                        = 0x0E,
    PINS_INT_N                          = 0x0F,
    PINS_CTP_RST_N                      = 0x10,
    PINS_CTP_SCL                        = 0x11,
    PINS_CTP_SDA                        = 0x12,
    PINS_SPI                            = 0x13,

    // (EVE3/EVE4 only)
    PINS_SPIM_SCLK                      = 0x14,
    PINS_SPIM_SS_N                      = 0x15,
    PINS_SPIM_MISO                      = 0x16,
    PINS_SPIM_MOSI                      = 0x17,
    PINS_SPIM_IO2                       = 0x18,
    PINS_SPIM_IO3                       = 0x19,
  };

public:
  //-------------------------------------------------------------------------
  // Parameter values for setting power-down pin state (HOSTCMD_PD_STATE)
  enum PD_STATE
  {
    // [DS2 p18][DS3 p18][DS4 p17]
    PD_STATE_FLOAT                      = 0x0,          // Float the pin
    PD_STATE_PULL_DOWN                  = 0x1,          // Pull the pin down
    PD_STATE_PULL_UP                    = 0x2,          // Pull the pin up
  };

public:
  //-------------------------------------------------------------------------
  // Parameter values for HOSTCMD_CLKSEL
  enum CLKSEL // 8 bits
  {
    // [DS2 p17][DS3 p16][DS4 p15]
    CLKSEL_DEFAULT                    = 0x00,         // Default for EVE1 compatibility (60 MHz)
    CLKSEL_X2                         = 0x02,         // 2x multiplier (24 MHz)
    CLKSEL_X3                         = 0x03,         // 3x multiplier (36 MHz)
    CLKSEL_X4                         = 0x44,         // 4x multiplier and high PLL range (48 MHz)
    CLKSEL_X5                         = 0x45,         // 5x multiplier and high PLL range (60 MHz)
    CLKSEL_X6                         = 0x46,         // (EVE3/EVE4) 6x multiplier and high PLL range (72 MHz)
    CLKSEL_X7                         = 0x47,         // (UNDOCUMENTED) 7x multiplier and high PLL range (84 MHz)
  };


public:
  //-------------------------------------------------------------------------
  // Chip identifiers
  //
  // [DS2 p46][DS3 p47][DS4 p45]
  enum CHIPID
  {
    // Use this value in the init parameters to skip chip ID checking
    // (Not recommended)
    CHIPID_ANY                        = 0,

    // Following are values in the chip ID register just after the
    // processor has been started.
    CHIPID_FT810                      = 0x00011008,
    CHIPID_FT811                      = 0x00011108,
    CHIPID_FT812                      = 0x00011208,
    CHIPID_FT813                      = 0x00011308,
    CHIPID_BT815                      = 0x00011508,
    CHIPID_BT816                      = 0x00011608,
    CHIPID_BT817                      = 0x00011708,
    CHIPID_BT818                      = 0x00011808
  };

public:
  //-------------------------------------------------------------------------
  // Data
  bool            _clkext;            // True=external clock
  CLKSEL          _clksel;            // Clock multiplier
  CHIPID          _chipid;            // Expected chip ID; ANY=don't care
  uint32_t        _frequency;         // ClockFreq to store; 0 = don't store
  bool            _lcd10ma;           // True=drive LCD with 10mA (false=5)
  bool            _cspread;           // True=enable RGB clock spreading, see datasheet 4.4 p.27
  bool            _dither;            // True=enable dither, see datasheet 4.4 p.27
  uint16_t        _outbits;           // 3x3 bits indicating num LCD bits used, see datasheet 4.4 p.27

  uint16_t        _hsize;             // active display width
  uint16_t        _hcycle;            // total number of clocks per line, incl front/back porch
  uint16_t        _hoffset;           // start of active line
  uint16_t        _hsync0;            // start of horizontal sync pulse
  uint16_t        _hsync1;            // end of horizontal sync pulse

  uint16_t        _vsize;             // active display height
  uint16_t        _vcycle;            // total number of lines per screen, incl pre/post
  uint16_t        _voffset;           // start of active screen
  uint16_t        _vsync0;            // start of vertical sync pulse
  uint16_t        _vsync1;            // end of vertical sync pulse

  uint8_t         _swizzle;           // FT800 output to LCD - pin order
  uint8_t         _pclkpol;           // LCD data is clocked in on this PCLK edge
  uint8_t         _pclk;              // Clock divisor

  const uint8_t  *_pindrivetable;     // Pin drive specifications (NULL=none)

  // Macros to generate an const static array in a sort-of type-safe way
  // (and without a lot of distracting scope prefixes) and assign the
  // pin drive table to it.
  // Display profiles that need this, should use the macros in their
  // constructor.
  #define STEVE_USE_PINDRIVE_TABLE_BEGIN const static uint8_t Steve__pindrive_table[] = {
  #define STEVE_USE_PINDRIVE_TABLE_END 0xFF }; _pindrivetable = Steve__pindrive_table;
  #define STEVE_PINDRIVE(pins, level) ((SteveDisplay::PINS_##pins << 2) | (PINDRIVE_STRENGTH_##level)),

public:
  //-------------------------------------------------------------------------
  // Constructor
  //
  // This generates some of the timing values based on the given
  // parameters. 
  SteveDisplay(
    uint16_t width,                   // Horizontal number of pixels
    uint16_t hfrontporch,             // Num clocks from display to sync
    uint16_t hsyncwidth,              // Number of clocks in hsync
    uint16_t hbackporch,              // Num clocks from hsync to display
    uint16_t hpadding,                // Num additional clocks per line
    uint16_t height,                  // Vertical number of pixels
    uint16_t vfrontporch,             // Num lines from display to vsync
    uint16_t vsyncheight,             // Number of lines in vsync
    uint16_t vbackporch,              // Num lines from vsync to display
    uint16_t vpadding,                // Num additional lines per frame
    uint8_t pclk,                     // Clock divisor
    uint8_t pclkpol = 1,              // Clock policy
    uint8_t swizzle = 0)              // Pin order
    : _clkext(false)
    , _clksel(CLKSEL_DEFAULT)
    , _chipid(CHIPID_ANY)
    , _frequency(0)
    , _lcd10ma(false)
    , _cspread(false)
    , _dither(false)
    , _outbits(0)
    , _hsize(width)
    , _hcycle(hfrontporch + hsyncwidth + hbackporch + width + hpadding)
    , _hoffset(hfrontporch + hsyncwidth + hbackporch)
    , _hsync0(hfrontporch)
    , _hsync1(hfrontporch + hsyncwidth)
    , _vsize(height)
    , _vcycle(vfrontporch + vsyncheight + vbackporch + height + vpadding)
    , _voffset(vfrontporch + vsyncheight + vbackporch)
    , _vsync0(vfrontporch)
    , _vsync1(vfrontporch + vsyncheight)
    , _swizzle(swizzle)
    , _pclkpol(pclkpol)
    , _pclk(pclk)
    , _pindrivetable(NULL)
  {
    // Nothing
  }
};

