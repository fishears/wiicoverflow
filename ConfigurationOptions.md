Configuration of Cover Floader is done through an XML file located at:
## SD:/usb-loader/wiicoverflow.xml ##

This file is auto-generated upon first use, be can be hand edited.

## General Settings ##
| **Element Name** | **Valid Values** | **Default** | **Description** |
|:-----------------|:-----------------|:------------|:----------------|
| sounds           | 0,1              | 0           | Enable sounds   |
| music            | 0,1              | 1           | Enable music    |
| lock             | 0,1              | 0           |Parental Lock setting, 1 disables the delete button|
| rumble           | 0,1              | 1           | Enable rumble   |
| quickstart       | 0,1              | 0           | Enable "1-Click" Launch of games, no dialog first|

## Graphics Settings ##
|**Element Name**|Valid Values|Default|Description|
|:---------------|:-----------|:------|:----------|
| spacing        |            | 3.1   | Adjusts the distance between covers |
| angle          |            | 90    | Adjusts the angle of cover rotation on the Y axis |
| zoom           |            | -2.0  | Adjust the current zoom distance |
| drawWindow     |            | 7     | Limits the number of covers on either side of center |
| coverText      | 0,1        | 1     | Enable Title text on main screen |
| theme          | 0,1        | 0     | Choose between 0-Black(default) and 1-White(not working) |
| enablepitch    | 0,1        | 0     | allow to pitch zoom in/out when B is held. you must hand edit XML config to enable this|

## Game Settings ##
|**Element Name**|Valid Values|Default|Description|
|:---------------|:-----------|:------|:----------|
| ocarina        | 0,1        | 0     | Ocarina on/off |
| hooktype       | 0,1,2      | 0     | Select Ocarina Hook (VI,Wii Pad, GC Pad) |
| language       | 0,1,2,3,4,5,6,7,8,9,10     | 0     | Select Language to patch |
| video          | 0,1,2,3,4,5      | 0     | Select Video mode patching options |
| vipatch        | 0,1        | 0     | VIDTV patch on/off |
| lastplayed     | gameID     | ""    | 6-digit gameID of last game played for start cover |
| presetFix      | 0,1,2      | 0     | preset game fix for games (none - error002 - anti002) |