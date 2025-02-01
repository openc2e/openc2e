#include "common/NumericCast.h"

#include <gtest/gtest.h>
#include <stdint.h>

/*
#!/usr/bin/env python

import math
import struct

types = [
    ("uint8_t", 0, 255),
    ("int8_t", -128, 127),
    ("uint16_t", 0, 65535),
    ("int16_t", -32768, 32767),
    ("uint32_t", 0, 4294967295),
    ("int32_t", -2147483648, 2147483647),
    ("uint64_t", 0, 18446744073709551615),
    ("int64_t", -9223372036854775808, 9223372036854775807)
]

from_only_types = [
	("float", -340282346638528859811704183484516925440, 340282346638528859811704183484516925440),
]

def literal(type, value):
    if type != "float":
        value = int(value)
    if type == "uint64_t":
        return f"{value}ull"
    if type == "int64_t":
        if value == -9223372036854775808:
            return f"-9223372036854775807ll - 1ll"
        return f"{value}ll"
    if type == "float":
        return f"{value:f}f"
    return f"{value}"

def nextafter(x, direction):
    x = struct.unpack('f', struct.pack('f', x))[0]
    epsilon  = math.ldexp(1.0, -24)
    m, e = math.frexp(x)
    if direction > 0:
        m += epsilon
    else:
        m -= epsilon
    return math.ldexp(m, e)

def lower(type, value):
    if type == 'float':
        while True:
            packed = struct.unpack('f', struct.pack('f', value))[0]
            if packed <= value:
                return packed
            value = nextafter(value, float("-inf"))
    return value

def higher(type, value):
    if type == 'float':
        while True:
            packed = struct.unpack('f', struct.pack('f', value))[0]
            if packed >= value:
                return packed
            value = nextafter(value)
    return value

for to, to_low, to_high in types:
    for from_, from_low, from_high in types + from_only_types:
        print("TEST(NumericCast, " + from_.replace('_t', '') + "_to_" + to.replace('_t', '') + ") {")
        print(f"\t{from_} value;")

        if from_low < to_low:
            print("\tvalue = {};".format(literal(from_, lower(from_, to_low - 1))))
            print(f"\tEXPECT_THROW(numeric_cast<{to}>(value), std::overflow_error);")
            print("\tvalue = {};".format(literal(from_, higher(from_, to_low))))
            print("\tEXPECT_EQ(numeric_cast<{}>(value), {});".format(to, literal(to, higher(from_, to_low))))
        else:
            print("\tvalue = {};".format(literal(from_, from_low)))
            print("\tEXPECT_EQ(numeric_cast<{}>(value), {});".format(to, literal(to, from_low)))
        if from_high > to_high:
            print("\tvalue = {};".format(literal(from_, higher(from_, to_high + 1))))
            print(f"\tEXPECT_THROW(numeric_cast<{to}>(value), std::overflow_error);")
            print("\tvalue = {};".format(literal(from_, lower(from_, to_high))))
            print("\tEXPECT_EQ(numeric_cast<{}>(value), {});".format(to, literal(to, lower(from_, to_high))))
        else:
            print("\tvalue = {};".format(literal(from_, from_high)))
            print("\tEXPECT_EQ(numeric_cast<{}>(value), {});".format(to, literal(to, from_high)))
        print("}")
        print()
*/

TEST(NumericCast, uint8_to_uint8) {
	uint8_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 0);
	value = 255;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 255);
}

TEST(NumericCast, int8_to_uint8) {
	int8_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 0);
	value = 127;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 127);
}

TEST(NumericCast, uint16_to_uint8) {
	uint16_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 0);
	value = 256;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 255;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 255);
}

TEST(NumericCast, int16_to_uint8) {
	int16_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 0);
	value = 256;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 255;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 255);
}

TEST(NumericCast, uint32_to_uint8) {
	uint32_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 0);
	value = 256;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 255;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 255);
}

