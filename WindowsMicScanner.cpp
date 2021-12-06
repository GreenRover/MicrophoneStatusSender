#include "WindowsMicScanner.h"
#include <iostream>
#include <string>
#include <list>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <avrt.h>
#include <atlstr.h>
#include <functiondiscoverykeys.h>
#include "StringUtils.h"

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

void WindowsMicScanner::getDeviceByMicrofoneName(std::wstring micName) {
    HRESULT hr = S_OK;
    IMMDeviceEnumerator* pEnumerator = NULL;
    BOOL result = FALSE;

    hr = CoInitialize(0);

    // Create the device enumerator.
    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        (void**)&pEnumerator);

    hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &dCol);
    UINT dCount;
    hr = dCol->GetCount(&dCount);
    std::list<std::wstring> micsFound;
    for (UINT i = 0; i < dCount; i++)
    {
        IMMDevice* pCaptureDevice = NULL;
        hr = dCol->Item(i, &pCaptureDevice);

        IPropertyStore* pProps = NULL;
        hr = pCaptureDevice->OpenPropertyStore(
            STGM_READ, &pProps);

        PROPVARIANT varName;
        // Initialize container for property value.
        PropVariantInit(&varName);

        // Get the endpoint's friendly-name property.
        hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);

        std::wstring nameStr(varName.pwszVal);

        micsFound.push_back(nameStr.c_str());

        // #2 Determine whether it is the microphone device you are focusing on
        std::size_t found = nameStr.find(micName);
        if (found != std::string::npos)
        {
            // Print endpoint friendly name.
            // printf("Endpoint friendly name: \"%S\"\n", varName.pwszVal);

            // Get the session manager.
            hr = pCaptureDevice->Activate(
                __uuidof(IAudioSessionManager2), 
                CLSCTX_ALL,
                NULL, 
                (void**)&pSessionManager
            );
            break;
        }
    }

    // Get session state
    if (!pSessionManager)
    {
        std::string micsFoundStr = micsFound.empty() ? "no active mics" : StringUtils::join(micsFound, ", ");
        throw std::invalid_argument("Not found mic matching filter \"" + StringUtils::ws2s(micName) + "\". Microphones: " + micsFoundStr);
    }
    else {
        micsFound.clear();
    }

    // Clean up.
    SAFE_RELEASE(pEnumerator);
}

std::list<std::wstring> WindowsMicScanner::getActiveDevices() {
    HRESULT hr = S_OK;

    int cbSessionCount = 0;
    LPWSTR pswSession = NULL;

    IAudioSessionEnumerator* pSessionList = NULL;
    IAudioSessionControl* pSessionControl = NULL;
    IAudioSessionControl2* pSessionControl2 = NULL;

    // Get the current list of sessions.
    hr = pSessionManager->GetSessionEnumerator(&pSessionList);

    // Get the session count.
    hr = pSessionList->GetCount(&cbSessionCount);
    //wprintf_s(L"Session count: %d\n", cbSessionCount);

    std::list<std::wstring> appsUsingThisMic;
    for (int index = 0; index < cbSessionCount; index++)
    {
        SAFE_RELEASE(pSessionControl);

        // Get the <n>th session.
        hr = pSessionList->GetSession(index, &pSessionControl);

        hr = pSessionControl->QueryInterface(
            __uuidof(IAudioSessionControl2), (void**)&pSessionControl2);

        // Exclude system sound session
        hr = pSessionControl2->IsSystemSoundsSession();
        if (S_OK == hr)
        {
            //wprintf_s(L" this is a system sound.\n");
            continue;
        }

        // Optional. Determine which application is using Microphone for recording
        LPWSTR instId = NULL;
        hr = pSessionControl2->GetSessionInstanceIdentifier(&instId);
        if (S_OK == hr)
        {
            //wprintf_s(L"SessionInstanceIdentifier: %s\n", instId);
        }

        AudioSessionState state;
        std::wstring winstId;
        hr = pSessionControl->GetState(&state);
        switch (state)
        {
        case AudioSessionStateInactive:
            //wprintf_s(L"Session state: Inactive\n", state);
            break;
        case AudioSessionStateActive:
            // #3 Active state indicates it is recording, otherwise is not recording.
            //wprintf_s(L"Session state: Active\n", state);
            winstId = instId;
            appsUsingThisMic.push_back(winstId.substr(56));
            break;
        case AudioSessionStateExpired:
            //wprintf_s(L"Session state: Expired\n", state);
            break;
        }
    }

    // Clean up.
    SAFE_RELEASE(pSessionControl);
    SAFE_RELEASE(pSessionList);
    SAFE_RELEASE(pSessionControl2);

    return appsUsingThisMic;
}