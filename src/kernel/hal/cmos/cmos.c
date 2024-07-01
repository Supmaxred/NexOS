#include <stdint.h>
#include "cpu.h"
#include <time.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define CURRENT_YEAR 2024

int century_register = 0x00;

static uint8_t second;
static uint8_t minute;
static uint8_t hour;
static uint8_t day;
static uint8_t month;
static uint32_t year;

uint8_t rtc_reg(int reg)
{
      outb(CMOS_ADDRESS, reg);
      return inb(CMOS_DATA);
}
 
time_t rtc_update()
{
      uint8_t century;
      uint8_t last_second;
      uint8_t last_minute;
      uint8_t last_hour;
      uint8_t last_day;
      uint8_t last_month;
      uint8_t last_year;
      uint8_t last_century;
      uint8_t registerB;

      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates

      while(rtc_reg(0x0A) & 0x80);

      second = rtc_reg(0x00);
      minute = rtc_reg(0x02);
      hour = rtc_reg(0x04);
      day = rtc_reg(0x07);
      month = rtc_reg(0x08);
      year = rtc_reg(0x09);

      if(century_register != 0)
            century = rtc_reg(century_register);

      do {
            last_second = second;
            last_minute = minute;
            last_hour = hour;
            last_day = day;
            last_month = month;
            last_year = year;
            last_century = century;

            while (rtc_reg(0x0A) & 0x80);

            second = rtc_reg(0x00);
            minute = rtc_reg(0x02);
            hour = rtc_reg(0x04);
            day = rtc_reg(0x07);
            month = rtc_reg(0x08);
            year = rtc_reg(0x09);

            if(century_register != 0)
                  century = rtc_reg(century_register);
    } while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
      (last_day != day) || (last_month != month) || (last_year != year) ||
      (last_century != century) );

    registerB = rtc_reg(0x0B);

    // Convert BCD to binary values if necessary

    if (!(registerB & 0x04))
    {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);

            if(century_register != 0)
                  century = (century & 0x0F) + ((century / 16) * 10);
      }

      // Convert 12 hour clock to 24 hour clock if necessary

      if (!(registerB & 0x02) && (hour & 0x80))
            hour = ((hour & 0x7F) + 12) % 24;

      // Calculate the full (4-digit) year

      if(century_register != 0)
      {
            year += century * 100;
      }
      else
      {
          year += (CURRENT_YEAR / 100) * 100;
          if(year < CURRENT_YEAR) year += 100;
      }

      struct tm temp;
      temp.tm_sec = second;
      temp.tm_min = minute;
      temp.tm_hour = hour;
      temp.tm_mday = day;
      temp.tm_mon = month - 1;
      temp.tm_year = year - 1900;

      return timegm(&temp);
}