TEST(NumericCast, int32_to_uint8) {
	int32_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 0);
	value = 256;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 255;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 255);
}

TEST(NumericCast, uint64_to_uint8) {
	uint64_t value;
	value = 0ull;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 0);
	value = 256ull;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 255ull;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 255);
}

TEST(NumericCast, int64_to_uint8) {
	int64_t value;
	value = -1ll;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 0ll;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 0);
	value = 256ll;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 255ll;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 255);
}

TEST(NumericCast, float_to_uint8) {
	float value;
	value = -1.000000f;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 0.000000f;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 0);
	value = 256.000000f;
	EXPECT_THROW(numeric_cast<uint8_t>(value), std::overflow_error);
	value = 255.000000f;
	EXPECT_EQ(numeric_cast<uint8_t>(value), 255);
}

TEST(NumericCast, uint8_to_int8) {
	uint8_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int8_t>(value), 0);
	value = 128;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = 127;
	EXPECT_EQ(numeric_cast<int8_t>(value), 127);
}

TEST(NumericCast, int8_to_int8) {
	int8_t value;
	value = -128;
	EXPECT_EQ(numeric_cast<int8_t>(value), -128);
	value = 127;
	EXPECT_EQ(numeric_cast<int8_t>(value), 127);
}

TEST(NumericCast, uint16_to_int8) {
	uint16_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int8_t>(value), 0);
	value = 128;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = 127;
	EXPECT_EQ(numeric_cast<int8_t>(value), 127);
}

TEST(NumericCast, int16_to_int8) {
	int16_t value;
	value = -129;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = -128;
	EXPECT_EQ(numeric_cast<int8_t>(value), -128);
	value = 128;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = 127;
	EXPECT_EQ(numeric_cast<int8_t>(value), 127);
}

TEST(NumericCast, uint32_to_int8) {
	uint32_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int8_t>(value), 0);
	value = 128;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = 127;
	EXPECT_EQ(numeric_cast<int8_t>(value), 127);
}

TEST(NumericCast, int32_to_int8) {
	int32_t value;
	value = -129;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = -128;
	EXPECT_EQ(numeric_cast<int8_t>(value), -128);
	value = 128;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = 127;
	EXPECT_EQ(numeric_cast<int8_t>(value), 127);
}

TEST(NumericCast, uint64_to_int8) {
	uint64_t value;
	value = 0ull;
	EXPECT_EQ(numeric_cast<int8_t>(value), 0);
	value = 128ull;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = 127ull;
	EXPECT_EQ(numeric_cast<int8_t>(value), 127);
}

TEST(NumericCast, int64_to_int8) {
	int64_t value;
	value = -129ll;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = -128ll;
	EXPECT_EQ(numeric_cast<int8_t>(value), -128);
	value = 128ll;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = 127ll;
	EXPECT_EQ(numeric_cast<int8_t>(value), 127);
}

TEST(NumericCast, float_to_int8) {
	float value;
	value = -129.000000f;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = -128.000000f;
	EXPECT_EQ(numeric_cast<int8_t>(value), -128);
	value = 128.000000f;
	EXPECT_THROW(numeric_cast<int8_t>(value), std::overflow_error);
	value = 127.000000f;
	EXPECT_EQ(numeric_cast<int8_t>(value), 127);
}

TEST(NumericCast, uint8_to_uint16) {
	uint8_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 0);
	value = 255;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 255);
}

TEST(NumericCast, int8_to_uint16) {
	int8_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint16_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 0);
	value = 127;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 127);
}

TEST(NumericCast, uint16_to_uint16) {
	uint16_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 0);
	value = 65535;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 65535);
}

TEST(NumericCast, int16_to_uint16) {
	int16_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint16_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 0);
	value = 32767;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 32767);
}

