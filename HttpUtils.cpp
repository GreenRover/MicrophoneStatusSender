#pragma once
#include "HttpUtils.h"
#include "windows.h"
#include "winhttp.h"
#pragma comment(lib, "winhttp.lib")
#include <iostream>

void HttpUtils::callGet(LPWSTR url) {
	WCHAR szHost[256];
	DWORD dwOpenRequestFlag = 0;
	URL_COMPONENTS urlComp;
	HINTERNET hSession;

	// Initialize URL_COMPONENTS structure.
	ZeroMemory(&urlComp, sizeof(urlComp));
	urlComp.dwStructSize = sizeof(urlComp);

	// Use allocated buffer to store the Host Name.
	urlComp.lpszHostName = szHost;
	urlComp.dwHostNameLength = sizeof(szHost) / sizeof(szHost[0]);

	// Set non zero lengths to obtain pointer to the URL Path.
	/* note: if we threat this pointer as a NULL terminated string
			this pointer will contain Extra Info as well. */
	urlComp.dwUrlPathLength = -1;

	// Crack HTTP scheme.
	urlComp.dwSchemeLength = -1;

	// Create the session handle using the default settings.
	hSession = WinHttpOpen(L"MicrophoneStatusSender 1.1",
		WINHTTP_ACCESS_TYPE_NO_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0);

	// Check to see if the session handle was successfully created.
	if (hSession == NULL)
	{
		throw std::invalid_argument("WinHttpOpen failed : " + GetLastError());
	}

	// Crack the URL.
	if (!WinHttpCrackUrl(url, 0, 0, &urlComp))
	{
		throw std::invalid_argument("WinHttpCrackUrl failed : " + GetLastError());
	}

	// Open an HTTP session.
	HINTERNET hConnect = WinHttpConnect(hSession, szHost, urlComp.nPort, 0);
	if (hConnect == NULL)
	{
		throw std::invalid_argument("WinHttpConnect failed: " + GetLastError());
	}

	// Prepare OpenRequest flag
	dwOpenRequestFlag = (INTERNET_SCHEME_HTTPS == urlComp.nScheme) ? WINHTTP_FLAG_SECURE : 0;

	// Open a "GET" request.
	auto hRequest = WinHttpOpenRequest(hConnect,
		L"GET",
		urlComp.lpszUrlPath,
		NULL,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		dwOpenRequestFlag
	);

	if (hRequest == 0)
	{
		throw std::invalid_argument("WinHttpOpenRequest failed: " + GetLastError());
	}

	// Send the request.
	if (!WinHttpSendRequest(hRequest,
		WINHTTP_NO_ADDITIONAL_HEADERS, 0,
		WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
	{
		throw std::invalid_argument("WinHttpSendRequest failed: " + GetLastError());
	}


	if (!WinHttpReceiveResponse(hRequest, NULL)) {
		throw std::invalid_argument("WinHttpReceiveResponse failed: " + GetLastError());
	}

	DWORD SizeHeaders = 0;
	LPCWSTR header;
	WinHttpQueryHeaders(hRequest, 22, NULL, (LPVOID)&header, &SizeHeaders, NULL);

}