import threading
import serial
import time
import grault
import os
import datetime
import time

inputLock=threading.Lock()
PORT=0
FILE="gpsData"

def connect(i, kind="USB"):
    return serial.Serial('/dev/tty'+kind+str(i),baudrate=1200)

def checkChecksum(data):
    checksum = reduce(lambda x,y: x^ord(y), data, 0)
    return not checksum

def generateChecksum(gps):
    return '*{:0>2X}'.format(reduce(lambda x,y: x^ord(y), gps, 0))

def makeNumber(bytes_):
    rev = bytes_[::-1]
    return sum([ord(rev[j]) * (2**(8*j)) for j in range(len(rev))])

def parsePong(data):
    command = data[0]
    internalTime = data[1:5]
    externalTime = data[5:9]
    height = data[9:13]
    temperature = data[13:15]
    motorOne = data[15:17]
    motorTwo = data[17:19]
    safemode = data[19]
    heaterOne = data[20]
    heaterTwo = data[21]
    HVDCOne = data[22]
    HVDCTwo = data[23]
    checksum = data[24]

    command = makeNumber(command)
    internalTime = makeNumber(internalTime)
    externalTime = esraPemit(makeNumber(externalTime))
    height = makeNumber(height)
    temperature = makeNumber(temperature)
    motorOne = makeNumber(motorOne)
    motorTwo = makeNumber(motorTwo)
    safemode = makeNumber(safemode)
    heaterOne = makeNumber(heaterOne)
    heaterTwo = makeNumber(heaterTwo)
    HVDCOne = makeNumber(HVDCOne)
    HVDCTwo = makeNumber(HVDCTwo)
    checksum = makeNumber(checksum)
    
    output = "command " + str(command) + '\n'
    output += "internalTime " + str(internalTime) + '\t\t(approx. {:.2f}s)'.format(internalTime / 1024.) + '\n'
    output += "externalTime " + str(externalTime) + '\n'
    output += "height " + str(height) + '\t\t(approx. {:.2f}ft)'.format(height * 3.28084) + '\n'
    output += "temperature " + str(temperature) + '\t\t(approx. {:.2f} C)'.format(grault.boardTemperature(temperature)) + '\n'
    output += "motorOne " + str(motorOne) + '\n'
    output += "motorTwo " + str(motorTwo) + '\n'
    output += "safemode " + str(safemode) + '\n'
    output += "heaterOne " + str(heaterOne) + '\n'
    output += "heaterTwo " + str(heaterTwo) + '\n'
    output += "HVDCOne " + str(HVDCOne) + '\n'
    output += "HVDCTwo " + str(HVDCTwo) + '\n'
    output += "checksum " + str(checksum)
    
    return output

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
    s += 'GPGGA,' + str(gpsTime) +','
    s += ',,,,' + str(fix) + ','
    s += ',,' + str(height) + ','
    s += ',,,,'
    checkString = s[s.find(',')+1:]
    checksum = generateChecksum(checkString)
    s += checksum
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

def pingPong(port=0,baud=1200, kind='USB'):
    logfile = raw_input('Logfile? (filepath) > ') or None
    # logfile = open(logfile,'a+') if logfile else None

    htmlfile = raw_input('HTML? (filepath) > ') or None
    # htmlfile = open(htmlfile,'w') if htmlfile else None

    conn = serial.Serial('/dev/tty'+kind+str(port),baudrate=baud)
    while True:
        c = conn.read()
        while c == '\xff':
            c = conn.read()

        data = c + conn.read(24)

        parsed = parsePong(data)
        checked =  'Checksum correct?: ' + str(checkChecksum(data)) + '\n'
        print parsed
        print checked

        if logfile:
            with open(logfile,'a+') as log:
                log.write(parsed)
                log.write('\n')
                log.write(checked)
                log.write('\n')
        
        if htmlfile:
            with open(htmlfile,'w') as html:
                html.write("<!DOCTYPE html>\n<html>\n<title>ASTRO HASP</title><head>\n")
                html.write('<style type="text/css">p{margin:0;}</style>')
                html.write("</head>\n<body>\n<h1>ASTRO HASP</h1>\n")
                html.write('<p>' + parsed.replace('\n','</p>\n<p>').replace('\t','&emsp;&emsp;') + '</p>\n')
                html.write('<p>' + checked + '</p>\n')
                html.write('<p>Last updated: ')
                # make datetime obj from current time, convert to human-readable, write to file
                html.write(datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S'))
                html.write('</p>\n')
                html.write('<br />')
                if logfile:
                    html.write('<p><a href="' + logfile.name + '">ASTRO log file</a>')
                html.write('<p><a href="http://laspace.lsu.edu/hasp/xml/data_gps_v6.2.1.php?py=2013">Flight Tracker</a></p>\n')
                html.write('<p><a href="http://laspace.lsu.edu/hasp/xml/data_adc.php?py=2013">Environmental Status</a></p>\n')
                html.write('</body>\n</html>')

s = None
SLEEP_TIME = 1.0
def sendGPS():
    global data, SLEEP_TIME
    for t,h,l in data:
        gps = generateGPS(t,h,l)
        with inputLock:
            s.write(gps)
        time.sleep(SLEEP_TIME)

def userCommand():
    while True:
        try:
            comm = raw_input('> ')
            
            if comm.lower() == "gps":
                t = raw_input('GPSTime?\t> ')
                h = raw_input('Height?\t\t> ')
                l = raw_input('Fix Type?\t> ')
                toSend = generateGPS(t,h,l)
                print toSend
            else:
                    comm = int(comm, 16)
                    toSend = chr(comm) + chr(comm)
            with inputLock:
                s.write(toSend)
        except (ValueError,TypeError):
            print comm, "is not a valid value"
        except (EOFError,KeyboardInterrupt): # ^D,^C
            print "Keyboard kill"
            return

def run(port = 0, baud=1200, kind='USB'):
    global s
    s = serial.Serial('/dev/tty'+kind+str(port),baudrate=baud)
    #threading.Thread(target=sendGPS).start()
    userCommand()
