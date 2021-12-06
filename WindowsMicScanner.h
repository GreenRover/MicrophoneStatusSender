
#pragma once
#include <iostream>
#include <list>
#include <MMDeviceAPI.h>
#include <AudioPolicy.h>

class WindowsMicScanner
{
public:
	void getDeviceByMicrofoneName(std::wstring micName);
	std::list<std::wstring> getActiveDevices();
private:
	IMMDeviceCollection* dCol;
	IAudioSessionManager2* pSessionManager;
};

