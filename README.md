# PersonalClimateControl
Control TECs connected to a cooling shirt to provide personal heating and cooling.
# Personal Climate Control
Control Thermoelectric Coolers (TECs) connected to a cooling shirt to provide personal heating and cooling.

## Description

This is a project to build a personal heating/cooling system using TECs and a cooling shirt. The TEC or TECs are sandwiched between two water blocks. One water block forms a loop with a radiator. DC fans are attached to the radiator to introduced forced air and aid in cooling. The other water block forms a loop with the cooling shirt or vest.

Each water loop includes DC pump that can be turned on and off as well as a flow meter that can monitor whether the pump is working properly.

Each water block has a thermistor attached. These are used to determine if the TECs are getting too hot, if the goal temperature for the shirt loop has been reached, and to prevent the cold side from potentially freezing the coolant.

Power is delivered either from a 12V power supply or connected directly to a vehicles 12 V charging system.  This could be a motorcycle, riding lawn mower, or any device that can provide 12V at sufficient amperage (about 20A).

Power is delivered to the pumps and TECs from several Dual H-Bridges.  For the TECs these have the ability to invert the polarity of the connection, turning this from a cooling device into a heating device if desired.  For the pumps and fans the Dual H-Bridges is overkill and could be replaced with a high speed power switch that would allow PWM control of 12V power from a microcontroller.

To power the microcontroller a DC regulator is needed to convert 12V to 5V.

This project has multiple voltages (12V, 5V and 3.3V) and some high amps. This might not be a good project to start with if you are only experienced with signal level designs.

The microcontroller targeted by this project is an mbed NXP LPC1768, but any microcontroller with sufficent PWM channels and that supported analog input could be substituted.

Control is through an Adafruit Bluefruit UART.  It can connect to an app on both Android and iOS that can control the desired temperature, turn on/off cooling or heating, and run the pumps on/off to aid in bleeding air from the water loops.  This application can also graph the temperature of both of the water block over time.

An optional uLCD panel can also be attached to provide visual feedback.  

