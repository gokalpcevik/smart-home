#pragma once
#include <Arduino.h>

enum Command
{
    CMD_BRIGHTNESS=0x0,
    CMD_POWER=0x1,
    CMD_SMARTLIGHT=0x2,
    CMD_SUNLIGHT=0x3,
    CMD_WINDOW=0x4,
    CMD_DOOR=0x5,
    CMD_SELECT_COLOR=0x6
};

enum Room
{
    INVALID=0x0,
    LIVING_ROOM=0x1,
    BED_ROOM=0x2
};

enum COLOR
{
    Red=0x0,
    Green=0x1,
    Blue=0x2,
    Cyan=0x3,
    Off=0x4
};