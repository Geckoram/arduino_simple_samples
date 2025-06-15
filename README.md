# arduino_simple_samples
Simple drums using the mozzi library and samples

## Installation and Setting up:
To load the program into your arduino (any arduino that is supported by Mozzi), you'll need to install mozzi via the arduino IDE libraries, and then add de samples provided into the "samples/drums" folder in mozzi (in /home/username/Arduino/libraries/Mozzi/samples). 
For the hardware, you can plug the hot of a jack into the output pin (see defaults [here](https://sensorium.github.io/Mozzi/#supported-boards-output-modes-and-default-pins)), although I reccomend setting up a RC low pass filter ([this](https://en.wikipedia.org/wiki/File:1st_Order_Lowpass_Filter_RC.svg) schematic, I found that a 103 capacitor and a 10-50k resistor work ok) between the arduino and the output. 
