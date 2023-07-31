/****************************************************************************
SteveHAL_Windows.h
(C) 2023 Jac Goudsmit
MIT License.

This file declares a Windows-specific Hardware Abstraction Layer for Steve.
****************************************************************************/

#ifndef _STEVEHAL_WINDOWS_H
#define _STEVEHAL_WINDOWS_H

#ifdef _WIN32

/////////////////////////////////////////////////////////////////////////////
// INCLUDES
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "ftd2xx.h"
#include "libmpsse_spi.h"
#include "SteveHAL.h"

#pragma comment(lib, "libmpsse.lib")

/////////////////////////////////////////////////////////////////////////////
// STEVE HAL FOR WINDOWS
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// This implements a Steve Hardware Abstraction Layer for Windows using the
// MPSSE library from FTDI. It can be used with the C232HM-DDHSL-0
// cable (which uses the FTDI FT232H at 3.3V) to control a display directly
// from a Windows application.
//
// Other FTDI chip sets should work too such as the FT4222.
//
// When using a Crystalfontz CFA10098 evaluation/interface board, connect
// the C232HM-DDHSL-0 cable as shown in the CFA10098 manual:
// (1) VCC: Read Below!
// (2) GND: Read Below!
// (3) SCK: Orange
// (4) MOSI: Yellow
// (5) MISO: Green
// (6) GPIO0: N/C
// (7) GPIO1: N/C
// (8) GND: Black
// (9) !CS: Brown
// (10): !INT: Purple
// (11): !PD: Blue
// (12): GPIO2: N/C
// (13): GND: N/C
// Grey, White and possibly Red wires are unused.
// Note: The CrystalFontz display evaluation kits use the same wiring colors
// between the Arduino and the CFA10098 breakout board as the wires that
// are attached to the C232HM-DDHSL-0.
// 
// IMPORTANT: The Red wire from the C232HM-DDHSL-0 cable can be used on pin
// 1 to supply SOME of the displays that are available from CrystalFontz,
// such as the CFA480128 series, because they use 3.3V as power voltage and
// don't use much current. However for most devices (especially bigger
// displays), you should NOT connect the red wire to pin 1 of the CFA10098,
// but should supply the power some other way. For example, the CFA800480
// requires 5V (not 3.3V) and 128 mA which cannot be supplied by the
// C232HM-DDHSL-0 cable. Hint: Check out the CrystalFontz evaluation kit for
// your display of choice. If the evaluation kit has pin 1 connected to
// the 5V pin of the Arduino, you can't supply the display from the
// C232HM-DDHSL-0 cable.
// 
// Check the documentation of your display and the documentation of your
// USB-SPI cable for information about power requirements and capabilities.
// The author will not take responsibility for hardware that failed for
// any reason. See the LICENSE file.
class SteveHAL_Windows_MPSSE : public SteveHAL
{
protected:
  //-------------------------------------------------------------------------
  // Data
  DWORD             _channel;           // MPSSE channel to use
  UINT32            _clockRate;         // Clock frequency to use

  FT_HANDLE         _ftHandle;          // Handle to the channel

public:
  //-------------------------------------------------------------------------
  // Constructor
  SteveHAL_Windows_MPSSE(
    DWORD channel,                      // MPSSE channel to use
    UINT32 clockrate)                   // Clock frequency to use
  {
    _channel = channel;
    _clockRate = clockrate;

    _ftHandle = 0;
  }

protected:
  //-------------------------------------------------------------------------
  // Initialize the hardware
  virtual bool Begin() override         // Returns true if successful
  {
    bool result = false;

    if (!_ftHandle)
    {
      FT_DEVICE_LIST_INFO_NODE devList;
      DWORD u;
      DWORD channels;
      FT_STATUS status;

      Init_libMPSSE();

      status = SPI_GetNumChannels(&channels);
      for (u = 0; u < channels; u++)
      {
        status = SPI_GetChannelInfo(u, &devList);
        printf("SPI_GetNumChannels returned %u for channel %u\n", status, u);
        /*print the dev info*/
        printf("      VID/PID: 0x%04x/0x%04x\n", devList.ID >> 16, devList.ID & 0xffff);
        printf("      SerialNumber: %s\n", devList.SerialNumber);
        printf("      Description: %s\n", devList.Description);
      }

      if (_channel < channels)
      {
        status = SPI_OpenChannel(_channel, &_ftHandle);
        if (status != FT_OK)
        {
          fprintf(stderr, "Channel %u failed to open status %u\n", _channel, status);
        }
        else
        {
          result = true;
        }
      }
      else
      {
        fprintf(stderr, "Not enough channels found (wanted >%u got %u)\n", _channel, channels);
      }
    }

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Initialize the communication
  virtual void Init(
    bool slow = false) override         // True=use slow speed for early init
  {
    FT_STATUS status;

    if (slow)
    {
      UINT32 rate = _clockRate;
      if (slow && rate > 8000000)
      {
        rate = 8000000;
      }

      ChannelConfig channelConf;
      channelConf.ClockRate = rate;
      channelConf.LatencyTimer = 10;
      channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;

      status = SPI_InitChannel(_ftHandle, &channelConf);
      if (status != FT_OK)
      {
        fprintf(stderr, "Channel %u failed to initialize SPI status %u\n", _channel, status);
        exit(-3);
      }
    }
  }

protected:
  //-------------------------------------------------------------------------
  // Pause or resume communication
  virtual void Pause(
    bool pause) override                // True=pause, false=resume
  {
    //printf("Pause is not supported at this time\n");
  }

protected:
  //-------------------------------------------------------------------------
  // Turn the power on or off
  virtual void Power(
    bool enable) override               // True=on (!PD high) false=off/reset
  {
    // Temporarily change the CS output to DBUS7 (Blue)
    SPI_ChangeCS(_ftHandle, SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS7 | SPI_CONFIG_OPTION_CS_ACTIVELOW);

    // Change the pin
    SPI_ToggleCS(_ftHandle, !enable);

    // Change CS back to pin DBUS3 (Orange)
    SPI_ChangeCS(_ftHandle, SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW);
  }

protected:
  //-------------------------------------------------------------------------
  // Select or de-select the chip
  virtual bool Select(
    bool enable) override               // True=select (!CS low) false=de-sel
  {
    SPI_ToggleCS(_ftHandle, !!enable);

    return true;
  }

protected:
  //-------------------------------------------------------------------------
  // Transfer data to and from the EVE chip
  virtual uint8_t Transfer(             // Returns received byte
    uint8_t value) override             // Byte to send
  {
    UINT8 result;
    DWORD transferred;

    SPI_ReadWrite(_ftHandle, &result, &value , 1, &transferred, 0);
    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Wait for at least the requested time
  virtual void Delay(
    uint32_t ms) override               // Number of milliseconds to wait
  {
    Sleep(ms);
  }
};

/////////////////////////////////////////////////////////////////////////////
// END
/////////////////////////////////////////////////////////////////////////////

#endif
#endif
