#pragma once
#ifndef PCF2129_H
#define PCF2129_H

#ifdef TEENSYDUINO
#include <i2c_t3.h>
#else
#include <Wire.h>
#endif

namespace EmbeddedDevices
{
    class PCF2129
    {
        enum class REG
        {
            CONTROL1 = 0x00,
            CONTROL2 = 0x01,
            CONTROL3 = 0x02,
            SECONDS = 0x03,
            MINUTES = 0x04,
            HOURS = 0x05,
            DAYS = 0x06,
            WEEKDAYS = 0x07,
            MONTHS = 0x08,
            YEARS = 0x09,
            SECOND_ALARM = 0x0A,
            MINUTE_ALARM = 0x0B,
            HOUR_ALARM = 0x0C,
            DAY_ALARM = 0x0D,
            WEEKDAY_ALARM = 0x0E,
            CLKOUT_CTL = 0x0F,
            WATCHDG_TIM_CTL = 0x10,
            WATCHDG_TIM_VAL = 0x11,
            TIMESTP_CTL = 0x12,
            SEC_TIMESTP = 0x13,
            MIN_TIMESTP = 0x14,
            HOUR_TIMESTP = 0x15,
            DAY_TIMESTP = 0x16,
            MON_TIMESTP = 0x17,
            YEAR_TIMESTP = 0x18,
            AGING_OFFSET = 0x19,
            INTERNAL_REG = 0x1A
        };
        enum class REG_VALUE
        {
            CONTROL1_STOP_0 = 0x09,
            CONTROL1_STOP_1 = 0x29,
            WATCHDG_TIM_CTL_TI_TP_0 = 0x03
        };

        const uint8_t I2C_ADDR {0x51};
        const uint8_t pin_INT;
        volatile bool b_interrupt;

        struct DateTime
        {
            uint8_t sec;         // 秒
            uint8_t min;         // 分
            uint8_t hour;         // 時間
            uint8_t day;         // 日
            uint8_t wday;        // 曜日
            uint8_t mon;        // 月
            uint8_t year;        // 年
        };

    public:

        explicit PCF2129(uint8_t pin_int)
        : pin_INT(pin_int)
        , b_interrupt(false)
        {
        }

        uint8_t second()  { return (read((uint8_t)REG::SECONDS)  & 0x7F); }
        uint8_t minute()  { return (read((uint8_t)REG::MINUTES)  & 0x7F); }
        uint8_t hour()    { return (read((uint8_t)REG::HOURS)    & 0x1F); }
        uint8_t day()     { return (read((uint8_t)REG::DAYS)     & 0x3F); }
        uint8_t weekday() { return (read((uint8_t)REG::WEEKDAYS) & 0x07); }
        uint8_t month()   { return (read((uint8_t)REG::MONTHS)   & 0x1F); }
        uint8_t year()    { return (read((uint8_t)REG::YEARS)    & 0xFF); }

        void setup()
        {
            pinMode(pin_INT, INPUT); // THIS SHOULD NOT BE "INPUT_PULLUP"
            Wire.begin();
            writeRegister(REG::WATCHDG_TIM_CTL, (uint8_t)REG_VALUE::WATCHDG_TIM_CTL_TI_TP_0);
            stop();
        }

        void start()
        {
            writeRegister(REG::CONTROL1, (uint8_t)REG_VALUE::CONTROL1_STOP_0);
            resumeInterrupt();
        }

        void stop()
        {
            writeRegister(REG::CONTROL1, (uint8_t)REG_VALUE::CONTROL1_STOP_1);
            resumeInterrupt();
        }

        uint8_t getIntPin() const { return pin_INT; }
        void setInterrupted() { b_interrupt = true; }
        void resumeInterrupt()
        {
            writeRegister(REG::CONTROL2, 0x00);
            b_interrupt = false;
        }
        bool isInterrupted() const { return b_interrupt; }

        int writeRegister(REG reg, uint8_t data)
        {
            Wire.beginTransmission(I2C_ADDR);
            Wire.write((uint8_t)reg);
            Wire.write(data);
            int err = Wire.endTransmission();
            if (err != 0)
            {
                Serial.print("I2C error : ");
                Serial.println(err);
            }
            return err;
        }

        uint8_t read(const uint8_t reg)
        {
            Wire.beginTransmission(I2C_ADDR);
            Wire.write((uint8_t)reg);
            Wire.endTransmission();

            Wire.requestFrom((int)I2C_ADDR, 1, true);  // blocking read (request 256 bytes)

            if (Wire.available()) return Wire.read();
            else                  return 0x00;
        }
    };

} // namespace EmbeddedDevices

using PCF2129 = EmbeddedDevices::PCF2129;

#endif // PCF2129_H
