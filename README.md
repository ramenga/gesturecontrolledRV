# gesturecontrolledRV
This is basically a Gesture Controlled Robotic Vehicle project from my junior college days(2014).

I am uploading old codes so the code may not be always clean.

Only basic functionality has been achieved and left at that,so it may not always work in all environments.

Several open source libraries (Adafruit,MotionApps,NRF24L01,etc.) are used.

Open source code snippets are also used, and I may not always mention the source. Please inform me if your code is present.

Developed on Arduino-Uno and Duemilanove.


It consists of three sketches for the Tx, Rx and the Motor-Driver.

Tx consists of the Remote Control incorporating AtMega328 running Arduino,MPU6050,HMC8553 and NRF24L01. The sketch communicates with these components.

Rx consists of the Receiver and incorporates NRF24L01 and HMC8553. Sends commands through Serial COM to the Motor-Driver.

Motor-Driver is used for driving the pins of H-Bridges for each individual wheel-driving motor. Also performs speed updating using PWM to vary power delivered to the wheels.
