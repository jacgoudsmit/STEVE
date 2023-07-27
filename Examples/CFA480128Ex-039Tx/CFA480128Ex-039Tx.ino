/*
 Name:    CFA480128Ex_039Tx.ino
 Created: 2023-07-27 12:53:49
 Author:  Jac Goudsmit
*/

#include <SPI.h>
#include <Steve.h>
#include <SteveDisplay.h>
#include <SteveHAL_Arduino.h>

#include "BounceDemo.h"

// Hardware profile for the CrystalFontz CFA480128Ex-039Tx display
SteveDisplay cfa480128profile(
  480, 24, 11, 6, 521,  // Horizontal width,  front porch, sync width, back porch, padding
  128, 4, 1, 3, 1,  // Vertical   height, front porch, sync lines, back porch, padding
  7);                   // Pixel clock is 60 MHz / 7 = ~8.57 MHz

// Hardware abstraction layer for Arduino
SteveHAL_Arduino Hal(SPI, 8000000, 9, 8);

// Display
Steve d(cfa480128profile, Hal);

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
