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

Use to control a TEC connected to an H-Bridge

 */

#include "TEC.h"

#include "mbed.h"

TEC::TEC(PinName ena_pin, PinName cool_pin, PinName heat_pin):
    _ena_pin(ena_pin), _cool_pin(cool_pin), _heat_pin(heat_pin) {

    // Setting the pwm frequency to 10 kHz
    // It seems to be a wives tale that TECs can't be used with PWM
    // 
    // The only actual study I've found indicates PWM has minimal 
    // or no effect long term.
    // 
    // https://tetech.com/wp-content/uploads/2013/10/ICT99MJN.pdf
    //
    _ena_pin.period(0.0001);
    
    // Start with everything off
    _ena_pin = 0.0;
    _cool_pin = 0;
    _heat_pin = 0;
}

void TEC::setClimate
      (TecAction action,
       float     power_percent)
 {
    if(power_percent == 0.0)
    {
        // Remove all power
        _cool_pin = 0;
        _heat_pin = 0;
        _ena_pin  = 0.0;
    } else
    {
        switch (action)
        {
            case Heating:
               _cool_pin = 0;
               _heat_pin = 1;
               break;
            case Cooling:
               _cool_pin = 1;
               _heat_pin = 0;
               break;
        }
        _ena_pin = power_percent/100.0;
    }
}