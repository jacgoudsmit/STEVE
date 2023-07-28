/*
 Name:    CFA800480E3050Sx.ino
 Created: 2023-07-27 14:34:19
 Author:  Jac Goudsmit
*/

#include <Steve.h>
#include <SteveHAL_Arduino.h>

#include "BounceDemo.h"

// Display profile for CrystalFontz CFA800480E3050Sx display
class CFA800480profile : public SteveDisplay
{
public:
  CFA800480profile()
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
} cfa800480profile;


// Hardware abstraction layer for Arduino
SteveHAL_Arduino Hal(SPI, 8000000, 9, 8);

// Display
Steve d(cfa800480profile, Hal);

// Demos
BounceDemo bounceDemo(d);

//---------------------------------------------------------------------------
// Initialization
void setup() {
  SPI.begin();
  d.Begin();

  // Initialize the demos
  bounceDemo.Init();
}

//---------------------------------------------------------------------------
// Main program
void loop() {
  d.CmdWaitComplete();

  // Start the command list
  d.cmd_DLSTART();

  // Clear the screen (and clear the current color, stencil and tag)
  d.CmdClear(0, 0, 0);

  // Add commands for the demos
  bounceDemo.AddCommands();

  // Cycle all demos
  bounceDemo.Cycle();

  // Instruct graphics processor to show the list
  d.CmdDlFinish();
}
