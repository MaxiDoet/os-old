#ifndef _datetime_h
#define _datetime_h

typedef struct datetime_t {
    uint8_t weekday;
    uint8_t day_of_month;
    uint8_t month;
    uint16_t year;
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
} datetime_t;

#endif