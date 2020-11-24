/* Mbed Flow Sensor.

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


Attaches a pin as a digital input and attaches an ISR to handle rising edge
inputs.  Each input corresponds to a fixed volume of water/air/fuel, which is
settable.

Volume over time can then be calculated.
*/

#ifndef MBED_FLOW_SENSOR_H
#define MBED_FLOW_SENSOR_H

#include "mbed.h"

/** Interface to initialize and read a flow sensor
 *
  * Example:
 * @code
 * //Initialize and control a TEC connected to an H-bridge
 * #include "mbed.h"
 * #include "FlowSensor.h"
 * 
 * FlowSensor myFlow(p17, 1.045); // Each tick calibrated to 1.045 ml
 * 
 * int main() {
 *     double thisFlow_ml;
 *     while(1) {
 *         // Get the flow since we last checked
 *         thisFlow_ml = myFlow.read_volume();
 *         printf("%4.1f ", thisFlow_ml);
 *         wait(1.0);
 *     }
 * }
 * @endcode
 */
class FlowSensor {
public:

    /** a Flow Sensor
     *
     * @param sensor_pin - the pin connected to the signal line of the flow meter
       @param volume_increment - volume indictated by single pulse of the flow meter
     */
    FlowSensor(PinName sensor_pin, double volume_increment);
    
    /** Set the speed of the motor
     * 
     * @param volume_increment Reset the volume_increment.
     */
    void calibrate_volume_increment(double volume_increment);
    
    /* calculates the the volume since the last read */
    double read_volume(void);
    
    /* calculates the total volume seen */
    double read_total_volume(void);
    
protected:
    // using a pin as an external counter isn't portable under mbed
    // interrupts are fast enough for these flow sensors.
    InterruptIn _flow_interrupt; // each rising edge add to volume
    volatile uint64_t _flow_count;  // only variable that might be read/set by
                                    // both a method and the ISR
    uint64_t _last_flow_count;
    double _volume_increment;
    
    /* adds to the _flow_count each time the ISR is called */
    void add_volume();    
};

#endif
