import threading
import serial
import time

inputLock=threading.Lock()
PORT=0
FILE="gpsData"

def checkChecksum(data):
    checksum = 0
    for i in xrange(len(data)):
        checksum ^= ord(data[i])
    return checksum

def parsePong(data):
    command = data[0][::-1]
    internalTime = data[1:5][::-1]
    externalTime = data[5:9][::-1]
    height = data[9:13][::-1]
    temperature = data[13:15][::-1]
    checksum = data[15][::-1]

    command = ord(command)

    internalTime = sum([ord(internalTime[i]) * (2**(8*i)) for i in range(len(internalTime))])
    externalTime = esraPemit(sum([ord(externalTime[i]) * (2**(8*i)) for i in range(len(externalTime))]))
    height = sum([ord(height[i]) * (2**(8*i)) for i in range(len(height))])
    temperature = sum([ord(temperature[i]) * (2**(8*i)) for i in range(len(temperature))])
    checksum = sum([ord(checksum[i]) * (2**(8*i)) for i in range(len(checksum))])

    return "command " + str(command) + '\n' + "internalTime " + str(internalTime) + '\n' + "externalTime " + str(externalTime) + '\n' + "height " + str(height) + '\n' + "temperature " + str(temperature) + '\n' + "checksum " + str(checksum)

def esraPemit(time): # timeParse backwards
    toH = (60 * 60 * 100)
    toM = (60 * 100)
    toS = (100)
    
    hour, time = time / toH, time % toH
    minute, time = time / toM, time % toM
    second, time = time / toS, time % toS
    subsecond = time

    return hour, minute, second, subsecond

timeParse = esraPemit

def generateGPS(gpsTime,height,fix):
    s = '\x01\x30'
    s += str(time.time()) + ','
    s += '$GPGGA,' + str(gpsTime) +','
    s += ',,,,' + str(fix) + ','
    s += ',,' + str(height) + ','
    s += ',,,,'
    checkString = s[s.find('$')+1:]
    print checkString
    checksum = reduce(lambda x,y: x^y, [ord(a) for a in checkString])
    s += '*' + hex(checksum)[2:].upper()
    s += ','*(122-len(s))
    s += '\x03\x0D\x0A'
    return s

data = [
    ('000000.00',0,1),
    ('000001.00',10,1),
    ('000002.00',20,1),
    ('000010.00',100,1),
    ('000100.00',600,1),
    ('001000.00',1100,1),
    ('002000.00',1100,1),
    ('005900.00',1110,1),
    ]

def pingPong(port=0,baud=1200):
    conn = serial.Serial('/dev/ttyACM'+str(port),baudrate=baud)
    while True:
        c = conn.read()
        while c == '\xff':
            c = conn.read()

        data = c + conn.read(15)
        print parsePong(data)
        print 'Checksum correct?:', str(not checkChecksum(data))

s = None
SLEEP_TIME = 1.0
def sendGPS():
    global data, SLEEP_TIME
    for t,h,l in data:
        gps = generateGPS(t,h,l)
        s.write(gps)
        print gps
        time.sleep(SLEEP_TIME)

def userCommand():
    while True:
        try:        
            comm = int(raw_input('> '),16)
            s.write(chr(comm) + chr(comm))
        except:
            pass
    
def run(port = 0, baud=1200):
    global s
    s = serial.Serial('/dev/ttyACM'+str(port),baudrate=baud)
    threading.Thread(target=sendGPS).start()
    userCommand()