TEST(NumericCast, uint32_to_uint16) {
	uint32_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 0);
	value = 65536;
	EXPECT_THROW(numeric_cast<uint16_t>(value), std::overflow_error);
	value = 65535;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 65535);
}

TEST(NumericCast, int32_to_uint16) {
	int32_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint16_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 0);
	value = 65536;
	EXPECT_THROW(numeric_cast<uint16_t>(value), std::overflow_error);
	value = 65535;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 65535);
}

TEST(NumericCast, uint64_to_uint16) {
	uint64_t value;
	value = 0ull;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 0);
	value = 65536ull;
	EXPECT_THROW(numeric_cast<uint16_t>(value), std::overflow_error);
	value = 65535ull;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 65535);
}

TEST(NumericCast, int64_to_uint16) {
	int64_t value;
	value = -1ll;
	EXPECT_THROW(numeric_cast<uint16_t>(value), std::overflow_error);
	value = 0ll;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 0);
	value = 65536ll;
	EXPECT_THROW(numeric_cast<uint16_t>(value), std::overflow_error);
	value = 65535ll;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 65535);
}

TEST(NumericCast, float_to_uint16) {
	float value;
	value = -1.000000f;
	EXPECT_THROW(numeric_cast<uint16_t>(value), std::overflow_error);
	value = 0.000000f;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 0);
	value = 65536.000000f;
	EXPECT_THROW(numeric_cast<uint16_t>(value), std::overflow_error);
	value = 65535.000000f;
	EXPECT_EQ(numeric_cast<uint16_t>(value), 65535);
}

TEST(NumericCast, uint8_to_int16) {
	uint8_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int16_t>(value), 0);
	value = 255;
	EXPECT_EQ(numeric_cast<int16_t>(value), 255);
}

TEST(NumericCast, int8_to_int16) {
	int8_t value;
	value = -128;
	EXPECT_EQ(numeric_cast<int16_t>(value), -128);
	value = 127;
	EXPECT_EQ(numeric_cast<int16_t>(value), 127);
}

TEST(NumericCast, uint16_to_int16) {
	uint16_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int16_t>(value), 0);
	value = 32768;
	EXPECT_THROW(numeric_cast<int16_t>(value), std::overflow_error);
	value = 32767;
	EXPECT_EQ(numeric_cast<int16_t>(value), 32767);
}

TEST(NumericCast, int16_to_int16) {
	int16_t value;
	value = -32768;
	EXPECT_EQ(numeric_cast<int16_t>(value), -32768);
	value = 32767;
	EXPECT_EQ(numeric_cast<int16_t>(value), 32767);
}

TEST(NumericCast, uint32_to_int16) {
	uint32_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int16_t>(value), 0);
	value = 32768;
	EXPECT_THROW(numeric_cast<int16_t>(value), std::overflow_error);
	value = 32767;
	EXPECT_EQ(numeric_cast<int16_t>(value), 32767);
}

TEST(NumericCast, int32_to_int16) {
	int32_t value;
	value = -32769;
	EXPECT_THROW(numeric_cast<int16_t>(value), std::overflow_error);
	value = -32768;
	EXPECT_EQ(numeric_cast<int16_t>(value), -32768);
	value = 32768;
	EXPECT_THROW(numeric_cast<int16_t>(value), std::overflow_error);
	value = 32767;
	EXPECT_EQ(numeric_cast<int16_t>(value), 32767);
}

TEST(NumericCast, uint64_to_int16) {
	uint64_t value;
	value = 0ull;
	EXPECT_EQ(numeric_cast<int16_t>(value), 0);
	value = 32768ull;
	EXPECT_THROW(numeric_cast<int16_t>(value), std::overflow_error);
	value = 32767ull;
	EXPECT_EQ(numeric_cast<int16_t>(value), 32767);
}

