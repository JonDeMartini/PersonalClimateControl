/* Personal Climate Control

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


#include "mbed.h"
#include "rtos.h"
#include "TEC.h"
#include "DcFan.h"
#include "FlowSensor.h"
#include "Thermistor.h"

#include "uLCD_4DGL.h"

// Define physical interfaces

// USB serial for debugging to PC
Serial pc(USBTX, USBRX, 115200);

// Bluetooth LE UART support
RawSerial  bluetoothLE(p28, p27);

// my flow meters are right around 1 mL per tick, 
// For this application we are more concerned with a minimum flow than accuracy
// No need to calibrate more than the initial calibration.
FlowSensor RadiatorFlow(p16, 1.045);
FlowSensor ShirtFlow(p17, 1.045);

// All 3 120mm fans are tied together into one Dual H-bridge.
// Dual H-bridge is overkill in this case, but it was available and allows us to
// drive a 12V ~1A total PWM signal
DcFan RadiatorFans(p26, 0.5); // pwm, minimum pwm speed

Thermistor RadiatorThermistor(p19, 3.3, 100000.0, 0.6172273387e-3, 2.287682172e-4, 0.6749479638e-7);
Thermistor ShirtThermistor(p20, 3.3, 100000.0, 0.6172273387e-3, 2.287682172e-4, 0.6749479638e-7);

// Four Thermo Electric Coolers - Peltier devices
// Using TEC-12706's, mainly because those were readily available and 
// can be driven by 12V
//
// When choosing a TEC you ideally want one that you will never drive right to 
// its voltage limit to avoid damage.
TEC TEC1(p21, p6,  p5);  // PWM, cool pin, heat pin
TEC TEC2(p22, p8,  p7);  // PWM, cool pin, heat pin
TEC TEC3(p23, p10, p9);  // PWM, cool pin, heat pin
TEC TEC4(p24, p12, p11); // PWM, cool pin, heat pin

// DC pumps for circulating water
// These are 12V basic DC pumps used for acquariums.
// Not a good idea to run these dry
// With more budget and more lead time would try to get a better pump,
// but these should do fine for a demo
DigitalOut DcRadiatorPump(p30);
DigitalOut DcShirtPump(p29);

uLCD_4DGL uLCD(p13,p14,p15); // serial tx, serial rx, reset pin;

enum user_state 
   {kUserOff, 
    kUserCool,
    kUserHeat,
    kUserRunRadiatorPump,
    kUserRunShirtPump}; // Hide system states from user

enum temperature_units 
   {kKelvin, 
    kCelcius, 
    kFahrenheit};

const char* UserStateToStr(user_state input)
{
    switch(input)
    {
        case kUserOff:
            return "Off       ";
        case kUserCool:
            return "Cooling   ";
        case kUserHeat:
            return "Heating   ";
        case kUserRunRadiatorPump:
            return "Rad Pump  ";
        case kUserRunShirtPump:
            return "Shirt Pump";
    }
    return "Unknown State";    
}

const char *boolToStr(bool input)
{
    if (input)
    {
        return "True ";
    } else
    {
        return "False";
    }
}

// setup for concurrent data access.  These have multiple reader
// but are only written by the BlueTooth task
// would prefer std::atomic<> but that isn't supported on this version of mbed.
//
// Note that volatile *happens* to work like atomics on mbed, 
// but this should not be considered portable
//
volatile user_state UserStateRequested = kUserOff;
volatile double     UserTemperature_C = 25.5; // 25.5 C, 78 F

const double kMinUserTemperature_C  = 1.0; // Just above freezing
const double kMaxUserTemperature_C  = 32.0; // About 90 F
const double kStepUserTemperature_C = 0.5;

const float MaxRadiatorTemp_C = 90.0; // Don't boil coolant!
const float MinRadiatorTemp_C =  1.0; // Don't freeze coolant (could lower with additive)

const float MaxShirtTemp_C     = 40.0; // Never turn on pump when it could burn!
const float MinShirtTemp_C     =  1.0; // Don't freeze coolant (could lower with additive)
const float ShirtPreCoolTemp_C =  2.0;

const double kMinTimeInMode_s = 20.0;

// control currently setup with bluetooth.  Using UART based bluetooth with AdaFruit App.
//
// Buttons are laid out like this:
//
//          5      1   2
//        7   8    
//          6      3   4
//
void bluetooth_recv()
{
    char bnum;
    while(bluetoothLE.readable()) {
        if (bluetoothLE.getc()=='!') {
            if (bluetoothLE.getc()=='B') { //button data
                bnum = bluetoothLE.getc(); //button number
                if (bnum == '1'){ // Turn Cooling on
                    if(bluetoothLE.getc() == '1')
                    {
                        UserStateRequested = kUserCool;
                    }
                }
                if (bnum == '2'){ // Turn OFF
                    if(bluetoothLE.getc() == '1')
                    {
                        UserStateRequested = kUserOff;
                    }
                }
                if (bnum == '3'){ // Turn Heating On
                    if(bluetoothLE.getc() == '1')
                    {
                        UserStateRequested = kUserHeat;
                    }
                }
                if (bnum == '4'){ // Run Shirt Pump on first press,
                // toggle between shirt and radiator pumps after that
                    if(bluetoothLE.getc() == '1')
                    {
                        if (UserStateRequested == kUserRunShirtPump)
                        {
                            UserStateRequested = kUserRunRadiatorPump;
                        } else
                        {
                            UserStateRequested = kUserRunShirtPump;
                        }
                    }
                }
                if (bnum == '5') // Increase temperature
                {
                    if(bluetoothLE.getc() == '1')
                    {
                       if (UserTemperature_C < kMaxUserTemperature_C)
                       {
                          UserTemperature_C += kStepUserTemperature_C;
                       }
                    }
                }
                if (bnum == '6') // Decrease temperature
                {
                    if(bluetoothLE.getc() == '1')
                    {
                       if (UserTemperature_C > kMinUserTemperature_C)
                       {
                          UserTemperature_C -= kStepUserTemperature_C;
                       }
                    }
                }
            }
        }
    }
}

void SetClimate
    (TEC::TecAction ThisClimateState,
     float          TecPowerPercent) // 0.0 to 100.0, as a % of max power
{
    // Set all 4 TECs the same
    TEC1.setClimate
    (ThisClimateState,
     TecPowerPercent);
    TEC2.setClimate
    (ThisClimateState,
     TecPowerPercent);
    TEC3.setClimate
    (ThisClimateState,
     TecPowerPercent);
    TEC4.setClimate
    (ThisClimateState,
     TecPowerPercent);

}    

enum system_state 
   {kSystemOff, 
    kSystemPrecool,
    kSystemPreheat,
    kSystemCooling, 
    kSystemHeating, 
    kSystemCoolDown,
    kSystemCoolCoast,
    kSystemHeatUp,
    kSystemHeatCoast,
    kSystemRunRadiatorPump,
    kSystemRunShirtPump};
    
const char* SystemStateToStr(system_state input)
{
    switch(input)
    {
        case kSystemOff:
            return "Off        ";
        case kSystemPrecool:
            return "Precool    ";
        case kSystemPreheat:
            return "Preheat    ";
        case kSystemCooling:
            return "Run Cool   ";
        case kSystemHeating:
            return "Run Heat   ";
        case kSystemCoolDown:
            return "Cool Down  ";
        case kSystemCoolCoast:
            return "Cool Coast ";
        case kSystemHeatUp:
            return "Heat Up    ";
        case kSystemHeatCoast:
            return "Heat Coast ";
        case kSystemRunRadiatorPump:
            return "Rad Pump   ";
        case kSystemRunShirtPump:
            return "Shirt Pump ";
    }
    return "Unk State  ";
}

const char* TecActionToStr(TEC::TecAction input)
{
    switch(input)
    {
        case TEC::Cooling:
            return "Cooling";
        case TEC::Heating:
            return "Heating";
    }
    return "Invalid";
}

time_t         TimeModeEntered_s    = 0;
double         PreUserTemperature_C = UserTemperature_C;
TEC::TecAction ClimateState         = TEC::Cooling;


system_state TransitionSystemState
    (system_state SystemState,
     float        RadiatorTemperature_C,
     float        ShirtTemperature_C)
{
    time_t CurrentTime_s = time(NULL);
    
    system_state ThisSystemState = SystemState;

    bool RadiatorTempOkay = 
        (RadiatorTemperature_C >= MinRadiatorTemp_C) &&
        (RadiatorTemperature_C <= MaxRadiatorTemp_C);
    bool ShirtTempOkay =
        (ShirtTemperature_C >= MinShirtTemp_C) &&
        (ShirtTemperature_C <= MaxShirtTemp_C);

    // DC pump rated for 240L / hr
    // That would be over 60 mL a second
    // Accept a fraction of that without considering the pump compromized.
    const float MinFlow_ml = 1.0; // TODO: this is a minimal "is pumping"
                                  //       change to ml / time period
                                  //       with a timer instead of per period.
                                  //       Up ml/sec to a safer pump level.
    
    const double CheckPumpTime_s = 10.0;
    
    double RadiatorFlow_ml = RadiatorFlow.read_volume();
    double ShirtFlow_ml    = ShirtFlow.read_volume();
    
    static time_t RadiatorPumpLastGood_s = time(NULL);
    static time_t ShirtPumpLastGood_s    = time(NULL);
    
    if (RadiatorFlow_ml >= MinFlow_ml)
    {
        RadiatorPumpLastGood_s = time(NULL);
    }
    if (ShirtFlow_ml >= MinFlow_ml)
    {
        ShirtPumpLastGood_s = time(NULL);
    }
    
    bool RadiatorPumpOkay = (CurrentTime_s - RadiatorPumpLastGood_s <= CheckPumpTime_s);
    bool ShirtPumpOkay    = (CurrentTime_s - ShirtPumpLastGood_s <= CheckPumpTime_s);
    
    
//    pc.printf("\n\nShirt Temp OK? %s\n", boolToStr(ShirtTempOkay));
//    pc.printf("Shirt Flow %4.1f ml\n", ShirtFlow_ml);
//    pc.printf("Shirt Pump OK? %s\n\n", boolToStr(ShirtPumpOkay));
//    pc.printf("Radiator Temp OK? %s\n", boolToStr(RadiatorTempOkay));
//    pc.printf("Radiator Flow %4.1f ml\n", RadiatorFlow_ml);
//    pc.printf("Radiator Pump OK? %s\n\n", boolToStr(RadiatorPumpOkay));

    //pc.printf("%4.1f %4.1f ", ShirtFlow_ml, RadiatorFlow_ml);

    uLCD.locate(11,6);
    uLCD.printf("% 3.0fml", RadiatorFlow_ml);
    uLCD.locate(11,7);
    uLCD.printf("% 3.0fml", ShirtFlow_ml);

    switch(UserStateRequested) 
    {
        case kUserOff:
            ThisSystemState = kSystemOff;
            // Was this a transition?
            if (ThisSystemState != SystemState)
            {
                TimeModeEntered_s = CurrentTime_s;
            }                
            break;
            
        case kUserCool:
            // Don't transition too quickly
            if (CurrentTime_s - TimeModeEntered_s > kMinTimeInMode_s)
            {
                if (RadiatorTempOkay && ShirtTempOkay)
                {
                    // Transition unless in cooling sub-state
                    if ((SystemState != kSystemPrecool) &&
                        (SystemState != kSystemCooling) &&
                        (SystemState != kSystemCoolDown) &&
                        (SystemState != kSystemCoolCoast) )
                    {
                        ThisSystemState = kSystemPrecool;
                    }
                    if (ThisSystemState == kSystemCooling)
                    {
                        // Already been in Running State for min time.
                        // Start checking pumps.
                        if (!RadiatorPumpOkay || !ShirtPumpOkay)
                        {
                            // Shut down for a bit
                            ThisSystemState = kSystemCoolCoast;
                        }
                    }
                } else
                {
                   // Shut down for a bit, temperature over safe
                    ThisSystemState = kSystemOff;
                }                
                // Was this a transition?
                if (ThisSystemState != SystemState)
                {
                    TimeModeEntered_s = CurrentTime_s;
                }
            }
            break;
            
        case kUserHeat:
            // Don't transition too quickly
            if (CurrentTime_s - TimeModeEntered_s > kMinTimeInMode_s)
            {
                if (RadiatorTempOkay && ShirtTempOkay)
                {
                    // Transition unless in heating sub-state
                    if ((SystemState != kSystemPreheat) &&
                        (SystemState != kSystemHeating) &&
                        (SystemState != kSystemHeatUp) &&
                        (SystemState != kSystemHeatCoast) )
                    {
                        ThisSystemState = kSystemPreheat;
                    }
                    if (ThisSystemState == kSystemHeating)
                    {
                        // Already been in Running State for min time.
                        // Start checking pumps.
                        if (!RadiatorPumpOkay || !ShirtPumpOkay)
                        {
                            // Shut down for a bit
                            ThisSystemState = kSystemHeatCoast;
                        }
                    }
                } else
                {
                   // Shut down for a bit, temperature over safe
                    ThisSystemState = kSystemOff;
                }                
                // Was this a transition?
                if (ThisSystemState != SystemState)
                {
                    TimeModeEntered_s = CurrentTime_s;
                }
            }
            break;
            
        case kUserRunRadiatorPump:
            // Don't transition too quickly
            if (CurrentTime_s - TimeModeEntered_s > kMinTimeInMode_s)
            {
                if (RadiatorTempOkay)
                {
                    ThisSystemState = kSystemRunRadiatorPump;
                    if (SystemState == kSystemRunRadiatorPump)
                    {
                        // Already been in this pump state for min time.
                        // Start checking pumps.
                        if (RadiatorPumpOkay)
                        {
                            ; // Do nothing
                        } else
                        {
                            // Shut down for a bit
                            ThisSystemState   = kSystemOff;
                        }
                    }                    
                } else
                {
                    ThisSystemState = kSystemOff;
                }
                // Was this a transition?
                if (ThisSystemState != SystemState)
                {
                    TimeModeEntered_s = CurrentTime_s;
                }
            }
            break;
            
        case kUserRunShirtPump:
            // Don't transition too quickly
            if (CurrentTime_s - TimeModeEntered_s > kMinTimeInMode_s)
            {
                if (ShirtTempOkay)
                {
                    ThisSystemState = kSystemRunShirtPump;
                    if (SystemState == kSystemRunShirtPump)
                    {
                        // Already been in this pump state for min time.
                        // Start checking pumps.
                        if (ShirtPumpOkay)
                        {
                            ; // Do nothing
                        } else
                        {
                            // Shut down for a bit
                            ThisSystemState   = kSystemOff;
                        }
                    }                    
                } else
                {
                    ThisSystemState = kSystemOff;
                }
                // Was this a transition?
                if (ThisSystemState != SystemState)
                {
                    TimeModeEntered_s = CurrentTime_s;
                }
            }
            break;
    }
    return ThisSystemState;
}

void Periodic_Processing()
{
    static system_state SystemState           = kSystemOff;
    
    // Keep state between calls
    static float          TecPowerPercent     = 0.0; // 0.0 to 100.0, as a % of max power
    static bool           RadiatorPumpEnabled = false;
    static bool           ShirtPumpEnabled    = false;
    
    // Reset every time
    bool                  EnableFanSeparately = false;

    // If we can't keep within this amount of the goal try coasting and doing
    // another mini cool down.
    const float           FallingBehind_C     = 5.0;
    // This is the number of degrees overwhich we ramp power down to 0%
    const float           Rampdown_C          = 2.0;
    // Precool/preheat up to 5 minutes
    const double          kPreTime_s          = 60.0 * 5.0; 
    // Heat or Cool without shirt pump for a while
    const double          kSmallCycleTime_s   = 30.0;
    
    time_t CurrentTime_s = time(NULL);

    // Check for temperature overruns
    
    float RadiatorTemperature_C = RadiatorThermistor.temperature_C();
    float ShirtTemperature_C    = ShirtThermistor.temperature_C();

    // basic state machine
    switch(SystemState)
    {
        case kSystemOff:
            // Turn all TECs off
            ClimateState = TEC::Cooling;
            TecPowerPercent = 0.0;
            // Turn off pumps
            RadiatorPumpEnabled = false;
            ShirtPumpEnabled    = false;
            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);
            break;

        case kSystemPrecool:
            // Radiator always on when TECs on
            RadiatorPumpEnabled = true;
            ShirtPumpEnabled    = false;
            ClimateState = TEC::Cooling;
            TecPowerPercent = 100.0;

            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);

            if(ShirtTemperature_C <= ShirtPreCoolTemp_C)
            {
                SystemState       = kSystemCooling;
                TimeModeEntered_s = CurrentTime_s;
            }

            if (CurrentTime_s - TimeModeEntered_s > kPreTime_s)
            {
                SystemState       = kSystemCooling;
                TimeModeEntered_s = CurrentTime_s;
            }

            break;

        case kSystemPreheat:
            // Radiator always on when TECs on
            RadiatorPumpEnabled = true;
            ShirtPumpEnabled    = false;
            ClimateState = TEC::Heating;
            TecPowerPercent = 100.0;
            
            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);            
            
            if(ShirtTemperature_C >= UserTemperature_C + Rampdown_C)
            {
                SystemState       = kSystemHeating;
                TimeModeEntered_s = CurrentTime_s;    
            } 
            
            if (CurrentTime_s - TimeModeEntered_s > kPreTime_s)
            {
                SystemState       = kSystemHeating;
                TimeModeEntered_s = CurrentTime_s;
            }

            break;

        case kSystemCooling:
            // Pumps are always on when running
            RadiatorPumpEnabled = true;
            ShirtPumpEnabled    = true;
            ClimateState = TEC::Cooling;
            if((CurrentTime_s - TimeModeEntered_s > kSmallCycleTime_s) && 
                (ShirtTemperature_C > UserTemperature_C + FallingBehind_C))
            {
                // Not keeping up
                // turn off shirt pump and attempt to chill a bit
                ShirtPumpEnabled = false;
                TecPowerPercent = 100.0;
                SystemState = kSystemCoolDown; //kSystemCoolCoast;
                TimeModeEntered_s = CurrentTime_s;
                break;
            } else if (ShirtTemperature_C > UserTemperature_C)
            {
                TecPowerPercent = 100.0;
            } else if (abs(ShirtTemperature_C - UserTemperature_C) <= Rampdown_C)
            {
                TecPowerPercent = 
                    100.0 * (abs(ShirtTemperature_C - UserTemperature_C) / Rampdown_C);
            } else
            {
                TecPowerPercent = 0.0;
            }
            
            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);
            break;

        case kSystemHeating:
            // Pumps are always on when running
            RadiatorPumpEnabled = true;
            ShirtPumpEnabled    = true;
            ClimateState = TEC::Heating;
            if((CurrentTime_s - TimeModeEntered_s > kSmallCycleTime_s) &&
                (ShirtTemperature_C < UserTemperature_C - FallingBehind_C))
            {
                // Not keeping up
                // turn off pump and attempt to heat up a bit
                ShirtPumpEnabled = false;
                TecPowerPercent = 100.0;   
                SystemState = kSystemHeatUp; // kSystemHeatCoast;
                TimeModeEntered_s = CurrentTime_s;
                break;             
            } else if (ShirtTemperature_C < UserTemperature_C)
            {
                TecPowerPercent = 100.0;
            } else if (abs(ShirtTemperature_C - UserTemperature_C) <= Rampdown_C)
            {
                TecPowerPercent = 
                    100.0 * (abs(ShirtTemperature_C - UserTemperature_C) / Rampdown_C);
            } else
            {
                TecPowerPercent = 0.0;
            }
            
            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);
            break;
            
        case kSystemCoolDown:
            // Radiator always on when TECs on
            RadiatorPumpEnabled = true;
            ShirtPumpEnabled    = false;
            ClimateState = TEC::Cooling;
            TecPowerPercent = 100.0;

            if(ShirtTemperature_C <= ShirtPreCoolTemp_C)
            {
                SystemState       = kSystemCooling;
                TimeModeEntered_s = CurrentTime_s;
            }

            if (CurrentTime_s - TimeModeEntered_s > kSmallCycleTime_s)
            {
                SystemState       = kSystemCooling;
                TimeModeEntered_s = CurrentTime_s;
            }

            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);
            break;

        case kSystemCoolCoast:
            // Cool down hot side a bit before continueing to cool
            RadiatorPumpEnabled = false;
            ShirtPumpEnabled    = false;
            EnableFanSeparately = true;
            ClimateState        = TEC::Cooling; // Still attempting to cool later
            TecPowerPercent     = 0.0;

            if (CurrentTime_s - TimeModeEntered_s > kSmallCycleTime_s)
            {
                SystemState       = kSystemCoolDown;
                TimeModeEntered_s = CurrentTime_s;
            }

            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);
            break;

        case kSystemHeatUp:
            // Radiator always on when TECs on
            RadiatorPumpEnabled = true;
            ShirtPumpEnabled    = false;
            ClimateState = TEC::Heating;
            TecPowerPercent = 100.0;
            if(ShirtTemperature_C >= UserTemperature_C + Rampdown_C)
            {
                SystemState       = kSystemHeating;
                TimeModeEntered_s = CurrentTime_s;
            } 
            if (CurrentTime_s - TimeModeEntered_s > kSmallCycleTime_s)
            {
                SystemState       = kSystemHeating;
                TimeModeEntered_s = CurrentTime_s;
            }
            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);
            break;
            
        case kSystemHeatCoast:
            // Heat up cold side a bit before continueing to heat
            RadiatorPumpEnabled = true;
            ShirtPumpEnabled    = false;
            EnableFanSeparately = true;
            ClimateState        = TEC::Heating; // Still attempting to heat later
            TecPowerPercent     = 0.0;

            if (CurrentTime_s - TimeModeEntered_s > kSmallCycleTime_s)
            {
                SystemState       = kSystemHeatUp;
                TimeModeEntered_s = CurrentTime_s;
            }

            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);
            break;

        case kSystemRunRadiatorPump:
            // Turn all TECs off
            ClimateState = TEC::Cooling;
            TecPowerPercent = 0.0;
            // Turn off pumps
            RadiatorPumpEnabled = true;
            ShirtPumpEnabled    = false;
            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);
            break;

        case kSystemRunShirtPump:
            // Turn all TECs off
            ClimateState = TEC::Cooling;
            TecPowerPercent = 0.0;
            // Turn off pumps
            RadiatorPumpEnabled = false;
            ShirtPumpEnabled    = true;
            SystemState = 
                TransitionSystemState
                    (SystemState,
                     RadiatorTemperature_C,
                     ShirtTemperature_C);
            break;
    }

    // One place to actually set system outputs
    
    // Set TEC states
    SetClimate
     (ClimateState,
      TecPowerPercent);

    // Set Pumps
    if(RadiatorPumpEnabled)
    {
        DcRadiatorPump = 1;
        // tie fan speed to TEC percent power.
        // fan speed has a minimum value
        // Was originally scaling with power output of TEC, but it seems to work
        // better if the fans just run when the radiator pump is active.
        RadiatorFans.speed(100.0);
    } else {
        DcRadiatorPump = 0;
        if(EnableFanSeparately)
        {
            RadiatorFans.speed(100.0);
        }
        else
        {   
            RadiatorFans.speed(0.0);
        }
    }
    
    if(ShirtPumpEnabled)
    {
        DcShirtPump = 1;
    } else {
        DcShirtPump = 0;
    }

    // Update status output
    //uLCD.BLIT(x, y, buzz_w, buzz_h, (int *)buzz); 
    uLCD.locate(5,1);
    uLCD.printf("%s", UserStateToStr(UserStateRequested));
    uLCD.locate(5,2);
    uLCD.printf("%s", SystemStateToStr(SystemState));
    uLCD.locate(7,3);
    uLCD.printf("% 3.1foC ", UserTemperature_C);
    uLCD.locate(7,4);
    uLCD.printf("% 3.1foC ", ShirtTemperature_C);
    uLCD.locate(7,5);
    uLCD.printf("% 3.1foC ", RadiatorTemperature_C);
    uLCD.locate(0,8);
    uLCD.printf("%s % 3.0f%%   ", TecActionToStr(ClimateState), TecPowerPercent);

    // stream temps to phone
    bluetoothLE.printf("%3.1f %3.1f %3.1f\n", RadiatorTemperature_C, ShirtTemperature_C, UserTemperature_C);

    // USB serial to PC
    pc.printf("%3.1f %3.1f %3.1f\n", RadiatorTemperature_C, ShirtTemperature_C, UserTemperature_C);
}


int main()
{
    // Initialize time, Don't need it to be correct, just for relative time stamps
    set_time(0);

    // uLCD setup
    uLCD.cls();
    uLCD.baudrate(3000000); //jack up baud rate to max for fast display
    uLCD.background_color(0x000000);
    uLCD.cls();    
    uLCD.textbackground_color(0x000000);
    uLCD.text_width(1); //1X size text
    uLCD.text_height(1);
    
    // Print the static display
    uLCD.locate(0,1);
    uLCD.printf("Req:");
    uLCD.locate(0,2);
    uLCD.printf("Sys:");
    uLCD.locate(0,3);
    uLCD.printf("User:");
    uLCD.locate(0,4);
    uLCD.printf("Shirt:");
    uLCD.locate(0,5);
    uLCD.printf("Rad:");
    uLCD.locate(0,6);
    uLCD.printf("Rad Flow:");
    uLCD.locate(0,7);
    uLCD.printf("Shirt Flow:");


    // Bluetooth UART setup    
    bluetoothLE.baud(9600);
    bluetoothLE.attach(&bluetooth_recv, Serial::RxIrq);    

    while(1) {
        Periodic_Processing();
        Thread::wait(1000);
    }
}

