#include <gtest/gtest.h>
#include "SmlParser.hpp"

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
    char testChar2 = 0x62;
    char testChar3 = 0xFF;
    EXPECT_EQ(isUnsigned8(testChar1), false);
    EXPECT_EQ(isUnsigned8(testChar2), true);
    EXPECT_EQ(isUnsigned8(testChar3), false);
}

TEST(getUnsigned8, getUnsigned8) {
    std::vector<char> v = {0x62, 0};
    EXPECT_EQ(getUnsigned8(&v, 0), 0);

    v = {0x62};
    EXPECT_EQ(getUnsigned8(&v, 0), 0xFF);

    v = {0x62, 0x99};
    EXPECT_EQ(getUnsigned8(&v, 0), 0x99);

    v = {0x63, 10};
    EXPECT_EQ(getUnsigned8(&v, 0), 0xFF);
}

TEST(getUnsigned16, getUnsigned16) {
    std::vector<char> v = {0x62, 0};
    EXPECT_EQ(getUnsigned16(&v, 0), 0xFFFF);

    v = {0x63};
    EXPECT_EQ(getUnsigned16(&v, 0), 0xFFFF);

    v = {0x63, 0x10};
    EXPECT_EQ(getUnsigned16(&v, 0), 0xFFFF);

    v = {0x63, 0x11, 0x11};
    EXPECT_EQ(getUnsigned16(&v, 0), 0x1111);
}

TEST(getUnsigned32, getUnsigned32) {
    std::vector<char> v = {0x62, 0};
    EXPECT_EQ(getUnsigned32(&v, 0), 0xFFFFFFFF);

    v = {0x65};
    EXPECT_EQ(getUnsigned32(&v, 0), 0xFFFFFFFF);

    v = {0x65, 0x10};
    EXPECT_EQ(getUnsigned32(&v, 0), 0xFFFFFFFF);

    v = {0x65, 0x11, 0x11};
    EXPECT_EQ(getUnsigned32(&v, 0), 0xFFFFFFFF);

    v = {0x65, 0x11, 0x11, 0x11};
    EXPECT_EQ(getUnsigned32(&v, 0), 0xFFFFFFFF);

    v = {0x65, 0x11, 0x11, 0x11, 0x11};
    EXPECT_EQ(getUnsigned32(&v, 0), 0x11111111);
}

TEST(list, getSmlListLength) {
    std::vector<char> v = {0x86};
    EXPECT_NE(getSmlListLength(&v, 0), 6);
    EXPECT_EQ(getSmlListLength(&v, 0), 0xFF);

    v = {0x76};
    EXPECT_EQ(getSmlListLength(&v, 0), 6);
}

TEST(smltime, getSmlTime) {
    std::vector<char> v = {0x73, 0x62, 0x01, 0x01, 0x01, 0x01, 0x01};
    EXPECT_EQ(getSmlTime(&v, 0), 0x00);

    std::vector<char> w = {0x72, 0x62, 0x01, 0x01, 0x01, 0x01, 0x01};
    EXPECT_EQ(getSmlTime(&w, 0), 0xFFFFFFFF);

    std::vector<char> x = {0x72, 0x62, 0x01, 0x65, 0x01, 0x01, 0x01, 0x01};
    EXPECT_EQ(getSmlTime(&x, 0), 0x01010101);

    std::vector<char> y = {0x72, 0x62, 0x02, 0x65, 0x01, 0x01, 0x01, 0x01};
    EXPECT_EQ(getSmlTime(&y, 0), 0x01010101);
}