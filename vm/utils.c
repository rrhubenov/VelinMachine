#include "utils.h"

void check_err(int status_code, const char* message) {
    if(status_code == -1) {
        err(1, "%s", message);
    }
}

void set_bit(uint32_t* target, uint8_t bit) {
    uint32_t mask = 1;
    *target = (*target) | (mask << bit);
}

void unset_bit(uint32_t* target, uint8_t bit) {
    uint32_t mask = 1;
    *target = (*target) & ~(mask << bit);
}

uint32_t get_bit(uint32_t target, uint8_t bit) {
    uint32_t mask = 1;
    return target & (mask << bit);
}


