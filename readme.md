# 2018 Hackaday Supercon Badge Ringtone Editor

The idea is to recreate a ringtone editor similar to the one found on older Nokia cellphones (3210, 3310 etc). The ringtone editor used to represent keys using text like this:
```
8e4 8d4 4#f3 4#g3 8#c4 8#b3 4d3 4e3 8b3 8a3 4#c3 4e3 2.a3
```

each note is composed of:
- duration:
  - A number indicating the denominator of the standard note duration (8 means 1/8th of standard duration, 16 means 1/16th, etc)
  - An optional dot indicating that half the number should be added (2. means 1/2 + 1/4th)
 - key (a-g with an optional # sign)
 - an octave (according to the docs published on hackaday the badge supports 4 octaves). 1 is the lowest, 4 is the highest pitched.

[Reference docs from the Nokia 3310 user manual](https://www.nokia.com/en_int/phones/sites/default/files/user-guides/3310_usersguide_en.pdf)

Initially the ringtone editor will present a blank screen on which a ringtone can be typed (or just press 'n' for a demo). to play the ringtone, press `Enter`. once the ringtone has played, the user is directed back to the editor.

Future ideas include save/load ringtones from the flash memory.

The user_program.c file is meant to be dropped in place of the default user_program.c file in the badge firmware.