#include "parser.h"

int parse_register(char* text) {
   //provera da li registar pocinje slovom x
    if (text[0] != 'x') {
        return -1;
    }

    int i = 1;
    int value = 0;

    //provera da li je cifra posle x
    if (text[i] < '0' || text[i] > '9') {
        return -1;
    }

    while (text[i] >= '0' && text[i] <= '9') {
        value = value * 10 + (text[i] - '0');
        i++;
    }

    if (text[i] != 0) {
        return -1;
    }

    if (value < 0 || value > 31) {
        return -1;
    }

    return value;
}

int parse_int(char* text) {
    int i = 0;
    int sign = 1;
    int value = 0;

    if (text[0] == '-') {
        sign = -1;
        i++;
    }

    if (text[i] < '0' || text[i] > '9') {
        return 0;
    }

    while (text[i] >= '0' && text[i] <= '9') {
        value = value * 10 + (text[i] - '0');
        i++;
    }

    if (text[i] != 0) {
        return 0;
    }

    return value * sign;
}
