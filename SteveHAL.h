/****************************************************************************
SteveHAL.h
(C) 2023 Jac Goudsmit
MIT License.

This file declares a Hardware Abstraction Layer for Steve.
****************************************************************************/

#pragma once

/////////////////////////////////////////////////////////////////////////////
// HARDWARE ABSTRACTION LAYER
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Hardware Abstraction Layer for Steve
//
// This abstract class provides the communication from the host to the 
// EVE chip, through SPI or QSPI.
//
// The functions in the class are only called by the Steve class which is
// declared as friend class.
//
// Platform-specific subclasses should implement the abstract virtual
// functions, and a constructor that calls the constructor in this class.
//
// Some default implementations of virtual functions can be overridden by
// subclasses; for example if a platform has an efficient way to send 4
// bytes at a time, its subclass can override the Send32 function.
class SteveHAL
{
  friend class Steve;

protected:
  //-------------------------------------------------------------------------
  // Constructor
  //
  // The constructor is protected so it can only be called by subclasses.
  SteveHAL()
  {
    // Nothing
  }

protected:
  //-------------------------------------------------------------------------
  // Initialize the communication
  //
  // This is called by Steve to initialize the communication with the EVE
  // chip.
  //
  // According to some documentation, in slow mode (until the EVE clock is
  // running, the SPI clock should run no faster than 11 MHz. After the EVE
  // chip is initialized, the SPI bus can use up to 30 MHz.
  virtual void Init(
    bool slow = false) = 0;             // True=use slow speed for early init

protected:
  //-------------------------------------------------------------------------
  // Pause or resume communication
  //
  // This is called by Steve to pause or resume communication to the
  // EVE chip.
  virtual void Pause(
    bool pause) = 0;                    // True=pause, false=resume

protected:
  //-------------------------------------------------------------------------
  // Turn the power on or off
  //
  // This is called by Steve to reset the chip as part of the initialization
  // sequence.
  //
  // NOTE: The pin is marked !PD (Power Down Not) so the pin is set to LOW
  // for a 'false' parameter, HIGH for 'true'.
  virtual void Power(
    bool enable) = 0;                   // True=on (!PD high) false=off/reset

protected:
  //-------------------------------------------------------------------------
  // Select or de-select the chip
  //
  // This is called by Steve to select or de-select the chip.
  //
  // The SPI interface on the EVE chips is not just used to let the chip
  // listen or ignore the data on the SPI bus, but also resets a sequencer
  // inside the chip that makes it start listening to host commands.
  // Some host commands initiate transfers of multiple bytes, and !CS needs
  // to stay active during the entire transfer.
  //
  // The HAL class keeps track of whether the call to this function actually
  // changed the state of the !CS line or not, and the return value is
  // used by the Steve class to make sure that the chip is the correct state.
  virtual bool                          // Returns true if !CS line changed
    Select(
      bool enable) = 0;                   // True=select (!CS low) false=de-sel

protected:
  //-------------------------------------------------------------------------
  // Transfer data to and from the EVE chip
  virtual uint8_t                       // Returns received byte
    Transfer(
      uint8_t value) = 0;                 // Byte to send

protected:
  //-------------------------------------------------------------------------
  // Send an 8-bit value
  virtual void Send8(
    uint8_t value)                      // Value to send
  {
    Transfer(value);
  }

protected:
  //-------------------------------------------------------------------------
  // Send a 16-bit value in little-endian format
  //
  // The least significant byte is sent first.
  virtual void Send16(
    uint16_t value)                     // Value to send
  {
    Transfer((uint8_t)(value));
    Transfer((uint8_t)(value >> 8));
  }

protected:
  //-------------------------------------------------------------------------
  // Send a 32-bit value in little-endian format
  //
  // The least significant byte is sent first.
  virtual void Send32(
    uint32_t value)                     // Value to send
  {
    Transfer((uint8_t)(value));
    Transfer((uint8_t)(value >> 8));
    Transfer((uint8_t)(value >> 16));
    Transfer((uint8_t)(value >> 24));
  }

protected:
  //-------------------------------------------------------------------------
  // Receive an 8-bit value
  virtual uint8_t                       // Returns incoming value
    Receive8()
  {
    return Transfer(0);
  }

protected:
  //-------------------------------------------------------------------------
  // Receive a 16-bit value in little-endian format
  //
  // The least significant byte is received first.
  virtual uint16_t                      // Returns incoming value
    Receive16()
  {
    uint16_t  result;

    result = (uint32_t)Transfer(0);
    result |= (uint32_t)Transfer(0) << 8;

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Receive a 32-bit value in little-endian format
  //
  // The least significant byte is received first.
  virtual uint32_t                      // Returns incoming value
    Receive32()
  {
    uint32_t  result;

    result = (uint32_t)Transfer(0);
    result |= (uint32_t)Transfer(0) << 8;
    result |= (uint32_t)Transfer(0) << 16;
    result |= (uint32_t)Transfer(0) << 24;

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Receive a buffer
  virtual uint32_t                      // Returns number of bytes received
    ReceiveBuffer(
      uint8_t *buffer,                    // Buffer to receive to
      uint32_t len)                       // Number of bytes to receive
  {
    uint32_t result;
    uint8_t *t = buffer;

    for (result = 0; result < len; result++)
    {
      *t++ = Receive8();
    }

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Send zero-bytes for alignment
  //
  // This takes a number of previously transmitted bytes and transmit the
  // required number of extra bytes to get the number to a multiple of 4.
  virtual uint32_t                      // Returns updated number bytes sent
    SendAlignmentBytes(
      uint32_t num)                       // Previous number of bytes sent
  {
    uint32_t result = num;

    while (result % 4)
    {
      Send8(0);
      result++;
    }

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Send data from a RAM buffer to the chip
  //
  // The function sends a block of data of the given size.
  virtual uint32_t                      // Returns number of bytes sent
    SendBuffer(
      const uint8_t *data,                // Data buffer to send
      uint32_t len)                       // Buffer length
  {
    uint32_t result;

    const uint8_t *p = data;
    for (result = 0; result < len; result++)
    {
      Send8(*p++);
    }

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Send a nul-terminated string
  //
  // The function reads a string from RAM, and transfers it to the EVE
  // It stops either when it finds the end of the source string, or when
  // it reaches the maximum length minus one. Then it sends a byte 0x00.
  //
  // The maximum length parameter includes the nul-terminator byte. If 0 is
  // used for the maximum length parameter, the value is interpreted as
  // "65536".
  //
  // If the pointer is NULL, an empty string is sent.
  virtual uint16_t                      // Returns number of bytes sent
    SendString(
      const char *message,                // Characters to send, '\0' is end
      uint16_t maxlen)                    // Max input length including \0
  {
    uint16_t result;
    const char *s = message;

    // Replace the pointer if it's NULL
    if (!s)
    {
      s = "";
    }

    // Send the non-nul characters. Note: if maxlen is 0, maxlen - 1
    // underflows to 65535.
    for (result = 0; result < maxlen - 1; result++)
    {
      char c = *s++;

      if (!c)
      {
        break;
      }

      Send8(c);
    }

    // Always send nul terminator byte
    Send8(0);
    result += 1;

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Wait for at least the requested time
  virtual void
    Delay(
      uint32_t ms) = 0;                   // Number of milliseconds to wait
};

/////////////////////////////////////////////////////////////////////////////
// END
/////////////////////////////////////////////////////////////////////////////