TEST(NumericCast, int64_to_int16) {
	int64_t value;
	value = -32769ll;
	EXPECT_THROW(numeric_cast<int16_t>(value), std::overflow_error);
	value = -32768ll;
	EXPECT_EQ(numeric_cast<int16_t>(value), -32768);
	value = 32768ll;
	EXPECT_THROW(numeric_cast<int16_t>(value), std::overflow_error);
	value = 32767ll;
	EXPECT_EQ(numeric_cast<int16_t>(value), 32767);
}

TEST(NumericCast, float_to_int16) {
	float value;
	value = -32769.000000f;
	EXPECT_THROW(numeric_cast<int16_t>(value), std::overflow_error);
	value = -32768.000000f;
	EXPECT_EQ(numeric_cast<int16_t>(value), -32768);
	value = 32768.000000f;
	EXPECT_THROW(numeric_cast<int16_t>(value), std::overflow_error);
	value = 32767.000000f;
	EXPECT_EQ(numeric_cast<int16_t>(value), 32767);
}

TEST(NumericCast, uint8_to_uint32) {
	uint8_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 0);
	value = 255;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 255);
}

TEST(NumericCast, int8_to_uint32) {
	int8_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint32_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 0);
	value = 127;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 127);
}

TEST(NumericCast, uint16_to_uint32) {
	uint16_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 0);
	value = 65535;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 65535);
}

TEST(NumericCast, int16_to_uint32) {
	int16_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint32_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 0);
	value = 32767;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 32767);
}

TEST(NumericCast, uint32_to_uint32) {
	uint32_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 0);
	value = 4294967295;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 4294967295);
}

TEST(NumericCast, int32_to_uint32) {
	int32_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint32_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 0);
	value = 2147483647;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 2147483647);
}

TEST(NumericCast, uint64_to_uint32) {
	uint64_t value;
	value = 0ull;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 0);
	value = 4294967296ull;
	EXPECT_THROW(numeric_cast<uint32_t>(value), std::overflow_error);
	value = 4294967295ull;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 4294967295);
}

TEST(NumericCast, int64_to_uint32) {
	int64_t value;
	value = -1ll;
	EXPECT_THROW(numeric_cast<uint32_t>(value), std::overflow_error);
	value = 0ll;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 0);
	value = 4294967296ll;
	EXPECT_THROW(numeric_cast<uint32_t>(value), std::overflow_error);
	value = 4294967295ll;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 4294967295);
}

TEST(NumericCast, float_to_uint32) {
	float value;
	value = -1.000000f;
	EXPECT_THROW(numeric_cast<uint32_t>(value), std::overflow_error);
	value = 0.000000f;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 0);
	value = 4294967296.000000f;
	EXPECT_THROW(numeric_cast<uint32_t>(value), std::overflow_error);
	value = 4294966784.000000f;
	EXPECT_EQ(numeric_cast<uint32_t>(value), 4294966784);
}

TEST(NumericCast, uint8_to_int32) {
	uint8_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int32_t>(value), 0);
	value = 255;
	EXPECT_EQ(numeric_cast<int32_t>(value), 255);
}

TEST(NumericCast, int8_to_int32) {
	int8_t value;
	value = -128;
	EXPECT_EQ(numeric_cast<int32_t>(value), -128);
	value = 127;
	EXPECT_EQ(numeric_cast<int32_t>(value), 127);
}

TEST(NumericCast, uint16_to_int32) {
	uint16_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int32_t>(value), 0);
	value = 65535;
	EXPECT_EQ(numeric_cast<int32_t>(value), 65535);
}

TEST(NumericCast, int16_to_int32) {
	int16_t value;
	value = -32768;
	EXPECT_EQ(numeric_cast<int32_t>(value), -32768);
	value = 32767;
	EXPECT_EQ(numeric_cast<int32_t>(value), 32767);
}

TEST(NumericCast, uint32_to_int32) {
	uint32_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int32_t>(value), 0);
	value = 2147483648;
	EXPECT_THROW(numeric_cast<int32_t>(value), std::overflow_error);
	value = 2147483647;
	EXPECT_EQ(numeric_cast<int32_t>(value), 2147483647);
}

