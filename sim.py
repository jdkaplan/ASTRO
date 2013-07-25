import threading
import serial
import time

inputLock=threading.Lock()
PORT=0
FILE="gpsData"

def generateSting(gpsTime,height,fix):
    s = '\x01\x30'
    s += str(time.time()) + ','
    s += '$GPGGA,' + str(gpsTime) +','
    s += ',,,,' + str(fix) + ','
    s += ',,' + str(height) + ','
    s += ',,,,'
    checkString = s[s.find('$')+1:]
    checksum = reduce(lambda x,y: x^y, [ord(a) for a in checkString])
    s += '*' + hex(checksum)[2:]
    s += ','*(122-len(s))
    s += '\x03\x0D\x0A'
    return s

def sendGPS():
    pass
    
def userCommand():
    comm = raw_input('> ')
    
