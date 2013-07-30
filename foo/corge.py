import time
import random
import sys

random.seed(0)

def generateChecksum(data):
    return chr(reduce(lambda x,y: x^ord(y), data, 0))

def generateLog(state=None):
    if state:
        t = [
            state["command"],

            state["internalTime"] >> 24 & 0xFF,
            state["internalTime"] >> 16 & 0xFF,
            state["internalTime"] >>  8 & 0xFF,
            state["internalTime"] >>  0 & 0xFF,

            state["externalTime"] >> 24 & 0xFF,
            state["externalTime"] >> 16 & 0xFF,
            state["externalTime"] >>  8 & 0xFF,
            state["externalTime"] >>  0 & 0xFF,

            state["height"] >> 24 & 0xFF,
            state["height"] >> 16 & 0xFF,
            state["height"] >>  8 & 0xFF,
            state["height"] >>  0 & 0xFF,

            state["temperature"] >>  8 & 0xFF,
            state["temperature"] >>  0 & 0xFF,

            state["desiredStepOne"] >>  8 & 0xFF,
            state["desiredStepOne"] >>  0 & 0xFF,

            state["desiredStepTwo"] >>  8 & 0xFF,
            state["desiredStepTwo"] >>  0 & 0xFF,

            state["safemode"] >>  0 & 0xFF,

            state["heaterOne"] >>  0 & 0xFF,
            state["heaterTwo"] >>  0 & 0xFF,

            state["HVDCOne"] >>  0 & 0xFF,
            state["HVDCTwo"] >>  0 & 0xFF,
            ]

        out = [chr(i) for i in t]

    else:
        out = [chr(random.randint(0,0xFF)) for i in xrange(24)]

    out.append(generateChecksum(out))

    while len(out) < 38:
        out.append(chr(0xFF))

    return ''.join(out)

def randomBytes(num):
    return random.randint(0,2**(8*num))

def generateState(): # currently not used
    state = {}

    state["command"] = int(raw_input("command: "))
    state["internalTime"] = int(raw_input("internalTime: "))
    state["externalTime"] = int(raw_input("externalTime: "))
    state["height"] = int(raw_input("height: "))
    state["temperature"] = int(raw_input("temperature: "))
    state["desiredStepOne"] = int(raw_input("desiredStepOne: "))
    state["desiredStepTwo"] = int(raw_input("desiredStepTwo: "))
    state["safemode"] = int(raw_input("safemode: "))
    state["heaterOne"] = int(raw_input("heaterOne: "))
    state["heaterTwo"] = int(raw_input("heaterTwo: "))
    state["HVDCOne"] = int(raw_input("HVDCOne: "))
    state["HVDCTwo"] = int(raw_input("HVDCTwo: "))

    return state

def generateFile(name, logCount, state=None):
    with open(name,'w') as f:
        for i in xrange(logCount):
            zeta = generateLog(state)
            f.write(zeta)

