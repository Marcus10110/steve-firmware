# steve-firmware
Firmware for Steve the Bartending Robot


## API

### Functions

#### setRevision

This function is used to set the device revision. Possible options are:
- "SteveV1"
- "SteveV2"
- "Unset"

The function will return 0 if successful and -1 if unsuccessful.

#### StartDrink

This function takes the time, in seconds, that each pump should be turned on for.

If the current revision is set to SteveV1, then 12 pump values must be sent.

If the current revision it set to SteveV2, then 4 pump values must be sent.

The function will return 0 if successful and -1 if unsuccessful.

Example strings:
```
"0,0,0,0"
"5,1,1,15"
"0,0,0,0,0,0,0,0,0,0,0,0"
"0,5,0,0,7,0,0,13,0,0,0,0"
```

### Variables

#### device_name

This is the name of the photon unit. It can be changed through the Particle app.

#### revision

This is the current set revision of the firmware.

If "Unset" the device will not be able to make drinks.
If "SteveV1" the device is configured as the original Steve design.
If "SteveV2" the device is configured as the new 12 pump design.

#### isReady

Boolean. If true, the revision is set to either SteveV1 or SteveV2, and no drinks are currently in progress.

#### timeLeft

The time left, in milliseconds, until the current drink is finished.

#### lastError

If an error has occurred (one of the functions returned -1) this string will indicate what went wrong.
It cannot be cleared.