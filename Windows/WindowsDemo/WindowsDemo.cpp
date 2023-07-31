/****************************************************************************
* Demo.ino
* (C) Jac Goudsmit
* MIT License.
****************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <Windows.h>

#include "Steve.h"
#include "SteveHAL_Windows_MPSSE.h"

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
// Hardware abstraction layer for Windows
//
// If using multiple displays, you will need to create multiple
// HAL instances.
SteveHAL_Windows_MPSSE Hal_Channel0(0, 8000000);

//---------------------------------------------------------------------------
// Display
//
// This is where the display type and the HAL are used to create a display
// instance.
Steve d(CFA480128_DisplayProfile, Hal_Channel0);

//---------------------------------------------------------------------------
// Demos
BounceDemo bounceDemo(d);

//---------------------------------------------------------------------------
// Main program
int main(int argc, char **argv)
{
  if (!d.Begin())
  {
    fprintf(stderr, "Begin failed\n");
    exit(-1);
  }

  // Initialize the demos
  bounceDemo.Init();

  for (;;)
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

  // Unreachable code
  return 0;
}
