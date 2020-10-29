#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Library-level functions.
 * You should use them in the main sections.
 */

uint64_t convertToUint64 (double number) {
    return *((uint64_t *)(&number));
}

bool getBit (const uint64_t number, const uint8_t index) {
    uint64_t mask = 1ull << index;
    return (number & mask) == mask;
}

/**
 * Checkers here:
 */

const uint64_t exponentMask = 0x7ff0000000000000;
const uint64_t fractionMask = 0x000fffffffffffff;

bool checkForFullExponent(uint64_t number) {
    return (number & exponentMask) == exponentMask;
}

bool checkForEmptyExponent(uint64_t number) {
    return (number & exponentMask) == 0;
}

bool checkForFullFraction(uint64_t number) {
    return (number & fractionMask) == fractionMask;
}

bool checkForEmptyFraction(uint64_t number) {
    return (number & fractionMask) == 0;
}

bool checkForNormal(uint64_t number) {
    return !checkForEmptyExponent(number) && !checkForFullExponent(number);
}

bool checkForDenormal(uint64_t number) {
    return checkForEmptyExponent(number) && getBit(number, 0);
}

bool checkForPlus (uint64_t number) {
    return !getBit(number, 63);
}

bool checkForPlusZero (uint64_t number) {
    return number == 0x0000000000000000;
}

bool checkForMinusZero (uint64_t number) {
    return number == 0x8000000000000000;
}

bool checkForPlusInf (uint64_t number) {
    return checkForPlus(number) && checkForFullExponent(number) && checkForEmptyFraction(number);
}

bool checkForMinusInf (uint64_t number) {
    return !checkForPlus(number) && checkForFullExponent(number) && checkForEmptyFraction(number);
}

bool checkForPlusNormal (uint64_t number) {
    return checkForPlus(number) && checkForNormal(number);
}

bool checkForMinusNormal (uint64_t number) {
    return !checkForPlus(number) && checkForNormal(number);
}

bool checkForPlusDenormal (uint64_t number) {
    return checkForPlus(number) && checkForDenormal(number);
}

bool checkForMinusDenormal (uint64_t number) {
    return !checkForPlus(number) && checkForDenormal(number);
}

bool checkForSignalingNan (uint64_t number) {
    return checkForFullExponent(number) && !getBit(number, 51) && !checkForEmptyFraction(number);
}

bool checkForQuietNan (uint64_t number) {
    return checkForFullExponent(number) && getBit(number, 51);
}


void classify (double number) {
    if (checkForPlusZero(convertToUint64(number))) {
        printf("Plus zero\n");
    }

    else if (checkForMinusZero(convertToUint64(number))) {
        printf("Minus zero\n");
    }

    else if (checkForPlusInf(convertToUint64(number))) {
        printf("Plus inf\n");
    }

    else if (checkForMinusInf(convertToUint64(number))) {
        printf("Minus inf\n");
    }

    else if (checkForPlusNormal(convertToUint64(number))) {
        printf("Plus regular\n");
    }

    else if (checkForMinusNormal(convertToUint64(number))) {
        printf("Minus regular\n");
    }

    else if (checkForPlusDenormal(convertToUint64(number))) {
        printf("Plus Denormal\n");
    }

    else if (checkForMinusDenormal(convertToUint64(number))) {
        printf("Minus Denormal\n");
    }

    else if (checkForSignalingNan(convertToUint64(number))) {
        printf("Signailing NaN\n");
    }

    else if (checkForQuietNan(convertToUint64(number))) {
        printf("Quiet NaN\n");
    }

    else {
        printf("Error.\n");
    }
}