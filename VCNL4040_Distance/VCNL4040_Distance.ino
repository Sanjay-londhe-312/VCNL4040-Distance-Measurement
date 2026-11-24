
#include <Wire.h>
#include "SparkFun_VCNL4040_Arduino_Library.h"

VCNL4040 proximitySensor;

// Calibration data
// Distance in mm
const float distanceTable[] = {
  10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160
};

// Average raw proximity readings
const float proximityTable[] = {
  12027, 2968.1, 1258.1, 646.9, 389.7, 267.1, 191.1, 143.6, 111.9, 88.8, 73, 61.8, 52, 44, 37, 32
};

const int TABLE_SIZE = 16;

// Convert raw proximity into estimated distance
float calculateDistance(float rawValue)
{
  // Outside calibrated range
  if (rawValue > proximityTable[0] ||
      rawValue < proximityTable[TABLE_SIZE - 1])
  {
    return -1;
  }

  // Find nearest calibration interval
  for (int i = 0; i < TABLE_SIZE - 1; i++)
  {
    float p1 = proximityTable[i];
    float p2 = proximityTable[i + 1];

    if (rawValue <= p1 && rawValue >= p2)
    {
      float d1 = distanceTable[i];
      float d2 = distanceTable[i + 1];

      // Linear interpolation
      float distance = d1 +
        ((p1 - rawValue) / (p1 - p2)) * (d2 - d1);

      return distance;
    }
  }

  return -1;
}

// Average multiple sensor readings
float getAverageProximity()
{
  unsigned long total = 0;

  const int samples = 5;

  for (int i = 0; i < samples; i++)
  {
    total += proximitySensor.getProximity();
    delay(5);
  }

  return (float)total / samples;
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  Serial.println("VCNL4040 Distance Measurement");

  if (!proximitySensor.begin())
  {
    Serial.println("Sensor not found!");
    while (1);
  }
}

void loop()
{
  float rawValue = getAverageProximity();

  float distance = calculateDistance(rawValue);

  Serial.print("Raw Proximity: ");
  Serial.print(rawValue, 1);

  Serial.print(" | Estimated Distance: ");

  if (distance >= 0)
  {
    Serial.print(distance, 2);
    Serial.println(" mm");
  }
  else
  {
    Serial.println("Outside calibrated range");
  }

  delay(100);
}
