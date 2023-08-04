# streetlighting
Real Time Control System for Street Lighting

Developed a real-time control system for street lighting that enables:
* Turning street lights on/off as vehicles pass by;
* Managing light levels based on
    • ambient light(Day->off, Night->on);
    • velocity of the passing vehicle(higher velocity->stronger light; slower vehicles don't need as much light as faster ones);

Managing light levels in this way reduces power usage and light pollution.

Some problems solved:
* Vehicle position tracking
      • Counter system that tracks in which areas vehicles are
      • Limited to tracking movement in one direction
         (Extending in both directions can be done by swapping the counter with a timer, or by doubling 
          the sensors to understand the direction; further expanded in the thesis)
* Speed-based lighting
      • Measuring speed done by recording the time deltas between sensor activations
      • Can measure the speed of two vehicles simultaneously (as long as there's no overtaking)
      • Light intensity determined by the measured speed

Technology:
* Written in C++ (Arduino variant) 
* Built on top of Arduino Mega, connected to 5 modules with infrared (for motion sensing) and a light 
dependent resistor for ambient light measuring
* Real-time processing system, based on FreeRTOS, with guaranteed responding to an input signal
