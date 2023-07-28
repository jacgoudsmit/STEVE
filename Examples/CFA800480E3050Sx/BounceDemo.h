// BounceDemo.h

#ifndef _BOUNCEDEMO_h
#define _BOUNCEDEMO_h

// #if defined(ARDUINO) && ARDUINO >= 100
//   #include "arduino.h"
// #else
//   #include "WProgram.h"
// #endif

class BounceDemo
{
private:
  Steve &eve;

  int32_t         _x_position;
  int32_t         _x_velocity;
  int32_t         _y_position;
  int32_t         _y_velocity;
  int32_t         _ball_size;
  int32_t         _ball_delta;
  //Cycle the color around
  uint8_t         _r;
  uint8_t         _g;
  uint8_t         _b;
  uint8_t         _transparency;
  uint8_t         _transparency_direction;

public:
  //-----------------------------------------------------------------------
  // Constructor
  BounceDemo(
    Steve &steve)
    : eve(steve)
  {
    // Nothing
  }

public:
  //-----------------------------------------------------------------------
  // Initialize the demo
  //
  // This should be called in the setup() function
  void Init()
  {
    //Choose some starting color
    _r = 0xff;
    _g = 0x00;
    _b = 0x80;

    //Start ghostly, getting more solid.
    _transparency = 0;
    _transparency_direction = 1;

    // Define a default point x-location (1/16 anti-aliased)
    _x_position = (eve.HCenter() * 16);
    _x_velocity = 3 * 16;

    // Define a default point y-location (1/16 anti-aliased)
    _y_position = (eve.VCenter() * 16);
    _y_velocity = -2 * 16;

    //Start small.
    _ball_size = 50 * 1;
    _ball_delta = 1 * 16;
  }

public:
  //-----------------------------------------------------------------------
  // Add commands to the list
  //
  // This should be called in the loop() function
  void AddCommands()
  {
    // Set the variable color of the bouncing ball
    eve.CmdColor(_r, _g, _b);

    // Make it transparent
    eve.CmdAlpha(_transparency);

    // Draw the ball -- a point (filled circle)
    eve.Point(_x_position, _y_position, _ball_size);
    //eve.cmd_TEXT(_x_position / 16, _y_position / 16, 31, eve.OPT_CENTER, "StEVE");

    //========== RUBBER BAND TETHER ==========
    // Draw the rubber band.
    // Maximum stretched would be LCD_WIDTH/2 + LCD_WIDTH/2
    // Make the minimum 10 pixels wide
    uint16_t rubberband_width;
    uint16_t x_distance;

    if ((_x_position / 16) < (int32_t)eve.HCenter())
    {
      x_distance = eve.HCenter() - (_x_position / 16);
    }
    else
    {
      x_distance = (_x_position / 16) - eve.HCenter();
    }

    uint16_t y_distance;
    if ((_y_position / 16) < (int32_t)eve.VCenter())
    {
      y_distance = eve.VCenter() - (_y_position / 16);
    }
    else
    {
      y_distance = (_y_position / 16) - eve.VCenter();
    }

    // Straight math does not make it skinny enough. This seems like it should
    // underflow often, but in real life never goes below 1. Need to dissect.
    rubberband_width = 10 - ((9 + 1) * (x_distance + y_distance)) / (eve.HCenter() + eve.VCenter());

    // Check for underflow just in case.
    if (rubberband_width & 0x8000)
    {
      rubberband_width = 1;
    }

    // Now that we know the rubber band width, drawing it is simple.
    eve.CmdColor(200, 0, 0);

    // (transparency set above still in effect)
    eve.Line(eve.HCenter() * 16, eve.VCenter() * 16, _x_position, _y_position, rubberband_width * 16);
  }

public:
  //-----------------------------------------------------------------------
  // Modify the internal variables for the next step
  void Cycle(void)
  {
    // Update the colors
    _r++;
    _g--;
    _b += 2;

    // Cycle the transparency
    if (_transparency_direction)
    {
      //Getting more solid
      if (_transparency != 255)
      {
        _transparency++;
      }
      else
      {
        _transparency_direction = 0;
      }
    }
    else
    {
      //Getting more clear
      if (128 < _transparency)
      {
        _transparency--;
      }
      else
      {
        _transparency_direction = 1;
      }
    }

    //========== BOUNCE THE BALL AROUND ==========

#define MIN_POINT_SIZE (10*16)
#define MAX_POINT_SIZE (int32_t)((eve.VCenter() - 20) * 16)

// Change the point (ball) size.
    if (_ball_delta < 0)
    {
      // Getting smaller. OK to decrease again?
      if (MIN_POINT_SIZE < (_ball_size + _ball_delta))
      {
        // It will be bigger than min after decrease
        _ball_size += _ball_delta;
      }
      else
      {
        // It would be too small, bounce.
        _ball_size = MIN_POINT_SIZE + (MIN_POINT_SIZE - (_ball_size + _ball_delta));

        //Turn around.
        _ball_delta = -_ball_delta;
      }
    }
    else
    {
      // Getting larger. OK to increase again?
      if ((_ball_size + _ball_delta) < MAX_POINT_SIZE)
      {
        // It will be smaller than max after increase
        _ball_size += _ball_delta;
      }
      else
      {
        // It would be too big, bounce.
        _ball_size = MAX_POINT_SIZE - (MAX_POINT_SIZE - (_ball_size + _ball_delta));

        // Turn around.
        _ball_delta = -_ball_delta;
      }
    }

    // Move X, bouncing
    if (_x_velocity < 0)
    {
      // Going left. OK to move again?
      if (0 < (_x_position - (_ball_size)+_x_velocity))
      {
        // It will be on-screen after decrease
        _x_position += _x_velocity;
      }
      else
      {
        // It would be too small, bounce.
        _x_position = _ball_size + (_ball_size - (_x_position + _x_velocity));

        // Turn around.
        _x_velocity = -_x_velocity;
      }
    }
    else
    {
      // Getting larger. OK to increase again?
      if ((_x_position + (_ball_size)+_x_velocity) < (int32_t)eve.Width() * 16)
      {
        // It will be on screen after increase
        _x_position += _x_velocity;
      }
      else
      {
        // It would be too big, bounce.
        int32_t max_x_ctr = (int32_t)eve.Width() * 16 - _ball_size;

        _x_position = max_x_ctr - (max_x_ctr - (_x_position + _x_velocity));

        // Turn around.
        _x_velocity = -_x_velocity;
      }
    }

    // Move Y, bouncing
    if (_y_velocity < 0)
    {
      // Going left. OK to move again?
      if (0 < (_y_position - (_ball_size)+_y_velocity))
      {
        // It will be on-screen after decrease
        _y_position += _y_velocity;
      }
      else
      {
        // It would be too small, bounce.
        _y_position = _ball_size + (_ball_size - (_y_position + _y_velocity));

        // Turn around.
        _y_velocity = -_y_velocity;
      }
    }
    else
    {
      // Getting larger. OK to increase again?
      if ((_y_position + (_ball_size)+_y_velocity) < (int32_t)eve.Height() * 16)
      {
        // It will be on screen after increase
        _y_position += _y_velocity;
      }
      else
      {
        // It would be too big, bounce.
        int32_t max_y_ctr = (int32_t)eve.Width() * 16 - _ball_size;
        _y_position = max_y_ctr - (max_y_ctr - (_y_position + _y_velocity));

        // Turn around.
        _y_velocity = -_y_velocity;
      }
    }
  }
};


#endif
