#include <gtest/gtest.h>
#include "SML_Parser.hpp"

TEST(isOctetString, noString) {
    EXPECT_FALSE(isOctetString(0x10));
}

TEST(isOctetString, isString) {
    EXPECT_TRUE(isOctetString(0x0C));
}

TEST(getOctetStringLength, wrongLength) {
    EXPECT_EQ(getOctetStringLength(0x11), -1);
    EXPECT_EQ(getOctetStringLength(0x10), -1);
    EXPECT_EQ(getOctetStringLength(0xFF), -1);
}

TEST(getOctetStringLength, zeroLength) {
    EXPECT_EQ(getOctetStringLength(0x00), 0);
}

TEST(getOctetStringLength, normalLength) {
    EXPECT_EQ(getOctetStringLength(0x01), 0x00);
    EXPECT_EQ(getOctetStringLength(0x0F), 0x0E);
}

TEST(getOctetStringAsVector, detectNullptr) {
    std::vector<char> w;
    std::vector<char> v;
    EXPECT_EQ(getOctetStringAsVector(NULL, 1, &w, 1), SML_ERROR_NULLPTR);
    EXPECT_EQ(getOctetStringAsVector(&v, 1, NULL, 1), SML_ERROR_NULLPTR);
}

TEST(getOctetStringAsVector, zeroLength) {
    std::vector<char> w;
    std::vector<char> v;
    EXPECT_EQ(getOctetStringAsVector(&v, 1, &w, 0), SML_ERROR_ZEROLENGTH);
}

TEST(getOctetStringAsVector, bufferOverflow) {
    std::vector<char> w;
    std::vector<char> v = {0x66};
    EXPECT_EQ(getOctetStringAsVector(&v, 1, &w, 10), SML_ERROR_SIZE);
}

TEST(getOctetStringAsVector, getChar) {
    std::vector<char> w;
    w.resize(5);
    std::vector<char> v = {0x06, 0x68, 0x61, 0x6c, 0x6c, 0x6f};
    std::vector<char> result = {0x68, 0x61, 0x6c, 0x6c, 0x6f};
    EXPECT_EQ(getOctetStringAsVector(&v, 1, &w, getOctetStringLength(v.at(0))), SML_OK);
    EXPECT_EQ(result, w);
}

TEST(isUnsigned8, isUnsigned8) {
    char testChar1 = 0x00;
    char testChar2 = 0x52;
    char testChar3 = 0xFF;
    EXPECT_EQ(isUnsigned8(testChar1), false);
    EXPECT_EQ(isUnsigned8(testChar2), true);
    EXPECT_EQ(isUnsigned8(testChar3), false);
}

TEST(getUnsigned8, getUnsigned8) {
    std::vector<char> v = {0x52, 0};
    EXPECT_EQ(getUnsigned8(&v, 0), 0);

    v = {0x52, 0x99};
    EXPECT_EQ(getUnsigned8(&v, 0), 0x99);

    v = {0x53, 10};
    EXPECT_EQ(getUnsigned8(&v, 0), 10);
}