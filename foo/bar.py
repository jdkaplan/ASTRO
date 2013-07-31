import urllib2
from HTMLParser import HTMLParser
from htmlentitydefs import name2codepoint
from time import sleep
import grault

# THESE ARE KIND OF IMPORTANT IF YOU WANT TO KNOW WHERE THINGS ARE GOING

QUEUE = set()
LASTPARSED = "sp6_07-30-13-22-07-55.raw" # null < every string
ALREADYPARSED = set()
CURRENTLYPARSING = "sp6_07-31-13-13-59-48.raw"

# these are just the same functions we had in sim.py (skip to line 80)
# start copypaste

def makeNumber(bytes_):
    rev = bytes_[::-1]
    return sum([ord(rev[j]) * (2**(8*j)) for j in range(len(rev))])

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
    output += "internalTime " + str(internalTime) + '\t\t(approx. {:.2f}s)'.format(internalTime/1024.) + '\n'
    output += "externalTime " + str(externalTime) + '\n'
    output += "height " + str(height) + '\n'
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

def checkChecksum(data):
    checksum = reduce(lambda x,y: x^ord(y), data, 0)
    return not checksum

# this is the HTML parser I had to make for this
class HASPParser(HTMLParser):
    def __init__(self):
        global QUEUE
        QUEUE = set()
        HTMLParser.__init__(self)     # inheritance
        self.useful = False           # useful is a flag for being in a table

    def handle_starttag(self, tag, attrs):
        global ALREADYPARSED
        # if tag is table, data is useful -> useful = True
        if tag == "table":
            self.useful = True
        
        # if we're in a table, and there's a link, we should parse this file
        elif tag == "a" and self.useful:
            # find the link to the *.raw
            for a in attrs:
                if a[1][-4:] == '.raw': # TODO: change to regex
                    file_to_parse = base_url + a[1]
                    #print file_to_parse,
                    
                    # don't parse anything earlier than we parsed before
                    # especially don't parse anything we've already parsed
                    if a[1][11:] > LASTPARSED:
                        if file_to_parse not in ALREADYPARSED:
                            QUEUE.add(file_to_parse)
                            #print '\tAdded'

                    else:
                        pass
                        #print '\tSkipped'

    def handle_endtag(self, tag):
        if tag == "table":
            self.useful = False

year = "2013"
payload = "06"

base_url = 'http://laspace.lsu.edu/hasp/groups/' + year + '/data/'
#base_url = '/home/jdkaplan/documents/astro/foo/'

url = base_url + 'data.php?pname=Payload_' + payload + '&py=' + year
#url = 'file://' + base_url + 'qux.html'

from collections import deque

class dataReader:
    def __init__(self,year="2013",payload="06"):
        self.buffer = deque()
        self.latestPos = 0
        
    def updateBuffer(self):
        global ALREADYPARSED
        # Initial parsing
        html = urllib2.urlopen(url)
        lines = html.readlines()
        inp = ''.join(lines)
        p = HASPParser()
        p.feed(inp)
        fileList = sorted(list(QUEUE-ALREADYPARSED))
        ALREADYPARSED |= set(fileList[:-1])

        # Always assume latestFile is first file in fileList
        # Because you don't add a file that is alphabetically smaller during flight,
        # RIGHT?
        temp = self.latestPos
        for f in fileList:
            f = urllib2.urlopen(f)
            newData = list(f.read()[self.latestPos:])
            temp = self.latestPos + len(newData)
            self.latestPos = 0
            self.buffer.extend(newData)
        self.latestPos = temp

    def read(self,n=1):
        out = []
        sound = False
        while len(self.buffer) < n:
            self.updateBuffer()
            sleep(10)
            sound = True
        if sound:
            pass
        for i in xrange(n):
            out.append(self.buffer.popleft())
        return ''.join(out)
        
def pingPong(stream):
    while True:
        c = stream.read()
        while c == '\xff':
            c = stream.read()

        data = c + stream.read(24)
        print parsePong(data)
        print 'Checksum correct?:', str(checkChecksum(data)), '\n'
        raw_input('> ')