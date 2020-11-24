/* Mbed Flow Sensor.
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


Attaches a pin as a digital input and attaches an ISR to handle rising edge
inputs.  Each input corresponds to a fixed volume of water/air/fuel, which is
settable.

Volume over time can be calculated and a flow history can be maintained.

*/
#include "FlowSensor.h" 

/** Interface to read a flow sensor
 */
FlowSensor::FlowSensor(PinName sensor_pin, double volume_increment): _flow_interrupt(sensor_pin) {
    _flow_interrupt.rise(this, &FlowSensor::add_volume);
    _volume_increment = volume_increment;
    _flow_count = 0;
    _last_flow_count = 0;
    
}

void FlowSensor::calibrate_volume_increment(double volume_increment)
{
    _volume_increment = volume_increment;
}
    
double FlowSensor::read_volume(void)
{
    // this should all be safe and should never lose a tick of flow from
    // the sensor
    const uint64_t this_flow_count = _flow_count;
    const double volume_since_last_read = 
      _volume_increment * double(this_flow_count - _last_flow_count);
    _last_flow_count = this_flow_count;
    return volume_since_last_read;
}
    
double FlowSensor::read_total_volume(void)
{
    return _volume_increment * double(_flow_count);
}
    
void FlowSensor::add_volume()
{
    _flow_count++;
}