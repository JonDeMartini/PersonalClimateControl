/* Mbed Thermistor Sensor Interface
Jonathan L. Martin Coyright 2020

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

Reads a thermistor given the Steinhart-Hart coefficients (A, B, C) and R1


For information on the Steinhart-Hart equation see:
https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation

If you have the datasheet for a thermistor but not the A, B, and C coefficients 
it is recommended you use a tool such as:
https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html

Pick three temperatures within the expected range of values you expect in
your application, then lookup the nominal resistance expected for the 
thermistor from its datasheet.  For instance for an NTC 3950 at 100K and 1% accuracy
thermistor the follow values are provided from its datasheet:

 5 C = 254,800 Ohm
25 C = 100,000 Ohm
50 C =  35,900 Ohm

This gives Steinhart-Hart coefficients of:
A = 0.6172273387e-3
B = 2.287682172e-4
C = 0.6749479638e-7

We generally use an R1 value close to the resistance at our expected nominal temperature.

So in this case we choose a 100 KOhm resistor as a pull up.
Our AnalogIn pin has a 0.0V to 3.3V range, so we pull up to 3.3V.


*/

#ifndef MBED_THERMISTOR_H
#define MBED_THERMISTOR_H

#include "mbed.h"

/** Interface to use a thermistor sensor.
 *
 * Example:
 * @code
 * //Setup and read temperature from a Thermistor
 * #include "mbed.h"
 * #include "Thermistor.h"
 * 
 * // (AnalogIn pin, VCC, R1, A, B, C)
 * Thermistor myTempSensor(p20, 3.3, 100000.0, 0.6172273387e-3, 2.287682172e-4, 0.6749479638e-7);
 * 
 * int main() {
 *     float thisTemp_C;
 *     while(1) {
 *         thisTemp_C = myTempSensor.temperature_C();
 *         printf("printf("% 3.1f C", thisTemp_C);
 *         wait(1.0);
 *     }
 * }
 * @endcode
 */
class Thermistor {
public:

    /** Create a thermistor sensor interface
     *  Wire up as a voltage divider like so:
     *  
     *            VCC
     *             |
     *             |
     *            <R1>
     *             |
     *             |      
     *   Analog In ----------------------------------o Thermistor
     *                                              /      
     *             ---------------------------------
     *             |
     *            ---  Ground
     *             -
     *
     * @param thermistor_pin - An AnalogIn pin with the thermistor connected
     * @param VCC - Vcc in Volts. Usually 3.3V or 5V.  Be careful not to overload your AnalogIn pin.
     * @param R1 - Resistor value in Ohms, connected the the thermistor_pin and Vcc.
     * @param A - Steinhart-Hart coefficient A
     * @param B - Steinhart-Hart coefficient B
     * @param C - Steinhart-Hart coefficient C
     */
    Thermistor(PinName thermistor_pin, double VCC, double R1, double A, double B, double C);
    
    /** Get the instant voltage out from the thermistor
     *
     */
    double Vout(void);
    
    /** Get the instance Resistance of the thermistor
     *
     */
    double R_thermistor(void);
    
    /** Get the instant temperature from the thermistor in Kelvin
     *
     */
    double temperature_K(void);
    
    /** Get the instant temperature from the thermistor in Celcius
     *
     */
    double temperature_C(void);
    
    /** Get the instant temperature from the thermistor in Fahrenheit
     *
     */
    double temperature_F(void);

protected:
    AnalogIn _thermistor_pin;
    double _VCC;
    double _R1;
    double _A;
    double _B;
    double _C;
    float  _min_active_pwm;
};

#endif
