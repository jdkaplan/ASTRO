import serial

def connect(i):
    return serial.Serial('/dev/ttyACM'+str(i),baudrate=1200)

def status(conn):
    r = conn.read(10)
    print 'Time (MS):', ord(r[0])*0xFFFFFF + ord(r[1])*0xFFFF + ord(r[2])*0xFF + ord(r[3])
    print 'Height (GPS):', ord(r[4])*0xFFFFFF + ord(r[5])*0xFFFF + ord(r[6])*0xFF + ord(r[7])
    print 'Command:',ord(r[8])
    print 'Checks?'
