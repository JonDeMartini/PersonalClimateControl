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

This library allows setting a minimum threshold for fans to turn on,
but otherwise uses PWM like any other motor.

*/

#include "DcFan.h"

#include "mbed.h"

DcFan::DcFan(PinName pwm, float min_active_pwm):
    _pwm(pwm) {

    // Set initial condition of PWM
    _pwm.period(0.00005);
    _pwm = 0.0;
    if(min_active_pwm > 0.0)
    {
        _min_active_pwm = min_active_pwm;
    } else
    {
        _min_active_pwm = 0.0;
    }
}

void DcFan::speed(float speed) {
    // Don't allow an invalid speed
    if (speed > 1.0)
    {
        speed = 1.0;
        // TODO warning?
    }
    if (speed < _min_active_pwm)
    {
        speed = 0.0;
    }
    _pwm = speed;
}
