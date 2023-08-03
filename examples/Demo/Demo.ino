/****************************************************************************
* Demo.ino
* (C) Jac Goudsmit
* MIT License.
****************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////////////////////////////

#include <Steve.h>
#include <SteveHAL_Arduino.h>

#include "BounceDemo.h"

/////////////////////////////////////////////////////////////////////////////
// KNOWN DISPLAY TYPES
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// These include files each declare a display type and an instance that can
// be used for all displays of that type.
//
// Include commands for display types that you don't use, can be
// commented out.
#include "SteveDisplay_CFA800480.h"
#include "SteveDisplay_CFA480128.h"

//---------------------------------------------------------------------------
// Hardware abstraction layer for Arduino
//
// If using multiple displays, you will need to create multiple
// HAL instances.
//
// For example if you have two displays on the same SPI bus, the other
// displays must use different pins for the select line and the power down
// line. The clock frequency depends on what your type of Arduino can do.
SteveHAL_Arduino Hal_SPI_9_8(SPI, 8000000, 9, 8);

//---------------------------------------------------------------------------
// Display
//
// This is where the display type and the HAL are used to create a display
// instance.
Steve d(CFA800480_DisplayProfile, Hal_SPI_9_8);

//---------------------------------------------------------------------------
// Demos
BounceDemo bounceDemo(d);

//---------------------------------------------------------------------------
// Initialization
void setup()
{
  SPI.begin();
  d.Begin();

  // Initialize the demos
  bounceDemo.Init();
}

//---------------------------------------------------------------------------
// Main program
void loop()
{
  // Wait for the next frame
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
