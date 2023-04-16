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

TEST(getOctetStringLength, extendedLength) {
    std::vector<unsigned char> v = {0x83, 0x02};
    int position = 0;
    EXPECT_EQ(getExtendedOctetstringLength(&v, position), 0x32);

    v = {0x83, 0x81, 0x02};
    position = 0;
    EXPECT_EQ(getExtendedOctetstringLength(&v, position), 0x312);
}

TEST(getOctetStringLength, normalLength) {
    EXPECT_EQ(getOctetStringLength(0x01), 0x00);
    EXPECT_EQ(getOctetStringLength(0x0F), 0x0E);
}

TEST(getOctetStringAsVector, detectNullptr) {
    std::vector<unsigned char> w;
    std::vector<unsigned char> v;
    EXPECT_EQ(getOctetStringAsVector(NULL, 1, &w, 1), SML_ERROR_NULLPTR);
    EXPECT_EQ(getOctetStringAsVector(&v, 1, NULL, 1), SML_ERROR_NULLPTR);
}

TEST(getOctetStringAsVector, zeroLength) {
    std::vector<unsigned char> w;
    std::vector<unsigned char> v;
    EXPECT_EQ(getOctetStringAsVector(&v, 1, &w, 0), SML_ERROR_ZEROLENGTH);
}

TEST(getOctetStringAsVector, bufferOverflow) {
    std::vector<unsigned char> w;
    std::vector<unsigned char> v = {0x66};
    EXPECT_EQ(getOctetStringAsVector(&v, 1, &w, 10), SML_ERROR_SIZE);
}

TEST(getOctetStringAsVector, getChar) {
    std::vector<unsigned char> w;
    w.resize(5);
    std::vector<unsigned char> v = {0x06, 0x68, 0x61, 0x6c, 0x6c, 0x6f};
    std::vector<unsigned char> result = {0x68, 0x61, 0x6c, 0x6c, 0x6f};
    EXPECT_EQ(getOctetStringAsVector(&v, 1, &w, getOctetStringLength(v.at(0))), SML_OK);
    EXPECT_EQ(result, w);
}

TEST(isUnsigned8, isUnsigned8) {
    unsigned char testChar1 = 0x00;
    unsigned char testChar2 = 0x62;
    unsigned char testChar3 = 0xFF;
    EXPECT_EQ(isUnsigned8(testChar1), false);
    EXPECT_EQ(isUnsigned8(testChar2), true);
    EXPECT_EQ(isUnsigned8(testChar3), false);
}

TEST(getUnsigned8, getUnsigned8) {
    std::vector<unsigned char> v = {0x62, 0};
    int position = 0;
    EXPECT_EQ(getUnsigned8(&v, position), 0);

    std::vector<unsigned char> ww = {0x62};
    position = 1;
    EXPECT_EQ(getUnsigned8(&ww, position), 0xFF);

    position = 0;
    EXPECT_EQ(getUnsigned8(&ww, position), 0xFF);

    std::vector<unsigned char> vv = {0x62, 0x99};
    EXPECT_EQ(getUnsigned8(&vv, position), 0x99);

    std::vector<unsigned char> vvv = {0x63, 10};
    EXPECT_EQ(getUnsigned8(&vvv, position), 0xFF);
}

TEST(getUnsigned16, getUnsigned16) {
    std::vector<unsigned char> v = {0x62, 0};
    int position = 0;
    EXPECT_EQ(getUnsigned16(&v, position), 0xFFFF);

    v = {0x63};
    EXPECT_EQ(getUnsigned16(&v, position), 0xFFFF);

    v = {0x63, 0x10};
    EXPECT_EQ(getUnsigned16(&v, position), 0xFFFF);

    v = {0x63, 0x11, 0x11};
    EXPECT_EQ(getUnsigned16(&v, position), 0x1111);
}

TEST(getUnsigned32, getUnsigned32) {
    std::vector<unsigned char> v = {0x62, 0};
    int position = 0;
    EXPECT_EQ(getUnsigned32(&v, position), 0xFFFFFFFF);

    v = {0x65};
    EXPECT_EQ(getUnsigned32(&v, position), 0xFFFFFFFF);

    v = {0x65, 0x10};
    EXPECT_EQ(getUnsigned32(&v, position), 0xFFFFFFFF);

    v = {0x65, 0x11, 0x11};
    EXPECT_EQ(getUnsigned32(&v, position), 0xFFFFFFFF);

    v = {0x65, 0x11, 0x11, 0x11};
    EXPECT_EQ(getUnsigned32(&v, position), 0xFFFFFFFF);

    v = {0x65, 0x11, 0x11, 0x11, 0x11};
    EXPECT_EQ(getUnsigned32(&v, position), 0x11111111);
}

