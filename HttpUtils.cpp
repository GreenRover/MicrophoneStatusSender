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
	BOOL fRet = FALSE;
	HINTERNET hSession;

	WINHTTP_AUTOPROXY_OPTIONS AutoProxyOptions = { 0 };
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG IEProxyConfig;
	WINHTTP_PROXY_INFO  proxyInfo = { 0 };

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

	DWORD dwAccessType = WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY; // WINHTTP_ACCESS_TYPE_DEFAULT_PROXY
	bool bAutomaticProxyConfiguration = false;

	// Create the session handle using the default settings.
	hSession = WinHttpOpen(L"Asynchronous WinHTTP Demo/1.1",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		WINHTTP_FLAG_ASYNC);

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
	auto hConnect = WinHttpConnect(hSession, szHost, urlComp.nPort, 0);
	if (hConnect == NULL)
	{
		throw std::invalid_argument("WinHttpConnect failed: " + GetLastError());
	}

	if (bAutomaticProxyConfiguration)
	{
		if (WinHttpGetIEProxyConfigForCurrentUser(&IEProxyConfig))
		{
			//
			// If IE is configured to autodetect, then we'll autodetect too
			//
			if (IEProxyConfig.fAutoDetect)
			{
				AutoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;

				//
				// Use both DHCP and DNS-based autodetection
				//
				AutoProxyOptions.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP |
					WINHTTP_AUTO_DETECT_TYPE_DNS_A;
			}

			//
			// If there's an autoconfig URL stored in the IE proxy settings, save it
			//
			if (IEProxyConfig.lpszAutoConfigUrl)
			{
				AutoProxyOptions.dwFlags |= WINHTTP_AUTOPROXY_CONFIG_URL;
				AutoProxyOptions.lpszAutoConfigUrl = IEProxyConfig.lpszAutoConfigUrl;
			}

			//
			// If there's a static proxy
			//
			if (IEProxyConfig.lpszProxy)
			{
				AutoProxyOptions.dwFlags |= WINHTTP_AUTOPROXY_ALLOW_STATIC;
			}

			BOOL bResult = WinHttpGetProxyForUrl(hSession,
				urlComp.lpszScheme,
				&AutoProxyOptions,
				&proxyInfo);

			if (!bResult)
			{
				throw std::invalid_argument("WinHttpGetProxyForUrl failed: " + GetLastError());
			}
			else
			{
				/*
				// WinHttpOpen dwAccessType values (also for WINHTTP_PROXY_INFO::dwAccessType)
				#define WINHTTP_ACCESS_TYPE_DEFAULT_PROXY               0
				#define WINHTTP_ACCESS_TYPE_NO_PROXY                    1
				#define WINHTTP_ACCESS_TYPE_NAMED_PROXY					3
				*/

				if (!WinHttpSetOption(hSession,
					WINHTTP_OPTION_PROXY,
					&proxyInfo,
					sizeof(proxyInfo)))
				{
					throw std::invalid_argument("WinHttpSetOption WINHTTP_OPTION_PROXY failed: " + GetLastError());
				}
			}
		}
	}
	// Prepare OpenRequest flag
	dwOpenRequestFlag = (INTERNET_SCHEME_HTTPS == urlComp.nScheme) ?
		WINHTTP_FLAG_SECURE : 0;

	// Open a "GET" request.
	auto hRequest = WinHttpOpenRequest(hConnect,
		L"GET", urlComp.lpszUrlPath,
		NULL, WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		dwOpenRequestFlag);

	if (hRequest == 0)
	{
		throw std::invalid_argument("WinHttpOpenRequest failed: " + GetLastError());
	}

	WINHTTP_PROXY_INFO         ProxyInfo;
	DWORD                      cbProxyInfoSize = sizeof(ProxyInfo);


	if (!WinHttpQueryOption(
		hRequest,
		WINHTTP_OPTION_PROXY,
		&ProxyInfo,
		&cbProxyInfoSize))
	{
		// Exit if setting the proxy info failed.	
		throw std::invalid_argument("WinHttpQueryOption WINHTTP_OPTION_PROXY failed: " + GetLastError());
	}
	else
	{
		/*
		// WinHttpOpen dwAccessType values (also for WINHTTP_PROXY_INFO::dwAccessType)
		#define WINHTTP_ACCESS_TYPE_DEFAULT_PROXY               0
		#define WINHTTP_ACCESS_TYPE_NO_PROXY                    1
		#define WINHTTP_ACCESS_TYPE_NAMED_PROXY					3
		*/
		// printf("\tAccessType : %d\r\n", ProxyInfo.dwAccessType);
	}

	// Send the request.
	if (!WinHttpSendRequest(hRequest,
		WINHTTP_NO_ADDITIONAL_HEADERS, 0,
		WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
	{
		throw std::invalid_argument("WinHttpSendRequest failed: " + GetLastError());
	}
	fRet = TRUE;
}