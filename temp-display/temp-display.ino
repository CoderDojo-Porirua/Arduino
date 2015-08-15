// This code taken from https://learn.sparkfun.com/tutorials/bmp180-barometric-pressure-sensor-hookup-

/* SFE_BMP180 library example sketch

This sketch shows how to use the SFE_BMP180 library to read the
Bosch BMP180 barometric pressure sensor.
https://www.sparkfun.com/products/11824

Like most pressure sensors, the BMP180 measures absolute pressure.
This is the actual ambient pressure seen by the device, which will
vary with both altitude and weather.

Before taking a pressure reading you must take a temparture reading.
This is done with startTemperature() and getTemperature().
The result is in degrees C.

Once you have a temperature reading, you can take a pressure reading.
This is done with startPressure() and getPressure().
The result is in millibar (mb) aka hectopascals (hPa).

If you'll be monitoring weather patterns, you will probably want to
remove the effects of altitude. This will produce readings that can
be compared to the published pressure readings from other locations.
To do this, use the sealevel() function. You will need to provide
the known altitude at which the pressure was measured.

If you want to measure altitude, you will need to know the pressure
at a baseline altitude. This can be average sealevel pressure, or
a previous pressure reading at your altitude, in which case
subsequent altitude readings will be + or - the initial baseline.
This is done with the altitude() function.

Hardware connections:

- (GND) to GND
+ (VDD) to 3.3V

(WARNING: do not connect + to 5V or the sensor will be damaged!)

You will also need to connect the I2C pins (SCL and SDA) to your
Arduino. The pins are different on different Arduinos:

Any Arduino pins labeled:  SDA  SCL
Uno, Redboard, Pro:        A4   A5
Mega2560, Due:             20   21
Leonardo:                   2    3

Leave the IO (VDDIO) pin unconnected. This pin is for connecting
the BMP180 to systems with lower logic levels such as 1.8V

Have fun! -Your friends at SparkFun.

The SFE_BMP180 library uses floating-point equations developed by the
Weather Station Data Logger project: http://wmrx00.sourceforge.net/

Our example code uses the "beerware" license. You can do anything
you like with this code. No really, anything. If you find it useful,
buy me a beer someday.

V10 Mike Grusin, SparkFun Electronics 10/24/2013
V1.1.2 Updates for Arduino 1.6.4 5/2015

*/

// Your sketch must #include this library, and the Wire library.
// (Wire is a standard library included with Arduino.):

#include <Adafruit_NeoPixel.h>

#include <SFE_BMP180.h>
#include <Wire.h>

// You will need to create an SFE_BMP180 object, here called "pressure":

SFE_BMP180 pressure;

#define PIN 8

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      16

 
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);


void setup()
{
  Serial.begin(9600);
  Serial.println("REBOOT");

  // Initialize the sensor (it is important to get calibration values stored on the device).

  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'

  if (pressure.begin()){
    Serial.println("BMP180 init success");

    setAllPixels(0,25,0);    
    delay(500);
    setAllPixels(0,0,25);    
    delay(500);
    setAllPixels(25,25,25);    
    delay(500);
    setAllPixels(0,0,0);    
    delay(500);
  }
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    setAllPixels(25,0,0);
    

    while(1); // Pause forever.
  }
}

void loop()
{
  char status;
  double T;
  
  // Loop here getting pressure readings every 10 seconds.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C\n");
      
      setAllPixels(temperatureToColour(T));
      //pixels.setBrightness(150);
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  //delay(500);  // Pause for 5 seconds.
}

void setAllPixels(int r, int g, int b)
{
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(r,g,b)); 
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
}

void setAllPixels(uint32_t colour)
{
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, colour); 
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
}

uint32_t temperatureToColour(double temperature){
  // 0 - 17 - Blue
  // 18-22 Green
  // 23 - 35 Red
  double greenMin = 17.0;
  double greenMax = 27.0;
  int red = 0;
  int green = 0;
  int blue = 0;
  

  if(temperature<greenMin){
    red = 0;
    green = 0;
    blue = 255;
  } else
  {
    if(temperature>greenMax){
      red = 255;
      green = 0;
      blue = 0;      
    } else
    {

      double greenRange = greenMax - greenMin;
      // Convert temp into number between 0 and 1.
      double temperatureGreenRange = (temperature - greenMin) / (greenRange);
      // 0 -> 0.5 blue -> green
      // 0.5 -> 1 green -> red

      blue = 63 * (1.0 -(min((temperatureGreenRange/0.5), 1.0)));
      red = 63 * (max(((temperatureGreenRange-0.5)/0.5), 0));
      green = 63 - blue - red;
    }
  }
  
  return pixels.Color(red, green, blue);

}

