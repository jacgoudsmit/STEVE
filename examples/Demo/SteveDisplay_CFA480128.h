// SteveDisplay_CFA480128.h

#ifndef _CFA480128_H
#define _CFA480128_H

//---------------------------------------------------------------------------
// Display profile for the CrystalFontz CFA480128Ex-039Tx display
//
// This single instance can be used for any number of displays.
SteveDisplay CFA480128_DisplayProfile(
  480, 24, 11, 6, 521,  // Horizontal width,  front porch, sync width, back porch, padding
  128, 4, 1, 3, 1,      // Vertical   height, front porch, sync lines, back porch, padding
  7);                   // Pixel clock is 60 MHz / 7 = ~8.57 MHz

#endif
