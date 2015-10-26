class MinuteMap:
  BASE64ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
  def __init__(self):
    self.minuteMap = [0,0]
    self.reset();

  def reset(self):
    self.minuteMap = [0,0]

  def setHourMin(self, hour, minStart, minStop):
    # set minuteMap bits [minStart, minStop)
  
    if minStart % 2 != 0:
      minStart -= 1 # snap down to 2 min range
    if minStop % 2 != 0:
      minStop += 1 # snap up to 2 min range

    minStart = max(minStart, 0)
    minStop = max(minStart, minStop)

    mmBaseIdx = hour*32
    mmStartIdx = mmBaseIdx + self.minute2mmIdx(minStart)
    mmStopIdx = mmBaseIdx + self.minute2mmIdx(minStop)

    for i in range(mmStartIdx, mmStopIdx):
      self.setMinuteMapBit(i)

  def setMinuteMapBit(self, idx):
    mmHourIdx = idx / 32
    remainder = idx % 32
    bit = 1 << remainder
    self.minuteMap[mmHourIdx] |= bit

  def minute2mmIdx(self, minute):
    # [0,60) -> [0, 32)
   
    idx = 0
    quadrant = minute / 15
    idx += quadrant * 8 # we use 8 bits to track every 15 minutes

    remainMins = minute % 15
    idx += remainMins / 2 # we use 1 bit to track 2-minute slots

    return idx

  def isHourMinSet(self, hour, minute):
    mmIdx = self.minute2mmIdx(minute)
    bit = 1 << mmIdx
    return self.minuteMap[hour] & bit

  def printMinuteMap(self):
    toPrint = ""
    for i in range(0,2):
      for j in range(0,32):
        bit = 1 << j
        if self.minuteMap[i] & bit:
          toPrint += "1"
        else:
          toPrint += "0"
    print toPrint

  def setWithBase64code(self, b64code):
    self.reset()
    
    mmIdx = 0
    codeLen = len(b64code)
    for i in range(0, codeLen):
      c = b64code[i]
      val = self.getBase64letterVal(c)
      if val == -1:
        continue # bad letter

      # scan bits of base64 lettter
      # and set bits in minuteMap
      for b in range (0, 6):
        if val & 1 << b:
          self.setMinuteMapBit(mmIdx)
        mmIdx += 1
   
  def getBase64letterVal(self, c):
    b64alpha = MinuteMap.BASE64ALPHA
    for val in range(0, len(b64alpha)):
      if b64alpha[val] == c:
        return val
    
    return -1

  def getMinuteMapBase64code(self):
    b64alpha = MinuteMap.BASE64ALPHA
    code = ""
    #
    # Iterate through the 32+32 bits that represent
    # the minute intervals in the 2 hours we're tracking.
    # Convert each 6-bit chunk into a base64 letter.
    #
    b64letterVal = 0
    b64bitIdx = 0
    for i in range(0, 2):
      for j in range(0, 32):
        bit = 1 << j
        if self.minuteMap[i] & bit:
          b64letterVal |= 1 << b64bitIdx

        b64bitIdx = (b64bitIdx+1) % 6

        if b64bitIdx == 0:
          # we've visted 6 bits 
          # -- output the completed base64 letter
          code += b64alpha[b64letterVal]
          b64letterVal = 0 # reset

    if b64bitIdx != 0:
      # output final base64 letter
      code += b64alpha[b64letterVal]

    return code
