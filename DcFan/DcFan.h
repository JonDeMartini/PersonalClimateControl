/* Mbed DC fan PWM control.  
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

Allows setting a minimum threshold

*/

#ifndef MBED_DC_FAN_H
#define MBED_DC_FAN_H

#include "mbed.h"

/** Interface to control a 2 pin DC fan.  Set speed with PWM. No rpm readings
 *
 * Drive a fan using a PwmOut
 * Example:
 * @code
 * //Initialize and control a TEC connected to an H-bridge
 * #include "mbed.h"
 * #include "DcFan.h"
 * 
 * DcFan myFan(p26, 0.5); // pwm, minimum pwm speed
 * 
 * int main() {
 *     double fanSpeed = 0.0;
 *     while(1) {
 *         if (fanSpeed < 1.0)
 *         {
 *             fanSpeed += 0.1;
 *         }
 *         // fans won't turn on until fanSpeed >= 0.5
 *         Fans.speed(fanSpeed);
 *         wait(1.0);
 *     }
 * }
 * @endcode
 */
class DcFan {
public:

    /** Create a fan motor control interface    
     *
     * @param pwm A PwmOut pin, driving the H-bridge enable line to control the speed
     */
    DcFan(PinName pwm, float min_active_pwm);
    
    /** Set the speed of the motor
     * 
     * @param speed The speed of the motor as a normalised value between 0.0 and 1.0
     */
    void speed(float speed);

    /** Get the set speed of the motor, does *not* read RPMs from fan */
    float current_speed(void);

protected:
    PwmOut _pwm;
    float  _min_active_pwm;
};

#endif
