# Author: Iain Macpherson

# File containing data to send to the controllers will contain the data in the following format.

# Format of 8-byte message:
# First byte is the Function Code (eg: writeOneByte)
# Function codes:
# readObject:       0x40
# writeOneByte:     0x2F
# writeTwoBytes:    0x2B
# writeThreeBytes:  0x27 (unused)
# writeFourBytes:   0x23
#
# The next 3 bytes are the index and sub-index of the register
# (eg register index: 0x1234 sub-index: 0x01 would be 0x34 0x12 0x01)
# The final 4 bytes are the data to write.

# message example: Writing one byte of data, 0x02, to the the index 0x1234 sub-index 0x00
# would be written as follows:
# 0x2F 0x34 0x12 0x00 0x02 0x00 0x00 0x00