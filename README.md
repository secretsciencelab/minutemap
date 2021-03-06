# minutemap
Minute Map: share 2-minute-resolution schedules between embedded devices &amp; RESTful web services. Code for Arduino (embedded side), Python (REST backend side) & Javascript (frontend). Passes schedule as HTTP-text-friendly Base64 code.

Example schedule
----------------

    /* 
    * Set Sprinklers
    *
    * Zone A
    * 1: 12 mins
    * 2: 16 mins
    * 3: 18 mins
    * 4: 18 mins
    * 
    * Zone B
    * 5: 14 mins
    * 6: 8 mins
    * 7: 12 mins
    */

Example Arduino code to set schedule programmatically
-------------------------------------

    // Zone A
    MinuteMap mm[8]; // 8 sprinkler channels
    mm[0].setHourMin(0, 0, 12);  // 12 mins
    mm[1].setHourMin(0, 12, 28); // 16 mins
    mm[2].setHourMin(0, 28, 46); // 18 mins
    mm[3].setHourMin(0, 46, 60); // 14 mins
    mm[3].setHourMin(1, 0, 4);   // +4 mins

    // Zone B
    mm[4].setHourMin(1, 4, 18);  // 14 mins
    mm[5].setHourMin(1, 18, 26); // 8 mins
    mm[6].setHourMin(1, 26, 38); // 12 mins

Example Arduino code to get/set schedule using Base64 code
-------------------------------------

    MinuteMap mm;
    mm.setHourMin(0, 0, 12);  // 12 mins

    char code[50];
    mm.getMinuteMapBase64code(code);
    Serial.println(code); // generate code from current schedule

    mm.setWithBase64code(code);
    mm.printMinuteMap(); // set schedule with given code

Example Arduino code to check schedule & fire sprinkler relays
---------------------------------------------------------
    
    #include <Time.h>  
    #define NUM_SPRINKLER_CHANNELS 8
    MinuteMap mm[NUM_SPRINKLER_CHANNELS];
    boolean sprinklerState[NUM_SPRINKLER_CHANNELS];

    void setup() {
      // Not shown: set MinuteMap (mm) with your schedule
    }

    void loop() {
      doSprinkle();
    }
    
    void doSprinkle() 
    {
      int myHour = hour() % 2; 
      int myMin = minute(); 
      // TODO: use real day/hour we want sprinklers to run, 
      // instead of "every two hours"

      // process MinuteMap to set sprinkler states
      for (int i=0; i < NUM_SPRINKLER_CHANNELS; i++)
        if (mm[i].isHourMinSet(myHour, myMin))
          sprinklerState[i] = true;
        else
          sprinklerState[i] = false;

      // TODO process sprinkler states to switch sprinklers on/off
      for (int i=0; i < NUM_SPRINKLER_CHANNELS; i++)
        if (sprinklerState[i])
        {
          // digitalWrite(SPRINKLER_RELAY_i, HIGH)
        }
        else
        {
          // digitalWrite(SPRINKLER_RELAY_i, LOW)
        }
    }

Example Python code to get/set schedule using Base64 code
--------------------------------------

    mm = MinuteMap()
    mm.setHourMin(0, 30, 32)
    mm.setHourMin(1, 30, 60)
    code = mm.getMinuteMapBase64code()
    print code

Example Javascript code to get schedule using Base64 code
-----------------------------------------

    function getMinutesRemaining(scheduleCode, currentTime)
    // E.g., scheduleCode="/AAAAAAAAAA", currentTime=01:30:26
    {
      var mm = new MinuteMap();
      mm.setWithBase64code(scheduleCode);
      mm.printMinuteMap();

      var minutesRemaining = 0;

      var currTimeArr = currentTime.split(":");
      var currHour = currTimeArr[0] % 2;
      var currMin = currTimeArr[1];
      for (var hour=currHour; hour < 2; hour++)
        for (var min=currMin; min <= 60; min++)
          if (mm.isHourMinSet(hour, min))
            minutesRemaining++;
          else
            return minutesRemaining;

      return minutesRemaining;
    }