For debugging a program the microcontroller can be connected to a PC and temperatures can be monitored with a program such as [SerialPloat](https://hackaday.io/project/5334-serialplot-realtime-plotting-software).

## Presentation

Insert youtube link here.

## Demonstration

Insert youtube link here.

## Inventory

Item | Description | Link | Picture|Quantity
-----|-------------|------|--------|--------
Thermoelectric Cooler (TEC1-12706)|6A TEC Used to heat and cool.  I used 4 TECs rated at a maximum current of 6A each, but as configured they rarely draw much more than 3.5A.|[TEC](https://www.amazon.com/gp/product/B01IT8SAZG/)|![TEC](/images/TEC1-12706.jpg)|4
Cooling Shirt|Any Cooling Shirt that can flow water through soft tubing such as those availabe from CoolShirt Systems|[Cooling Shirt](https://www.amazon.com/gp/product/B000K3GHZY)|![Cooling Shirt](/images/Cooling_Shirt.jpg)|1
DC Pump|A good DC pump, I used the model listed here, but you'd be advised to get a better on, at least for the shirt loop.  You want something with plenty of lift to get water moving in all the small tubing of the shirt.  The shirt I have has over 15m of tubing.|[DC Pump](https://www.amazon.com/gp/product/B085NQ5VVJ/)|![DC Pump](/images/DC_Pump_12V.png)|2
Water blocks|Big enough water blocks to accommodate the TECs.|[Water Cooling Block](https://www.amazon.com/gp/product/B07TDQ1M32/)|![Water block](/images/Water_Block.jpg)|2
Radiator|Main source of cooling the hot side of the TECs.  I used a 360mm x 120mm radiator.|[360mm x 120mm Radiator](https://www.amazon.com/gp/product/B01D84P0WM/)|![Radiator](/images/Radiator_and_Fans.jpg)|1
DC fans|Used to cool the radiator.  You want fans that will move a lot of air.  Ideally the radiator needs to stay near ambient temperature.|[12V DC Fans](https://www.amazon.com/gp/product/B07PS2GDRS/)||3
1-2A Dual H-Bridge|A lower power Dual H-Bridge used to drive the DC fans and DC pumps|[Dual H-Bridge 2A](https://www.amazon.com/gp/product/B06X9D1PR9/)|![Dual H-Bridge](/images/Dual_H_Bridge.jpg)|2
6-7A Dual H-Bridge|A higher power Dual H-Bridge used to drive the TECs|[Dual H-Bridge 7A](https://www.amazon.com/gp/product/B06XGD5SCB/)|![7A Dual H-Bridge](/images/Dual_H_Bridge_High_Amp.png)|2
DC Regulator|Used to convert 12V from the vehicle/power supply to 5V to drive the microcontroller and the accessories (uLCD and Bluetooth).  Note that if you change microcontrollers you might need to provide 3.3V instead.|[DC Regulator](https://www.amazon.com/gp/product/B01MQGMOKI/)|![DC Regulator](/images/DC_Regulator.jpg)|1
Flow Sensors|Used to determine pump health.|[Flow Sensor](https://www.amazon.com/gp/product/B07MY76NCV/)|![Flow Sensor](/images/Flow_Sensor.jpg)|2
Thermistors|Measures the temperature of the water blocks|[Thermistors](https://www.amazon.com/gp/product/B07JGJKPT9/)|![Thermistor](/images/Thermistor.jpg)|2
Resistors|Used in the Thermistor circuit.  Must be matched to the thermistor selected.  Thermistors I used were 100k 1%, so used similarly valued Resistors|||2
Microcontroller|mbed NXP LPC1768|[mbed LPC1768](https://www.digikey.com/en/product-highlight/n/nxp-semi/mbed-lpc1768-development-board)|![mbed](/images/mbed.png)|1
Bluetooth UART|I used a Bluefruit LE UART from Adafruit, but any Bluetooth device that can connect as a serial UART should work.|[Adafruit Bluefruit LE UART Friend](https://www.adafruit.com/product/2479)|![Bluefruit](/images/Bluetooth_UART.jpg)|1
uLCD panel|Optional.  Used mainly for debugging.  I used a uLCD-144-G2 from 4D Systems Pty Ltd|[uLCD-144-G2](https://www.digikey.com/en/products/detail/4d-systems-pty-ltd/ULCD-144-G2/5725469)|![uLCD](/images/uLCD.jpg)|1
Various tubing|Used in the radiator water loop and shirt water loop. Depending on how you setup your tubing you may also need various fittings.|||As needed
Water reservoir or valves|You need some way to fill and bleed the water loops.  I used valves since I couldn't source a reasonably priced reservoir in the time I had to make this project, but I highly recommend using good reservoir instead.  If you go with valves you'll need two T's and two valves for each loop.  Otherwise you'll need a reservoir for each loop.|||2+
Enclosure|You need something to contain everything.  Keep the water blocks separate from the power and electronics.  The cold side of the TECs will cause condensation.|||1
Wire in various gauges|You need to use appropriate gauges for the amount of power used or you'll run the risk of melting insulators or even starting a fire.  16AWG should be good enough for most of the power distribution since individual wires shouldn't carry much more than 3A for extended periods.|||As needed
Heatshrink/solder|There will be some custom wiring needed.|||As needed
Standoffs|You need some way to attach the boards to the enclosure.  Standoffs work best when you have matching machine screws and a tap and die set.|||As needed
Breadboard|To wire up all the sensors and controls to the mbed microcontroller.  Could use a custom PCB or solder directly to pins, but that isn't recommended.|||As needed
12V 30A Power Supply|Need a pretty beefy power supply to test with.|[12V 30A DC Power Supply](https://www.amazon.com/gp/product/B06VWV5YCH/)|![Power Supply](/images/Power_Supply_12V_30A.jpg)|1
Power Socket and Switch|To connect the DC Power supply to A/C|[Power Socket and Switch](https://www.amazon.com/gp/product/B07RRY5MYZ/)||1
Junction Block|Over 20A of power needs to be distributed. Standard junctions aren't quite up to the task and would be a fire hazard so use something like these hefty power junctions.|[Power and Ground Junctions](https://www.amazon.com/gp/product/B079R4VDJC/)|![Power Junction](/images/Power_Junction.png)|1


## Dual H-Bridges Connection

Most Dual H-Bridges have similar connections.  Refer to the following example when wiring up the full system from the schematic.

![Labeled Dual H-Bridge](/images/Labeled_Dual_H_Bridge.png)

This project uses a TEC library I developed for mbed. It is similar to motor libraries that use Dual H-Bridges to control the speed and direction of a motor, but with some differences in terminology and with a few cases like floating eliminated. [mbed TEC library](https://os.mbed.com/users/demartini/code/TEC/)

## Flow Sensors

The Flow Sensors have connections for Vcc (red), Ground (black), and Signal (yellow).  Internally there is wheel that turns and activates a hall sensor as it spins.  Each activation is a pulse on the Signal line.  The microcontroller counts these pulses to determine a volume of liquid (or gas) that has passed through sensor.  If you want accurate measurements from a flow sensor it is a good idea to calibrate it before use by measuring how many signals are seen when a known volume passes through the sensor.

This project uses a Flow Sensor library I developed for mbed. [mbed Flow Sensor Library](https://os.mbed.com/users/demartini/code/FlowSensor/)

![Flow Sensor](/images/Flow_Sensor.jpg)

## Thermistors

Thermistors are resistors whose values change with temperature.  They are setup as simple voltage dividers.  One side is connected to Vcc (3.3V in our case) and the other side to an Analog Input to the microcontroller.  A resister from the Analog Input to ground completes the circuit.  As the temperature at the thermistor changes the voltage measured at the Analog input changes.  It is important that the resistor chosen match the nominal resistance of the thermistor.

By attaching the thermistors directly in contact with the water blocks we measure the hot and cold sides of the TECs (through the water blocks).  This is used both for safety and to control the amount of power delivered to the TECs.  If the radiator side gets too hot the TECs are turned off to prevent them from overheating.  The shirt side measures how cold we are able to chill the water going to the shirt.  Once it reaches the target temperature the TECs are slowly powered down, or back up if the temperature creeps back above the goal temperature.

This project uses a Thermistor library that I developed for mbed.  It implements the Steinhart-Hart A, B, C coefficients. [mbed Thermistor library](https://os.mbed.com/users/demartini/code/Thermistor/)  Read more about Steinhart-Hart coefficients at [Steinhart-Hart](https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation)

![Thermistors](/images/Thermistor.jpg)

## Bluetooth Control

Use the iOS or Android Bluefruit Connect app to control the Personal Climate Control hardware.  Search for Bluefruit Connect on the app store to find it.

![Bluefruit Connect app search](/images/Bluefruit_App.JPG)

Once downloaded launch the app and if Bluetooth is turned on it will show the Adafruit Bluefruit LE is available to connect. Connect to it. The first time it connects there may be an automatic firmware update needed.  Perform that and continue.

![Bluefruit Connection](/images/Bluefruit_Connect.JPG)

Once connected select Controller.

![Bluefruit Controller Selection](/images/Bluefruit_Connected.PNG)

Select Control Pad.

![Bluefruit Controller Selection](/images/Bluefruit_Controller.JPG)

The controller display shows the temperature of the water blocks as well as a goal tempterature for cooling/heating.  You can adjust this goal temperature by pressing the up and down directions.  You can also choose to start cooling or heating, turn the system Off or turn the the pumps on.

Turning the pumps on can be very helpful when bleeding air from the system.  It is not recommended that the TECs are enabled without bleeding air from the system as they risk overheating.  It is also bad to run the pumps dry for extended periods.

![Controls](/images/Controls-notated.png)

## Schematics and Construction

The following schematic shows everything is connected (click for large version):

![Schematic and circuit diagram](/images/Personal_Climate_Control_Schematic_4k.png)

Just the mbed connections:

![mbed connections](/images/mbed_Connections.png)

The TECs should be securely sandwiched between the water blocks. Thermal paste should be used to make contact between the water blocks and the TECs.  The thermistors should be firmly attached to each side of the water block.  I used electrical tape on the thermistors and tape along with zip ties to secure the entire water block assembly together.  This should be kept well separated from the Dual H-Bridges, microcontroller, and all the power distribution of the rest of the project.

![Water Block and TEC Assembly](/images/Water_Block_and_TEC_Assembly.jpg)

I ended up using acrylic to make a custom enclosure, but there are commercial products that would work as well or better.

## Function

The logic running on the mbed is state machine that monitors the temperature of the water blocks and the flow sensors.  It will turn on and off the DC Fans, each DC pump separately and the TEC function (heating or cooling) as well as the power level (through PWM).  Each TEC can be controlled individually, but the final logic always sets each TEC exactly the same.

The User input over bluetooth sets both a target temperature and a user input state.  The User input state could be Off, Cooling, Heating, Radiator Pump On or Shirt Pump On.  A combination of the sensors and user input state are used to determine a System State.  For debugging the User Input State, System State, Temperatures of the Radiator and Shirt Water blocks as well as the current flow rates of the Radiator and Shirt Flow Sensors are displayed on the uLCD.

When Cooling is first started the system state will begin to precool the cooling water block.  The radiator pump and DC fans will be turned on to cool the hot side of the TECs, but the shirt pump will not be turned on.  This will cool the cold side until either a set time has passed or the block is near freezing.

The shirt side pump will then be engaged and cooling will begin.

Heating is similar in having preheat and heating states.

Several other states exist for cases where the pumps are turned on but no flow is detected for an extended period.  Or when the cooling isn't keeping up with demand a cool down state is entered where the shirt pump is temporarily shut down and the cooling block is chilled again.

## Performance

The power usage of this system was intentionally limited to around 20A at 12V as that is a common power usage for motorcycle heating gear.  It definitely works and I've seen it chill down to 13°C.  Typically it chills closer to 17°C-18°C.  Which while cooler than ambient it doesn't feel quite as refreshing as I would like.

Perhaps counterintuitively, wearing a winter motorcycle jacket increases the cooling effect.  The follows the expectation from thermodynamics as you are both removing body heat while also insulating from the outside heat.

## Improvements

My implementation of the project is designed to fit into a backpack and be worn on a motorcycle.  While it does fit in a backpack and would function it doesn't feel particularly safe.  The next iteration will be built into a top box or luggage for one of my motorcycles.  I will also invest in better pumps and seek to replace the TEC1-12706 with more capable models.  The biggest limit on that front will be how many amps can be reliably used without overloading the charging system of the bike.  Much past the current 20A and thicker gauge cables will be needed as well.

For an indoor application only better pumps, a 40A or 50A 12V power supply, and better TECs are all that are needed.  Similar systems costing over $3000 are used by surgeons to presumably keep from sweating into patients during operation.

Film applications to cool off actors in heavy prosthetics between takes might also be possible.
