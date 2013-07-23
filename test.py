import serial
import time

def connect(i):
    return serial.Serial('/dev/ttyACM'+str(i),baudrate=1200)

def status(conn):
    r = conn.read(10)
    print 'Time (MS):', ord(r[0])*0xFFFFFF + ord(r[1])*0xFFFF + ord(r[2])*0xFF + ord(r[3])
    print 'Height (GPS):', ord(r[4])*0xFFFFFF + ord(r[5])*0xFFFF + ord(r[6])*0xFF + ord(r[7])
    print 'Command:',ord(r[8])
    print 'Checks?'

def showTemp(s):
   for i in xrange(10000):
     a = s.write('\x07\x07')
     a = s.read(10)
     a = s.read()
     b = s.read()
     t = ord(b)*256 + ord(a)
     print float(t-11)/(1024.)*3.601,t
     time.sleep(1)

def esraPemit(time): # timeParse backwards
    toH = (60 * 60 * 100)
    toM = (60 * 100)
    toS = (100)
    
    hour, time = time / toH, time % toH
    minute, time = time / toM, time % toM
    second, time = time / toS, time % toS
    subsecond = time

    return hour, minute, second, subsecond

def parsePong(data):
    command = data[0][::-1]
    internalTime = data[1:5][::-1]
    externalTime = data[5:9][::-1]
    height = data[9:13][::-1]
    temperature = data[13:15][::-1]
    checksum = data[15][::-1]

    command = ord(command)

    internalTime = sum([ord(internalTime[i]) * (2**(8*i)-1) for i in range(len(internalTime))])
    externalTime = esraPemit(sum([ord(externalTime[i]) * (2**(8*i)-1) for i in range(len(externalTime))]))
    height = sum([ord(height[i]) * (2**(8*i)-1) for i in range(len(height))])
    temperature = sum([ord(temperature[i]) * (2**(8*i)-1) for i in range(len(temperature))])
    checksum = sum([ord(checksum[i]) * (2**(8*i)-1) for i in range(len(checksum))])

    return "command " + str(command) + '\n' + "internalTime " + str(internalTime) + '\n' + "externalTime " + str(externalTime) + '\n' + "height " + str(height) + '\n' + "temperature " + str(temperature) + '\n' + "checksum " + str(checksum)

def pingPong(conn):
    data = conn.read(16)
    print parsePong(conn,data)
    time.sleep(1)
    
    while True:
        c = conn.read()
        while c == 0xFF:
            c = conn.read()
        
        data = [c] + conn.read(15)
        print parsePong(conn,data)
        time.sleep(1)
