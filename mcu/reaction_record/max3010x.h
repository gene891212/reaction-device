/*
Arduino-MAX30100 oximetry / heart rate integrated sensor library
Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

// #define REPORTING_PERIOD_MS 1000

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

// uint32_t tsLastReport = 0;

// // Callback (registered below) fired when a pulse is detected
// void onBeatDetected()
// {
//     Serial.println("Beat!");
// }

void setupMax3010x() {
  Wire.begin(8, 7);
  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("[ERROR] Initializing PulseOximeter FAILED");
    for (;;)
      ;
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
  // pox.setOnBeatDetectedCallback(onBeatDetected);
}

void getMax3010x(int* data) {
  int heartrate = pox.getHeartRate() / 2;
  int spo2 = pox.getSpO2();

  if (heartrate < 50) {
    heartrate = 60;
  }
  if (spo2 == 0) {
    heartrate = 0;
  }
  data[0] = heartrate;
  data[1] = spo2;

  Serial.print("Heart rate:");
  Serial.print(heartrate);
  Serial.print("bpm / SpO2:");
  Serial.print(spo2);
  Serial.println("%");
}
