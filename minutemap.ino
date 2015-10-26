/*******************************************************
 * MinuteMap class
 * Written for LilyPad Arduino USB / Cactus Micro
 * Assumes ints are 16-bits like on Arduino Uno
 *******************************************************/

const char b64alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

class MinuteMap {
public:
  MinuteMap() {
    reset();
  }
  void setHourMin(int hour, int minStart, int minStop)
  // set minuteMap bits [minStart, minStop)
  {
    if (minStart % 2 != 0)
      minStart -= 1; // snap down to 2 min range
    if (minStop % 2 != 0)
      minStop += 1; // snap up to 2 min range

    minStart = max(minStart, 0);
    minStop = max(minStart, minStop);

    int mmBaseIdx = hour*32;
    int mmStartIdx = mmBaseIdx + minute2mmIdx(minStart);
    int mmStopIdx = mmBaseIdx + minute2mmIdx(minStop);

    for (int i=mmStartIdx; i < mmStopIdx; i++)
      setMinuteMapBit(i);
  }
  void setMinuteMapBit(int idx)
  {
    int mmHourIdx = idx / 32;
    int remainder = idx % 32;
    long bit = 1UL << remainder;
    minuteMap[mmHourIdx] |= bit;
  }
  bool isHourMinSet(int hour, int min)
  {
    int mmIdx = minute2mmIdx(min);
    long bit = 1UL << mmIdx;
    return minuteMap[hour] & bit;
  }
  void printMinuteMap()
  {
    char mmBits[70];
    memset(mmBits, 0, sizeof(mmBits));

    int mmBitsIdx = 0;
    for (int i=0; i < 2; i++)
      for (int j=0; j < 32; j++)
      {
        long bit = 1UL << j;
        if (minuteMap[i] & bit)
          mmBits[mmBitsIdx++] = '1';
        else
          mmBits[mmBitsIdx++] = '0';
      }

    Serial.println(mmBits);
  }
  void reset() 
  {
    minuteMap[0] = minuteMap[1] = 0;
  }
  void setWithBase64code(const char *b64code)
  {
    reset();
    
    int mmIdx = 0;
    const int codeLen = strlen(b64code);
    for (int i=0; i < codeLen; i++)
    {
      char c = b64code[i];
      int val = getBase64letterVal(c);
      if (val == -1)
        continue; // bad letter

      // scan bits of base64 lettter
      // and set bits in minuteMap
      for (int b=0; b < 6; b++, mmIdx++)
        if (val & (1UL << b))
          setMinuteMapBit(mmIdx);
    }
  }
  int getBase64letterVal(char c) 
  {
    for (int val=0; val < strlen(b64alpha); val++)
      if (b64alpha[val] == c)
        return val;
    
    return -1;
  }
  void getMinuteMapBase64code(char *code)
  // Note: code buf needs to be large enough; I'm not checking sizes
  {
    int codeIdx = 0;

    /*
     * Iterate through the 32+32 bits that represent
     * the minute intervals in the 2 hours we're tracking.
     * Convert each 6-bit chunk into a base64 letter.
     */
    int b64letterVal = 0;
    int b64bitIdx = 0;
    for (int i=0; i < 2; i++)
      for (int j=0; j < 32; j++)
      {
        long bit = 1UL << j;
        if (minuteMap[i] & bit)
          b64letterVal |= 1 << b64bitIdx;

        b64bitIdx = (b64bitIdx+1) % 6;

        if (b64bitIdx == 0)
        {
          // we've visted 6 bits 
          // -- output the completed base64 letter
          code[codeIdx++] = b64alpha[b64letterVal];
          b64letterVal = 0; // reset
        }
      }

    if (b64bitIdx != 0)
    {
      // output final base64 letter
      code[codeIdx++] = b64alpha[b64letterVal];
    }

    code[codeIdx] = '\0'; // NULL-terminate string
  }

  int minute2mmIdx(int minute)
  // [0,60) -> [0, 32)
  {
    int idx = 0;
    int quadrant = minute / 15;
    idx += quadrant * 8; // we use 8 bits to track every 15 minutes

    int remainMins = minute % 15;
    idx += remainMins / 2; // we use 1 bit to track 2-minute slots

    return idx;
  }

private:
  long minuteMap[2]; // 2 x 32 bits 
                    // = 2-minute chunks over 2 hours 
};

