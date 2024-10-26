### Hardware Issue

The first issue that we had faced was figuring out a way to control PWM of the brushless fan. The output current of the Arduino is not sufficed enough to control the 12 volt brushless dc fan. If we connect the brushless fan to the Arduino, it will draw out too much current and damage the Arduino. The best way to control the Fan would be to use a MOSFET which allows for a larger output voltage to be controlled by the smaller input voltage from the Arduino. We had to learn how to drive the fan with a MOSFET.

### Software Issue

Only brief issues working with the States of a Finite State Machine. 
We solved this by changing the type of loop that was used. In a 	regular ‘if else’, it is ok to use the do while loop but when using 	an FSM, it can lock the Finite State Machine in a specific state.


