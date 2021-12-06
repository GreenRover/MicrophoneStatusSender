#include <iostream>
#include <string>
#include <list>
#include <chrono>
#include <thread>
#include "WindowsMicScanner.h"
#include "StringUtils.h"
#include "ListUtils.h"
#include "MicrophoneStatusSender.h"
#include "HttpUtils.h"
#include "TimeUtils.h"

void callWebHook(std::wstring url);

int main(int argc, char** argv)
{
    if (argc < 5) {
        std::cerr << "Missing programm argument" << std::endl;
        std::cerr << "Usage:" << std::endl;
        std::cerr << "MicrophoneStatusSender.exe \"microphone name\" \"comma,separated,list,of,applications,to,ignore\" \"http://urlToTriggerOnActive\" \"http://urlToTriggerOnInactive\" :" << std::endl;
        return -2;
    }

    std::wstring micToUse = StringUtils::s2ws(argv[1]);
    std::list<std::wstring> blacklistApps = StringUtils::split(StringUtils::s2ws(argv[2]), L',');
    std::wstring webHookActive = StringUtils::s2ws(argv[3]);
    std::wstring webHookInactive = StringUtils::s2ws(argv[4]);

    try {
        WindowsMicScanner windowsMicScanner;
        windowsMicScanner.getDeviceByMicrofoneName(micToUse);

        bool lastState = false;
        int i = -1;
        while (true) {
            std::list<std::wstring> activeDevices = ListUtils::filterList(
                windowsMicScanner.getActiveDevices(),
                blacklistApps
            );
            bool micIsActive = !activeDevices.empty();

            if (lastState != micIsActive || i == -1 || i++ > 30) {
                try {
                    if (micIsActive) {
                        std::cout << TimeUtils::currentDateTime() << ": Microphone is used by: " << std::endl << "\t";
                        std::cout << StringUtils::join(activeDevices, "\n\t") << std::endl;
                        callWebHook(webHookActive);
                    }
                    else {
                        std::cout << TimeUtils::currentDateTime() << ": No programm is using this microphone" << std::endl;
                        callWebHook(webHookInactive);
                    }
                }
                catch (...) {

                }
                i = 0;
            }
            activeDevices.clear();
            lastState = micIsActive;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (const std::invalid_argument& ia) {
        std::cerr << "Invalid argument: " << ia.what() << '\n';
        return -1;
    }
}

void callWebHook(std::wstring url) {
    try {
        LPWSTR wtUrl = const_cast<LPWSTR>(url.c_str());
        HttpUtils::callGet(wtUrl);
    }
    catch (const std::invalid_argument& ia) {
        std::cerr << "WebHook failed: " << ia.what() << '\n';
    }
}