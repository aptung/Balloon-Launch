void getGeiger() {
// Raw data of Radiation Pulse: Not-detected -> High, Detected -> Low
  int sign = digitalRead(SIGN_PIN);

  // Raw data of Noise Pulse: Not-detected -> Low, Detected -> High
  int noise = digitalRead(NOISE_PIN);

  //Radiation Pulse normally keeps low for about 100[usec]
  if (sign == 0 && sON == 0)
  { //Deactivate Radiation Pulse counting for a while
    sON = 1;
    signCount++;
  } else if (sign == 1 && sON == 1) {
    sON = 0;
  }

  //Noise Pulse normally keeps high for about 100[usec]
  if (noise == 1 && nON == 0)
  { //Deactivate Noise Pulse counting for a while
    nON = 1;
    noiseCount++;
  } else if (noise == 0 && nON == 1) {
    nON = 0;
  }

  //Output readings to serial port, after 10000 loops
  if (index == 10000) //About 160-170 msec in Arduino Nano(ATmega328)
  {
    //Get current time
    currTime = millis();

    //No noise detected in 10000 loops
    if (noiseCount == 0)
    {
      //Shift an array for counting log for each 6 sec.
      if ( totalSec % 6 == 0 && cpmIndexPrev != totalSec)
      {
        cpmIndexPrev = totalSec;
        cpmIndex++;

        if (cpmIndex >= 200)
        {
          cpmIndex = 0;
        }

        if (cpmHistory[cpmIndex] > 0)
        {
          cpm -= cpmHistory[cpmIndex];
        }
        cpmHistory[cpmIndex] = 0;
      }

      //Store count log
      cpmHistory[cpmIndex] += signCount;
      //Add number of counts
      cpm += signCount;

      //Get ready time for 10000 loops
      cpmTimeMSec += abs(currTime - prevTime);
      //Transform from msec. to sec. (to prevent overflow)
      if (cpmTimeMSec >= 1000)
      {
        cpmTimeMSec -= 1000;
        //Add measurement time to calcurate cpm readings (max=20min.)
        if ( cpmTimeSec >= 20 * 60 )
        {
          cpmTimeSec = 20 * 60;
        } else {
          cpmTimeSec++;
        }

        //Total measurement time
        totalSec++;
        //Transform from sec. to hour. (to prevent overflow)
        if (totalSec >= 3600)
        {
          totalSec -= 3600;
          totalHour++;
        }
      }

      //Elapsed time of measurement (max=20min.)
      double min = cpmTimeSec / 60.0;
      if (min != 0)
      {
        //Calculate cpm, uSv/h and error of uSv/h
        dtostrf(cpm / min, -1, 3, cpmBuff);
        dtostrf(cpm / min / alpha, -1, 3, uSvBuff);
        dtostrf(sqrt(cpm) / min / alpha, -1, 3, uSvdBuff);
      } else {
        //Devision by zero
        dtostrf(0, -1, 3, cpmBuff);
        dtostrf(0, -1, 3, uSvBuff);
        dtostrf(0, -1, 3, uSvdBuff);
      }

      //Create message for serial port
      sprintf(msg, "%d,%d.%03d,%d,%s,%s,%s",
              totalHour, totalSec,
              cpmTimeMSec,
              signCount,
              cpmBuff,
              uSvBuff,
              uSvdBuff
             );

      //Send message to serial port
      Serial.println(msg);

    }

    //Initialization for next 10000 loops
    prevTime = currTime;
    signCount = 0;
    noiseCount = 0;
    index = 0;
  }
  index++;
}