TEST(getUnsigned64, getUnsigned64) {
    std::vector<unsigned char> v = {0x62, 0};
    int position = 0;
    EXPECT_EQ(getUnsigned64(&v, position), 0xFFFFFFFFFFFFFFFF);

    v = {0x69};
    EXPECT_EQ(getUnsigned64(&v, position), 0xFFFFFFFFFFFFFFFF);

    v = {0x69, 0x10};
    EXPECT_EQ(getUnsigned64(&v, position), 0xFFFFFFFFFFFFFFFF);

    v = {0x69, 0x11, 0x11};
    EXPECT_EQ(getUnsigned64(&v, position), 0xFFFFFFFFFFFFFFFF);

    v = {0x69, 0x11, 0x11, 0x11};
    EXPECT_EQ(getUnsigned64(&v, position), 0xFFFFFFFFFFFFFFFF);

    v = {0x69, 0x11, 0x11, 0x11, 0x11};
    EXPECT_EQ(getUnsigned64(&v, position), 0xFFFFFFFFFFFFFFFF);

    v = {0x69, 0x11, 0x11, 0x11, 0x11, 0x11};
    EXPECT_EQ(getUnsigned64(&v, position), 0xFFFFFFFFFFFFFFFF);

    v = {0x69, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    EXPECT_EQ(getUnsigned64(&v, position), 0xFFFFFFFFFFFFFFFF);

    v = {0x69, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    EXPECT_EQ(getUnsigned64(&v, position), 0xFFFFFFFFFFFFFFFF);

    v = {0x69, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
    EXPECT_EQ(getUnsigned64(&v, position), 0x1111111111111111);
}

TEST(list, getSmlListLength) {
    std::vector<unsigned char> v = {0x86};
    EXPECT_NE(getSmlListLength(&v, 0), 6);
    EXPECT_EQ(getSmlListLength(&v, 0), 0xFF);

    v = {0x76};
    EXPECT_EQ(getSmlListLength(&v, 0), 6);
}

TEST(smltime, getSmlTime) {
    std::vector<unsigned char> v = {0x73, 0x62, 0x01, 0x01, 0x01, 0x01, 0x01};
    int position = 0;
    EXPECT_EQ(getSmlTime(&v, position), 0x00);
    EXPECT_EQ(position, 0x00);

    position = 0;
    std::vector<unsigned char> w = {0x72, 0x62, 0x01, 0x01, 0x01, 0x01, 0x01};
    EXPECT_EQ(getSmlTime(&w, position), 0xFFFFFFFF);
    EXPECT_EQ(position, 0x03);

    position = 0;
    std::vector<unsigned char> x = {0x72, 0x62, 0x01, 0x65, 0x01, 0x01, 0x01, 0x01};
    EXPECT_EQ(getSmlTime(&x, position), 0x01010101);
    EXPECT_EQ(position, 0x08);

    position = 0;
    std::vector<unsigned char> y = {0x72, 0x62, 0x02, 0x65, 0x01, 0x01, 0x01, 0x01};
    EXPECT_EQ(getSmlTime(&y, position), 0x01010101);
    EXPECT_EQ(position, 0x08);
}

TEST(smlStatus, getSmlStatus) {
    std::vector<unsigned char> v = {0x61, 0x12, 0x34};
    int position = 0;
    EXPECT_EQ(getSmlStatus(&v, position), 0xFFFFFFFFFFFFFFFF);

    v = {0x62, 0x56};
    position = 0;
    EXPECT_EQ(getSmlStatus(&v, position), 0x56);

    v = {0x63, 0x12, 0x34};
    position = 0;
    EXPECT_EQ(getSmlStatus(&v, position), 0x1234);

    v = {0x65, 0x12, 0x34, 0x56, 0x78};
    position = 0;
    EXPECT_EQ(getSmlStatus(&v, position), 0x12345678);

    v = {0x69, 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78};
    position = 0;
    EXPECT_EQ(getSmlStatus(&v, position), 0x1234567812345678);
}