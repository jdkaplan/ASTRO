def generateChecksum(gps):
    return '*{0:X}'.format(reduce(lambda x,y: x^ord(y), gps, 0))

# # Testing
# print generateChecksum('GPGGA,202212.00,3024.7205,N,09110.7264,W,1,06,1.69,999999.9,M,-025,M,,')
# print generateChecksum('GPGGA,002000.00,,,,,1,,,1100,,,,,')
