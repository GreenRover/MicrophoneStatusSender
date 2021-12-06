# MicrophoneStatusSender

My target is it to provide to operate a presence light but dont got the permission to access the Microsoft Graph api.

This tool is monitroing the usage of your microphone to indicate if your are in a call or not.

For the actual presence light i use a ESP32 running a webserver and accepting status via http get.

## programm arguments

This mini application takes 4 arguments:
1. The name or a part of the name of your microphone
2. A comma sepearted list of application that should not trigger an active status if using the microphone.
3. The HTTP url that should be called when microphone is in use.
4. The HTTP url that should be called when microphone is not used any more.

## programm flow

The web hooks will be called on each status chage and all 30 secs as keyframe.
If the web hook can not be called a error will be logged, but the programm will continue running an retry in 30sec.

## install as service

Using the windows sc tool kit: https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/sc-create

Run this as from a administrator CMD:
```
sc.exe create MicrophoneStatusSender binPath="C:\devsbb\tools\MicrophoneStatusSender.exe \"MDrill Dome\" \"\" \"http://192.168.50.21:80/active\" \"http://192.168.50.21:80/inactive\"" start=auto
```
