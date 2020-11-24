/* mbed TEC controller

Copyright 2020 Jonathan L. Martin <jon.martini@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

 */

#ifndef MBED_TEC_H
#define MBED_TEC_H

#include "mbed.h"

/** Interface to control a Thermo Electric Cooler Peltier Device connected
 * to an H-bridge. Allows both cooling and heating.  You should monitor temperatures
 * to avoid burning out the TEC.
 *
 * TECs work as a heat pump by creating a temperature delta between sides.
 * You need to take away the heat generated into a heat sink or radiator in order
 * to see a cooling effect.
 *
 *
 * Example:
 * @code
 * //Initialize and control a TEC connected to an H-bridge
 * #include "mbed.h"
 * #include "TEC.h"
 * 
 * TEC TEC1(p21, p6,  p5);  // PWM Enable pin, cool pin, heat pin
 * 
 * int main() {
 *     float TecPowerPercent = 0.0;
 *     TEC::TecAction Climate = TEC::Cooling;
 *     while(1) {
 *         TEC1.setClimate
 *          (Climate,
 *           TecPowerPercent);
 *         if (TecPowerPercent < 100.0)
 *         {
 *             TecPowerPercent += 10.0;
 *         }
 *         wait(1.0);
 *     }
 * }
 * @endcode
 */
class TEC {
public:

    /** Create a TEC control interface    
     *
     * @param ena_pin This pin connected to ENA on the H-bridge.  Must be a pwm pin.
     * @param cool_pin - IN1 on H-bridge.  When set to 1 the TEC will cool if heat_pin is set to 0.
     * @param heat_pin - IN2 on H-bridge.  When set to 1 the TEC will heat if cool_pin is set to 0.
     *
     * cool_pin   heat_pin   TEC action
     *    0          0        Turn off
     *    0          1        Heat
     *    1          0        Cool
     *    1          1        Float - if it was a motor it would spin freely.  Shouldn't be used with a TEC.
     */
    TEC(PinName ena_pin, PinName cool_pin, PinName heat_pin);
    
    enum TecAction 
     {Heating = 0, 
      Cooling = 1};
    
    /** Set the output to the TEC depending on desired heating or cooling
     *
     * @param action Selects cooling or heating 
     * @param percent_power  The amount of heating or cooling done.  
     *                       Range 0.0 .. 100.0 as a percentage of full power
     */
    void setClimate
      (TecAction action,
       float     percent_power);
    
protected:
    PwmOut     _ena_pin;
    DigitalOut _cool_pin;
    DigitalOut _heat_pin;
};

#endif // MBED_TEC_H
