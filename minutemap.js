var b64alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

function MinuteMap() {
  this.minuteMap = [0,0];// 2 x 32 bits 
                         // =2-minute chunks over 2 hours 
  this.reset();
}

MinuteMap.prototype.reset = function() {
  for (var i=0 ; i < this.minuteMap.length; i++)
      this.minuteMap[i] = 0;
}

MinuteMap.prototype.setHourMin = function(hour, minStart,  minStop)
// set minuteMap bits [minStart, minStop)
{
  if (minStart % 2 != 0)
    minStart -= 1; // snap down to 2 min range
  if (minStop % 2 != 0)
    minStop += 1; // snap up to 2 min range

  minStart = max(minStart, 0);
  minStop = max(minStart, minStop);

  var mmBaseIdx = hour*32;
  var mmStartIdx = mmBaseIdx + this.minute2mmIdx(minStart);
  var mmStopIdx = mmBaseIdx + this.minute2mmIdx(minStop);

  for (var i=mmStartIdx; i < mmStopIdx; i++)
    this.setMinuteMapBit(i);
}

MinuteMap.prototype.setMinuteMapBit = function(idx)
{
  var mmHourIdx = ~~(idx / 32);
  var remainder = idx % 32;
  var bit = 1 << remainder;
  this.minuteMap[mmHourIdx] |= bit;
}

MinuteMap.prototype.isHourMinSet = function(hour, min)
{
  var mmIdx = this.minute2mmIdx(min);
  var bit = 1 << mmIdx;
  return this.minuteMap[hour] & bit;
}

MinuteMap.prototype.printMinuteMap = function()
{
  var mmBits = "";

  for (var i=0; i < 2; i++)
    for (var j=0; j < 32; j++)
    {
      var bit = 1 << j;
      if (this.minuteMap[i] & bit)
        mmBits += "1";
      else
        mmBits += "0";
    }

  console.log(mmBits);
}

MinuteMap.prototype.setWithBase64code = function(b64code)
{
  this.reset();
  
  var mmIdx = 0;
  var codeLen = b64code.length;
  for (var i=0; i < codeLen; i++)
  {
    var c = b64code[i];
    var val = this.getBase64letterVal(c);
    if (val == -1)
      continue; // bad letter

    // scan bits of base64 lettter
    // and set bits in minuteMap
    for (var b=0; b < 6; b++, mmIdx++)
      if (val & (1 << b))
        this.setMinuteMapBit(mmIdx);
  }
}

MinuteMap.prototype.getBase64letterVal = function(c) 
{
  for (var val=0; val < b64alpha.length; val++)
    if (b64alpha[val] == c)
      return val;
  
  return -1;
}
MinuteMap.prototype.getMinuteMapBase64code = function()
{
  var code = "";

  /*
   * Iterate through the 32+32 bits that represent
   * the minute intervals in the 2 hours we're tracking.
   * Convert each 6-bit chunk into a base64 letter.
   */
  var b64letterVal = 0;
  var b64bitIdx = 0;
  for (var i=0; i < 2; i++)
    for (var j=0; j < 32; j++)
    {
      var bit = 1 << j;
      if (this.minuteMap[i] & bit)
        b64letterVal |= 1 << b64bitIdx;

      b64bitIdx = (b64bitIdx+1) % 6;

      if (b64bitIdx == 0)
      {
        // we've visted 6 bits 
        // -- output the completed base64 letter
        code += b64alpha[b64letterVal];
        b64letterVal = 0; // reset
      }
    }

  if (b64bitIdx != 0)
  {
    // output final base64 letter
    code += b64alpha[b64letterVal];
  }

  return code;
}

MinuteMap.prototype.minute2mmIdx = function(minute)
// [0,60) -> [0, 32)
{
  var idx = 0;
  var quadrant = ~~(minute / 15);
  idx += quadrant * 8; // we use 8 bits to track every 15 minutes

  var remainMins = minute % 15;
  idx += ~~(remainMins / 2); // we use 1 bit to track 2-minute slots

  return idx;
}
