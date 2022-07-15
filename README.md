# shift-indicator

**This project is in developement**

The idea is to create race shift indicator for autosimulators based on arduino and rgb led strip. One of main goals was to be able to create smooth gradient color map for rgb led strip.

Current features:
- using fastLed library, so many types of led strip are supported
- capability to create a color map for the led strip

Example gradients created with the algorythm from color maps:
![gradient 1](https://eldar.bagirov.ru/res/grad1.jpg)
![gradient 2](https://eldar.bagirov.ru/res/grad2.jpg)


Features to be added:
- com port connection for real-time data transfer (current RPM)
- better fill animation
- PC client for color management and data delivery
