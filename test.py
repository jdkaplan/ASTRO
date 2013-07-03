import serial

def connect(i):
    return serial.Serial('/dev/ttyACM'+str(i),baudrate=1200)

def status(conn):
    r = conn.read(10)
    print 'Time (MS):', ord(r[0])*0xFFFFFF + ord(r[1])*0xFFFF + ord(r[2])*0xFF + ord(r[3])
    print 'Height (GPS):', ord(r[4])*0xFFFFFF + ord(r[5])*0xFFFF + ord(r[6])*0xFF + ord(r[7])
    print 'Command:',ord(r[8])
    print 'Checks?'

import time
def showTemp(s):
   for i in xrange(10000):
     a = s.write('\x07\x07')
     a = s.read(10)
     a = s.read()
     b = s.read()
     t = ord(b)*256 + ord(a)
     print float(t)/(1024.)*3.6,t
     time.sleep(1)
