// CFA800480.h

#ifndef _CFA800480_H
#define _CFA800480_H

//---------------------------------------------------------------------------
// Display profile for CrystalFontz CFA800480E3050Sx display
class SteveDisplay_CFA800480 : public SteveDisplay
{
public:
  SteveDisplay_CFA800480()
    : SteveDisplay(
      800, 8, 4, 8, 178,  // Width,  front porch, sync width, back porch, padding
      480, 8, 4, 8, 1,    // Height, front porch, sync lines, back porch, padding
      2)                  // PCLK divider (72 MHz / 2 = 36 MHz)
  {
    STEVE_USE_PINDRIVE_TABLE_BEGIN
      STEVE_PINDRIVE(GPIO0,       LOW)
      STEVE_PINDRIVE(GPIO1,       LOW)
      STEVE_PINDRIVE(GPIO2,       LOW)
      STEVE_PINDRIVE(GPIO3,       LOW)
      STEVE_PINDRIVE(DISP,        LOW)
      STEVE_PINDRIVE(DE,          LOW)
      STEVE_PINDRIVE(VSYNC_HSYNC, LOW)
      STEVE_PINDRIVE(PCLK,        HIGH)
      STEVE_PINDRIVE(BACKLIGHT,   LOW)
      STEVE_PINDRIVE(RGB,         LOW)
      STEVE_PINDRIVE(AUDIO_L,     LOW)
      STEVE_PINDRIVE(INT_N,       LOW)
      STEVE_PINDRIVE(CTP_RST_N,   LOW)
      STEVE_PINDRIVE(CTP_SCL,     LOW)
      STEVE_PINDRIVE(CTP_SDA,     LOW)
      STEVE_PINDRIVE(SPI,         LOW)
      STEVE_PINDRIVE(SPIM_SCLK,   MEDIUM)
      STEVE_PINDRIVE(SPIM_SS_N,   LOW)
      STEVE_PINDRIVE(SPIM_MISO,   LOW)
      STEVE_PINDRIVE(SPIM_MOSI,   LOW)
      STEVE_PINDRIVE(SPIM_IO2,    LOW)
      STEVE_PINDRIVE(SPIM_IO3,    LOW)
    STEVE_USE_PINDRIVE_TABLE_END

    _chipid = CHIPID_BT817;
    _clksel = CLKSEL_X6;
    _frequency = 72000000;
  }
};

//---------------------------------------------------------------------------
// This single instance can be used for any number of displays.
SteveDisplay_CFA800480 CFA800480_DisplayProfile;

#endif