TEST(NumericCast, int32_to_int32) {
	int32_t value;
	value = -2147483648;
	EXPECT_EQ(numeric_cast<int32_t>(value), -2147483648);
	value = 2147483647;
	EXPECT_EQ(numeric_cast<int32_t>(value), 2147483647);
}

TEST(NumericCast, uint64_to_int32) {
	uint64_t value;
	value = 0ull;
	EXPECT_EQ(numeric_cast<int32_t>(value), 0);
	value = 2147483648ull;
	EXPECT_THROW(numeric_cast<int32_t>(value), std::overflow_error);
	value = 2147483647ull;
	EXPECT_EQ(numeric_cast<int32_t>(value), 2147483647);
}

TEST(NumericCast, int64_to_int32) {
	int64_t value;
	value = -2147483649ll;
	EXPECT_THROW(numeric_cast<int32_t>(value), std::overflow_error);
	value = -2147483648ll;
	EXPECT_EQ(numeric_cast<int32_t>(value), -2147483648);
	value = 2147483648ll;
	EXPECT_THROW(numeric_cast<int32_t>(value), std::overflow_error);
	value = 2147483647ll;
	EXPECT_EQ(numeric_cast<int32_t>(value), 2147483647);
}

TEST(NumericCast, float_to_int32) {
	float value;
	value = -2147483904.000000f;
	EXPECT_THROW(numeric_cast<int32_t>(value), std::overflow_error);
	value = -2147483648.000000f;
	EXPECT_EQ(numeric_cast<int32_t>(value), -2147483648);
	value = 2147483648.000000f;
	EXPECT_THROW(numeric_cast<int32_t>(value), std::overflow_error);
	value = 2147483392.000000f;
	EXPECT_EQ(numeric_cast<int32_t>(value), 2147483392);
}

TEST(NumericCast, uint8_to_uint64) {
	uint8_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 0ull);
	value = 255;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 255ull);
}

TEST(NumericCast, int8_to_uint64) {
	int8_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint64_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 0ull);
	value = 127;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 127ull);
}

TEST(NumericCast, uint16_to_uint64) {
	uint16_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 0ull);
	value = 65535;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 65535ull);
}

TEST(NumericCast, int16_to_uint64) {
	int16_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint64_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 0ull);
	value = 32767;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 32767ull);
}

TEST(NumericCast, uint32_to_uint64) {
	uint32_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 0ull);
	value = 4294967295;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 4294967295ull);
}

TEST(NumericCast, int32_to_uint64) {
	int32_t value;
	value = -1;
	EXPECT_THROW(numeric_cast<uint64_t>(value), std::overflow_error);
	value = 0;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 0ull);
	value = 2147483647;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 2147483647ull);
}

TEST(NumericCast, uint64_to_uint64) {
	uint64_t value;
	value = 0ull;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 0ull);
	value = 18446744073709551615ull;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 18446744073709551615ull);
}

TEST(NumericCast, int64_to_uint64) {
	int64_t value;
	value = -1ll;
	EXPECT_THROW(numeric_cast<uint64_t>(value), std::overflow_error);
	value = 0ll;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 0ull);
	value = 9223372036854775807ll;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 9223372036854775807ull);
}

TEST(NumericCast, float_to_uint64) {
	float value;
	value = -1.000000f;
	EXPECT_THROW(numeric_cast<uint64_t>(value), std::overflow_error);
	value = 0.000000f;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 0ull);
	value = 18446744073709551616.000000f;
	EXPECT_THROW(numeric_cast<uint64_t>(value), std::overflow_error);
	value = 18446741874686296064.000000f;
	EXPECT_EQ(numeric_cast<uint64_t>(value), 18446741874686296064ull);
}

