## Protocol description

Parameters to set:
1) uint8_t num_of_stairs - from 1 to 23
2) uint16_t max_pwm_duty - from 0 to 65535
3) uint32_t stair_light_on_time_ms - from - 100ms to 100000 sec
4) effect number:
 - effect 0 - base effect
 - effect 1 - moving effect
5) Effect settings:
    Effect 0:
    - uint16_t increment_step;    // speed 
    - uint16_t wide;              // wide

    Effect 1:
    - uint16_t increment_step;    // speed

Action:
1) Press switch 1 
2) Press switch 2 
3) Sens top press
4) Sens bottom press


Additional function
1) Press switch to channel ON config


| COMMAND:                | PARAMETER:                      | DATA  
| ------------------------|---------------------------------|-------------------------------------------------------------------------------------------------|
| SET PARAMETER - 0xDE    | num of stair:             0x01  | 1 byte value: 0-23                                                                              |
|                         | max pwm:                  0x02  | 2 byte value: 1 - 65535                                                                         |
|                         | light on time:            0x03  | 4 byte value: 1 - 4 294 967 295 ms                                                              |
|                         | effect number + settings: 0x04  | 1 byte value: 1 or 2 effect type + (effect settiings: 2 byte + 2 byte)                          |
|                         | set all + save:           0xFF  | (num of stair 1 byte) + (max pwm 2 byte) + (light on time 4 byte) + (effect settings 5 byte)    |
|                         | save settings:            0xEE  |                                                                                                 |
| ------------------------|---------------------------------|-------------------------------------------------------------------------------------------------|
| ACTION - 0xAA           | Press switch 1:           0xA1  |                                                                                                 |
|                         | Press switch 2:           0xB1  |                                                                                                 |
|                         | Sens top press:           0x01  |                                                                                                 |
|                         | Sens bottom press:        0x02  |                                                                                                 |
| ------------------------|---------------------------------|-------------------------------------------------------------------------------------------------|
| ADDITIONAL FUN - 0xAF   | Switch to channel:        0x01  | (switch type - 1 byte) + (channel number - 1 byte) + (inverse - 1 byte) + (max brigh - 2 byte ) |










