"""testing file for messaging library in python"""
import messaging_library

MIN = b"\x00"
MAX = b"\x7F"

print("Testing bsd checksum")
PAYLOAD = b"\x01\x01"
c = messaging_library.messaging_bsd_checksum(PAYLOAD)
print(c)
R = ""
# correct message
print("Testing correct message")
MES = b"\xCC\x02\x01\x01\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

# incorrect given payload length
print("Testing too short given payload length")
MES = b"\xCC\x02" + MIN + b"\x01\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

print("Testing too long given payload length")
MES = b"\xCC\x02" + MAX + b"\x01\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

print("Testing too short actual payload length")
MES = b"\xCC\x02\x01\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

print("Testing too long actual payload length")
MES = b"\xCC\x02\x01\x01\x01\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

# newlines are included in multi-line strings, must concat strings
print("Testing limit payload length")
MES = b"\xCC\x7F" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01" + \
      b"\xB9\x01\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

print("Testing exceed limit payload length")
MES = b"\xCC\x80" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\x01\x01\x01\x01\x01\x01\x01\x01" + \
      b"\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

# incorrect checksum
print("Testing incorrect checksum")
MES = b"\xCC\x02\x01\x01\x01\xB9" + MIN + b"\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

# fixed character test (HEAD, TAIL, DELIMITERS)
print("Testing incorrect head")
MES = MIN + b"\x02\x01\x01\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

print("Testing incorrect tail")
MES = b"\xCC\x02\x01\x01" + MIN + b"\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

print("Testing incorrect delimiter 1\n")
MES = b"\xCC\x02\x01\x01\xB9\x81" + MIN + b"\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

print("Testing incorrect delimiter 2\n")
MES = b"\xCC\x02\x01\x01\xB9\x81\x0D" + MIN
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

# message structure test
print("Testing incorrect structure")
MES = b"\xCC" + MIN + b"\x02\x01\x01\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

MES = b"\xCC\x02" + MIN + b"\x01\x01\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

MES = b"\xCC\x02\x01\x01" + MIN + b"\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

MES = b"\xCC\x02\x01\x01\xB9" + MIN + b"\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

MES = b"\xCC\x02\x01\x01\xB9\x81" + MIN + b"\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

MES = b"\xCC\x02\x01\x01\xB9\x81\x0D" + MIN + b"\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

print("Testing characters before/after message")
MES = MIN + b"\xCC\x02\x01\x01\xB9\x81\x0D\x0A"
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

MES = b"\xCC\x02\x01\x01\xB9\x81\x0D\x0A" + MIN
print(messaging_library.messaging_readbuffer(MES, R))
print(R)

print("Testing WriteMessage")
# put python encoded message here
MES = "TEST_TEST_TEST,1,2,34545edrdfgCCV_+{}|\\\\\\\'\b\b67879$^&" +\
      "*^#$54rtyfghvnghn65876878nyghvgnh658766$--+P]"
R = messaging_library.messaging_write_message(MES)
print("Python output: ", end="")
print(R)
print("C version: ", end="")
print("{", end="")
for char in list(R):
    if char == 10:
        print(hex(char), end="")
    else:
        print(hex(char), end=",")
print("};", end="\n\n")

print("Testing encode/decode between libraries")
# put c test harness output message here
MES = b"\xCC\x1B\x54\x45\x53\x54\x5F\x54\x45\x53\x54\x5F\x54\x45\x53\x54" +\
    b"\x2C\x31\x2C\x32\x2C\x33\x24\x5E\x26\x2A\x5E\x23\x24\xB9\xAD\x0D\x0A\x00"
print(messaging_library.messaging_readbuffer(MES, R))