TEST(NumericCast, uint8_to_int64) {
	uint8_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int64_t>(value), 0ll);
	value = 255;
	EXPECT_EQ(numeric_cast<int64_t>(value), 255ll);
}

TEST(NumericCast, int8_to_int64) {
	int8_t value;
	value = -128;
	EXPECT_EQ(numeric_cast<int64_t>(value), -128ll);
	value = 127;
	EXPECT_EQ(numeric_cast<int64_t>(value), 127ll);
}

TEST(NumericCast, uint16_to_int64) {
	uint16_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int64_t>(value), 0ll);
	value = 65535;
	EXPECT_EQ(numeric_cast<int64_t>(value), 65535ll);
}

TEST(NumericCast, int16_to_int64) {
	int16_t value;
	value = -32768;
	EXPECT_EQ(numeric_cast<int64_t>(value), -32768ll);
	value = 32767;
	EXPECT_EQ(numeric_cast<int64_t>(value), 32767ll);
}

TEST(NumericCast, uint32_to_int64) {
	uint32_t value;
	value = 0;
	EXPECT_EQ(numeric_cast<int64_t>(value), 0ll);
	value = 4294967295;
	EXPECT_EQ(numeric_cast<int64_t>(value), 4294967295ll);
}

TEST(NumericCast, int32_to_int64) {
	int32_t value;
	value = -2147483648;
	EXPECT_EQ(numeric_cast<int64_t>(value), -2147483648ll);
	value = 2147483647;
	EXPECT_EQ(numeric_cast<int64_t>(value), 2147483647ll);
}

TEST(NumericCast, uint64_to_int64) {
	uint64_t value;
	value = 0ull;
	EXPECT_EQ(numeric_cast<int64_t>(value), 0ll);
	value = 9223372036854775808ull;
	EXPECT_THROW(numeric_cast<int64_t>(value), std::overflow_error);
	value = 9223372036854775807ull;
	EXPECT_EQ(numeric_cast<int64_t>(value), 9223372036854775807ll);
}

TEST(NumericCast, int64_to_int64) {
	int64_t value;
	value = -9223372036854775807ll - 1ll;
	EXPECT_EQ(numeric_cast<int64_t>(value), -9223372036854775807ll - 1ll);
	value = 9223372036854775807ll;
	EXPECT_EQ(numeric_cast<int64_t>(value), 9223372036854775807ll);
}

TEST(NumericCast, float_to_int64) {
	float value;
	value = -9223373136366403584.000000f;
	EXPECT_THROW(numeric_cast<int64_t>(value), std::overflow_error);
	value = -9223372036854775808.000000f;
	EXPECT_EQ(numeric_cast<int64_t>(value), -9223372036854775807ll - 1ll);
	value = 9223372036854775808.000000f;
	EXPECT_THROW(numeric_cast<int64_t>(value), std::overflow_error);
	value = 9223370937343148032.000000f;
	EXPECT_EQ(numeric_cast<int64_t>(value), 9223370937343148032ll);
}

TEST(NumericCast, int32_to_float) {
	int32_t value;
	value = -16777217;
	EXPECT_THROW(numeric_cast<float>(value), std::overflow_error);
	value = -16777216;
	EXPECT_EQ(static_cast<int32_t>(numeric_cast<float>(value)), -16777216);
	value = 16777216;
	EXPECT_EQ(static_cast<int32_t>(numeric_cast<float>(value)), 16777216);
	value = 16777217;
	EXPECT_THROW(numeric_cast<float>(value), std::overflow_error);
}

TEST(NumericCast, uint32_to_float) {
	uint32_t value;
	value = 0;
	EXPECT_EQ(static_cast<uint32_t>(numeric_cast<float>(value)), 0);
	value = 16777216;
	EXPECT_EQ(static_cast<uint32_t>(numeric_cast<float>(value)), 16777216);
	value = 16777217;
	EXPECT_THROW(numeric_cast<float>(value), std::overflow_error);
}