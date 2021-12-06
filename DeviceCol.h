#pragma once
#include <MMDeviceAPI.h>
#include <AudioPolicy.h>

struct DeviceCol {
    IMMDeviceCollection* dCol;
    IAudioSessionManager2* pSessionManager;
};
