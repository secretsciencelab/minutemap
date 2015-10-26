# minutemap
Minute Map: share 2-minute-resolution schedules between embedded devices &amp; RESTful web services. Code for Arduino (embedded side) & Python (REST service side). Passes schedule as a HTTP text-friendly Base64 code.

Example schedule
----------------

    /* 
    * Set Sprinklers
    *
    * Zone A
    * 1: 12 mins
    * 2: 15 mins
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
    char code[50];
    mm.getMinuteMapBase64code(code);
    Serial.println(code); // generate code from current schedule

    mm.setWithBase64code(code);
    mm.printMinuteMap(); // set schedule with given code

Example Python code to get/set schedule using Base64 code
--------------------------------------

    mm = MinuteMap()
    mm.setHourMin(0, 30, 32)
    mm.setHourMin(1, 30, 60)
    code = mm.getMinuteMapBase64code()
    print code

