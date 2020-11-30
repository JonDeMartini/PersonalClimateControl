/* Mbed Thermistor Sensor Interface

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

#include "Thermistor.h"
#include <math.h>

Thermistor::Thermistor(PinName thermistor_pin, double VCC, double R1, double A, double B, double C):
    _thermistor_pin(thermistor_pin) {
        _VCC = VCC;
        _R1  = R1;
        _A   = A;
        _B   = B;
        _C   = C;
}

double Thermistor::Vout(void) {
    // Convert _thermistor_pin 0.0 to 1.0 value to the Vout voltage
    // by multiplying by _VCC (usually 3.3 or 5.0V)
    return _thermistor_pin * _VCC;
}
    
double Thermistor::R_thermistor(void) {
    const double Vout = Thermistor::Vout();
    return (Vout * _R1) / (_VCC - Vout);
}

double Thermistor::temperature_K(void) {
    // Implement the Steinhart-Hart equation to convert the measured 
    // voltage to temperature in Kelvin
    const double ln_R2 = log (Thermistor::R_thermistor());
    //     {      [     (          )   (        (          )) ] }
    return (1.0 / (_A + (_B * ln_R2) + (_C * pow(ln_R2, 3.0)) ) );
}

double Thermistor::temperature_C(void) {
    return Thermistor::temperature_K() - 273.15;
}

double Thermistor::temperature_F(void) {
    return ((9.0 / 5.0) * Thermistor::temperature_C()) + 32.0;
}