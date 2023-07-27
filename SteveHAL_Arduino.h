/****************************************************************************
SteveHAL_Arduino.h
(C) 2023 Jac Goudsmit
MIT License.

This file declares an Arduino-specific Hardware Abstraction Layer for Steve.
****************************************************************************/

#pragma once

#ifdef ARDUINO

/////////////////////////////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////////////////////////////

#include <SPI.h>

#include "SteveHAL.h"

/////////////////////////////////////////////////////////////////////////////
// MACROS
/////////////////////////////////////////////////////////////////////////////

// Redefine these macros for debugging. They are called with printf-like
// parameters.
#ifndef DBG_TRAFFIC
#define DBG_TRAFFIC(...)
#endif
#ifndef DBG_GEEK
#define DBG_GEEK(...)
#endif
#ifndef DBG_STAT
#define DBG_STAT(...)
#endif

/////////////////////////////////////////////////////////////////////////////
// HARDWARE ABSTRACTION LAYER SPECIFIC TO ARDUINO
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Minimal Hardware Abstraction Layer for Arduino
//
// This should be compatible with basically all variations of Arduino:
// * It uses a single SPI port (not dual SPI or quad SPI).
// * There are no optimizations for sending and receiving multiple successive
//   bytes.
// * No interrupts or DMA are used.
// * Only a single SPI clock speed is used (up to 8 MHz). The speed is not
//   switched to a higher frequency once the EVE is ready for it.
class SteveHAL_Arduino : public SteveHAL
{
private:
  //-------------------------------------------------------------------------
  // Constants and initialization parameters

  SPIClass &_spi;               // SPI instance
  const SPISettings _spi_settings;      // SPI settings
  const int         _pin_cs;            // Chip Select Not Pin
  const int         _pin_pd;            // Power Down Not Pin

private:
  //-------------------------------------------------------------------------
  // State data

  bool              _selected;          // True if chip currently selected

public:
  //-------------------------------------------------------------------------
  // Constructor
  SteveHAL_Arduino(
    SPIClass &spi,                      // SPI port
    uint32_t spi_clock,                 // SPI clock speed
    int pin_cs,                         // !CS pin
    int pin_pd)                         // !PD pin
    : SteveHAL()
    , _spi(spi)
    , _spi_settings(spi_clock, MSBFIRST, SPI_MODE0)
    , _pin_cs(pin_cs)
    , _pin_pd(pin_pd)
  {
    // Set the output pins before switching the pins to output, to
    // avoid glitches
    _selected = true; // Make sure the CS pin is changed next
    Select(false); // De-select
    Power(true); // Power on

    // Configure the Power Down Not pin; it's also used as reset.
    // This will power up the panel.
    if (_pin_pd >= 0)
    {
      pinMode(_pin_pd, OUTPUT);
    }

    // Finally configure the chip select pin
    if (_pin_cs >= 0)
    {
      pinMode(_pin_cs, OUTPUT);
    }
  }

protected:
  //-------------------------------------------------------------------------
  // Initialize the communication
  void Init(
    bool slow = false) override         // True=use slow speed for early init
  {
    (void)slow; // Ignored

    DBG_TRAFFIC("beginTransaction\n");
    _spi.beginTransaction(_spi_settings);
  }

protected:
  //-------------------------------------------------------------------------
  // Pause or resume communication
  void Pause(
    bool pause) override                // True=pause, false=resume
  {
    if (pause)
    {
      DBG_TRAFFIC("endTransaction\n");
      _spi.endTransaction();
    }
    else
    {
      Init();
    }
  }

protected:
  //-------------------------------------------------------------------------
  // Turn the power on or off
  void Power(
    bool enable) override               // True=on (!PD high) false=off/reset
  {
    // Set the pin HIGH to power up
    digitalWrite(_pin_pd, enable ? HIGH : LOW);
  }

protected:
  //-------------------------------------------------------------------------
  // Select or de-select the chip
  bool                                  // Returns true if !CS line changed
    Select(
      bool enable) override               // True=select (!CS low) false=de-sel
  {
    bool result = (enable != _selected);

    if (result)
    {
      _selected = enable;

      DBG_TRAFFIC("Select %u\n", !!enable);

      // Set the pin LOW to enable the chip
      digitalWrite(_pin_cs, enable ? LOW : HIGH);
    }

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Transfer data to and from the EVE chip
  virtual uint8_t                       // Returns received byte
    Transfer(
      uint8_t value) override             // Byte to send
  {
    return _spi.transfer(value);
  }

protected:
  //-------------------------------------------------------------------------
  // Wait for at least the requested time
  virtual void
    Delay(
      uint32_t ms) override               // Number of milliseconds to wait
  {
    delay(ms);
  }
};

#endif // ARDUINO

/////////////////////////////////////////////////////////////////////////////
// END
/////////////////////////////////////////////////////////////////////////////
