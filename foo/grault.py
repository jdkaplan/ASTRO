import numpy

class Object(object):
    pass

interp1 = numpy.interp 
diff = lambda x: x[1] - x[0]

hasp = Object()
astro = Object()

# temperature.m

## THERMISTOR MODEL
#
# These coefficients are for the 100K6MRBD1 
# from Betatherm, 100k Ohm, beta = 4261.
# Datasheet R and T data defined over -55C 
# to 150C.
 
T = range(-55,151)
R = [12162871, 11266450, 10441605, 9682176, 8982600, 8337869, 7743400, 7194826, 6688364, 6220553, 5788455, 5388878, 5019313, 4677268, 4360636, 4067212, 3795342, 3543286, 3309422, 3092416, 2890843, 2703671, 2529672, 2367900, 2217423, 2077394, 1947006, 1825568, 1712400, 1606911, 1508530, 1416745, 1331059, 1251079, 1176328, 1106485, 1041173, 980100, 922956, 869458, 819378, 772463, 728492, 687276, 648624, 612366, 578321, 546376, 516372, 488178, 461683, 436773, 413344, 391294, 370547, 351017, 332619, 315288, 298959, 283558, 269041, 255337, 242414, 230210, 218688, 207807, 197521, 187803, 178613, 169924, 161702, 153923, 146560, 139588, 132984, 126729, 120799, 115179, 109850, 104796, 100000, 95449, 91128, 87026, 83129, 79428, 75912, 72567, 69389, 66365, 63489, 60752, 58149, 55668, 53307, 51058, 48915, 46873, 44927, 43071, 41301, 39613, 38003, 36465, 34999, 33598, 32260, 30983, 29761, 28595, 27479, 26413, 25394, 24419, 23486, 22593, 21739, 20921, 20138, 19388, 18669, 17981, 17321, 16689, 16083, 15502, 14944, 14410, 13897, 13405, 12932, 12479, 12043, 11625, 11223, 10837, 10466, 10110, 9767.6, 9438.1, 9121.4, 8816.9, 8523.8, 8241.9, 7970.7, 7709.7, 7458.3, 7216.3, 6983.4, 6758.9, 6542.7, 6334.5, 6133.8, 5940.5, 5754, 5574.3, 5401.1, 5234.1, 5072.9, 4917.3, 4767.3, 4622.6, 4482.9, 4348.1, 4217.8, 4092.1, 3970.7, 3853.5, 3740.2, 3630.7, 3525, 3422.8, 3324, 3228.5, 3136.2, 3046.9, 2960.6, 2877, 2796.2, 2718, 2642.4, 2569.2, 2498.3, 2429.7, 2363.3, 2298.9, 2236.7, 2176.3, 2117.9, 2061.3, 2006.4, 1953.2, 1901.6, 1851.7, 1803.3, 1756.3, 1710.8, 1666.6, 1623.8, 1582.2, 1541.9, 1502.8, 1464.8, 1428, 1392.2, 1357.5]
 
## HASP Analog Output Design
hasp.R_EXT = 402000.         # external (linearizing) resistor resistance [Ohms]
hasp.V_REF = 4.096           # Reference voltage used to energize thermistors [V] (Voltage output to HASP)
hasp.A2DBits = 12            # Number of bits in A2D conversion (Assumed worst case for HASP)
hasp.VoltageRange = [0, 5]   # Allowable input voltage range
hasp.T = T
hasp.V = [hasp.R_EXT / rho * hasp.V_REF for rho in [(r + hasp.R_EXT)for r in R]]
 
## HASP ASTRO payload design (MSP430)
astro.R_EXT = 402000.
astro.V_REF = 3.3              # Reference voltage used to energize thermistors [V] (MSP430 circuit)
astro.A2DBits = 10             # Number of bits in A2D conversion (MSP430)
astro.VoltageRange = [0, 3.6]  # Allowable input voltage range
astro.T = T;
astro.V = [astro.R_EXT / rho * astro.V_REF for rho in [(r + astro.R_EXT)for r in R]]

def temperatureFinding(motor1, motor2, board): 
    return motorTemperature(motor1, motor2) + boardTemperature(board)

def boardTemperature(temperature):
    ADC_ASTRO = temperature
    ADC_ASTRO = float(ADC_ASTRO)
    V_ASTRO = ADC_ASTRO / 2**(astro.A2DBits) * diff(astro.VoltageRange)
    T_ASTRO = interp1(V_ASTRO, astro.V, astro.T)
    return [T_ASTRO]

def motorTemperature(motor1, motor2):
    V_HASP = [motor1, motor2]
    T_HASP = interp1(V_HASP, hasp.V, hasp.T)
    return list(T_HASP)
