// Camera pins definitions for different boards
#if defined(MP_CAMERA_MODEL_WROVER_KIT)
#define MICROPY_CAMERA_PIN_PWDN    -1
#define MICROPY_CAMERA_PIN_RESET   -1
#define MICROPY_CAMERA_PIN_XCLK    21
#define MICROPY_CAMERA_PIN_SIODM   26
#define MICROPY_CAMERA_PIN_SIOC    27

#define MICROPY_CAMERA_PIN_D7      35
#define MICROPY_CAMERA_PIN_D6      34
#define MICROPY_CAMERA_PIN_D5      39
#define MICROPY_CAMERA_PIN_D4      36
#define MICROPY_CAMERA_PIN_D3      19
#define MICROPY_CAMERA_PIN_D2      18
#define MICROPY_CAMERA_PIN_D1       5
#define MICROPY_CAMERA_PIN_D0       4
#define MICROPY_CAMERA_PIN_VSYNC   25
#define MICROPY_CAMERA_PIN_HREF    23
#define MICROPY_CAMERA_PIN_PCLK    22

#elif defined(MP_CAMERA_MODEL_ESP_EYE)
#define MICROPY_CAMERA_PIN_PWDN    -1
#define MICROPY_CAMERA_PIN_RESET   -1
#define MICROPY_CAMERA_PIN_XCLK    4
#define MICROPY_CAMERA_PIN_SIODM   18
#define MICROPY_CAMERA_PIN_SIOC    23

#define MICROPY_CAMERA_PIN_D7      36
#define MICROPY_CAMERA_PIN_D6      37
#define MICROPY_CAMERA_PIN_D5      38
#define MICROPY_CAMERA_PIN_D4      39
#define MICROPY_CAMERA_PIN_D3      35
#define MICROPY_CAMERA_PIN_D2      14
#define MICROPY_CAMERA_PIN_D1      13
#define MICROPY_CAMERA_PIN_D0      34
#define MICROPY_CAMERA_PIN_VSYNC   5
#define MICROPY_CAMERA_PIN_HREF    27
#define MICROPY_CAMERA_PIN_PCLK    25

#define LED_GPIO_NUM     22

#elif defined(MP_CAMERA_MODEL_M5STACK_PSRAM)
#define MICROPY_CAMERA_PIN_PWDN     -1
#define MICROPY_CAMERA_PIN_RESET    15
#define MICROPY_CAMERA_PIN_XCLK     27
#define MICROPY_CAMERA_PIN_SIODM    25
#define MICROPY_CAMERA_PIN_SIOC     23

#define MICROPY_CAMERA_PIN_D7       19
#define MICROPY_CAMERA_PIN_D6       36
#define MICROPY_CAMERA_PIN_D5       18
#define MICROPY_CAMERA_PIN_D4       39
#define MICROPY_CAMERA_PIN_D3        5
#define MICROPY_CAMERA_PIN_D2       34
#define MICROPY_CAMERA_PIN_D1       35
#define MICROPY_CAMERA_PIN_D0       32
#define MICROPY_CAMERA_PIN_VSYNC    22
#define MICROPY_CAMERA_PIN_HREF     26
#define MICROPY_CAMERA_PIN_PCLK     21

#elif defined(MP_CAMERA_MODEL_M5STACK_V2_PSRAM)
#define MICROPY_CAMERA_PIN_PWDN     -1
#define MICROPY_CAMERA_PIN_RESET    15
#define MICROPY_CAMERA_PIN_XCLK     27
#define MICROPY_CAMERA_PIN_SIODM    22
#define MICROPY_CAMERA_PIN_SIOC     23

#define MICROPY_CAMERA_PIN_D7       19
#define MICROPY_CAMERA_PIN_D6       36
#define MICROPY_CAMERA_PIN_D5       18
#define MICROPY_CAMERA_PIN_D4       39
#define MICROPY_CAMERA_PIN_D3        5
#define MICROPY_CAMERA_PIN_D2       34
#define MICROPY_CAMERA_PIN_D1       35
#define MICROPY_CAMERA_PIN_D0       32
#define MICROPY_CAMERA_PIN_VSYNC    25
#define MICROPY_CAMERA_PIN_HREF     26
#define MICROPY_CAMERA_PIN_PCLK     21

#elif defined(MP_CAMERA_MODEL_M5STACK_WIDE)
#define MICROPY_CAMERA_PIN_PWDN     -1
#define MICROPY_CAMERA_PIN_RESET    15
#define MICROPY_CAMERA_PIN_XCLK     27
#define MICROPY_CAMERA_PIN_SIODM    22
#define MICROPY_CAMERA_PIN_SIOC     23

#define MICROPY_CAMERA_PIN_D7       19
#define MICROPY_CAMERA_PIN_D6       36
#define MICROPY_CAMERA_PIN_D5       18
#define MICROPY_CAMERA_PIN_D4       39
#define MICROPY_CAMERA_PIN_D3        5
#define MICROPY_CAMERA_PIN_D2       34
#define MICROPY_CAMERA_PIN_D1       35
#define MICROPY_CAMERA_PIN_D0       32
#define MICROPY_CAMERA_PIN_VSYNC    25
#define MICROPY_CAMERA_PIN_HREF     26
#define MICROPY_CAMERA_PIN_PCLK     21

#elif defined(MP_CAMERA_MODEL_M5STACK_ESP32CAM)
#define MICROPY_CAMERA_PIN_PWDN     -1
#define MICROPY_CAMERA_PIN_RESET    15
#define MICROPY_CAMERA_PIN_XCLK     27
#define MICROPY_CAMERA_PIN_SIODM    25
#define MICROPY_CAMERA_PIN_SIOC     23

#define MICROPY_CAMERA_PIN_D7       19
#define MICROPY_CAMERA_PIN_D6       36
#define MICROPY_CAMERA_PIN_D5       18
#define MICROPY_CAMERA_PIN_D4       39
#define MICROPY_CAMERA_PIN_D3        5
#define MICROPY_CAMERA_PIN_D2       34
#define MICROPY_CAMERA_PIN_D1       35
#define MICROPY_CAMERA_PIN_D0       17
#define MICROPY_CAMERA_PIN_VSYNC    22
#define MICROPY_CAMERA_PIN_HREF     26
#define MICROPY_CAMERA_PIN_PCLK     21

#elif defined(MP_CAMERA_MODEL_M5STACK_UNITCAM)
#define MICROPY_CAMERA_PIN_PWDN     -1
#define MICROPY_CAMERA_PIN_RESET    15
#define MICROPY_CAMERA_PIN_XCLK     27
#define MICROPY_CAMERA_PIN_SIODM    25
#define MICROPY_CAMERA_PIN_SIOC     23

#define MICROPY_CAMERA_PIN_D7       19
#define MICROPY_CAMERA_PIN_D6       36
#define MICROPY_CAMERA_PIN_D5       18
#define MICROPY_CAMERA_PIN_D4       39
#define MICROPY_CAMERA_PIN_D3        5
#define MICROPY_CAMERA_PIN_D2       34
#define MICROPY_CAMERA_PIN_D1       35
#define MICROPY_CAMERA_PIN_D0       32
#define MICROPY_CAMERA_PIN_VSYNC    22
#define MICROPY_CAMERA_PIN_HREF     26
#define MICROPY_CAMERA_PIN_PCLK     21

#elif defined(MP_CAMERA_MODEL_M5STACK_CAMS3_UNIT)
#define MICROPY_CAMERA_PIN_PWDN  -1
#define MICROPY_CAMERA_PIN_RESET 21
#define MICROPY_CAMERA_PIN_XCLK  11
#define MICROPY_CAMERA_PIN_SIODM 17
#define MICROPY_CAMERA_PIN_SIOC  41

#define MICROPY_CAMERA_PIN_D7    13
#define MICROPY_CAMERA_PIN_D6    4
#define MICROPY_CAMERA_PIN_D5    10
#define MICROPY_CAMERA_PIN_D4    5
#define MICROPY_CAMERA_PIN_D3    7
#define MICROPY_CAMERA_PIN_D2    16
#define MICROPY_CAMERA_PIN_D1    15
#define MICROPY_CAMERA_PIN_D0    6
#define MICROPY_CAMERA_PIN_VSYNC 42
#define MICROPY_CAMERA_PIN_HREF  18
#define MICROPY_CAMERA_PIN_PCLK  12

#elif defined(MP_CAMERA_MODEL_AI_THINKER)
#define MICROPY_CAMERA_PIN_PWDN     32
#define MICROPY_CAMERA_PIN_RESET    -1
#define MICROPY_CAMERA_PIN_XCLK     0
#define MICROPY_CAMERA_PIN_SIODM    26
#define MICROPY_CAMERA_PIN_SIOC     27

#define MICROPY_CAMERA_PIN_D7       35
#define MICROPY_CAMERA_PIN_D6       34
#define MICROPY_CAMERA_PIN_D5       39
#define MICROPY_CAMERA_PIN_D4       36
#define MICROPY_CAMERA_PIN_D3       21
#define MICROPY_CAMERA_PIN_D2       19
#define MICROPY_CAMERA_PIN_D1       18
#define MICROPY_CAMERA_PIN_D0        5
#define MICROPY_CAMERA_PIN_VSYNC    25
#define MICROPY_CAMERA_PIN_HREF     23
#define MICROPY_CAMERA_PIN_PCLK     22

#elif defined(MP_CAMERA_MODEL_TTGO_T_JOURNAL)
#define MICROPY_CAMERA_PIN_PWDN      0
#define MICROPY_CAMERA_PIN_RESET    15
#define MICROPY_CAMERA_PIN_XCLK     27
#define MICROPY_CAMERA_PIN_SIODM    25
#define MICROPY_CAMERA_PIN_SIOC     23

#define MICROPY_CAMERA_PIN_D7       19
#define MICROPY_CAMERA_PIN_D6       36
#define MICROPY_CAMERA_PIN_D5       18
#define MICROPY_CAMERA_PIN_D4       39
#define MICROPY_CAMERA_PIN_D3        5
#define MICROPY_CAMERA_PIN_D2       34
#define MICROPY_CAMERA_PIN_D1       35
#define MICROPY_CAMERA_PIN_D0       17
#define MICROPY_CAMERA_PIN_VSYNC    22
#define MICROPY_CAMERA_PIN_HREF     26
#define MICROPY_CAMERA_PIN_PCLK     21

#elif defined(MP_CAMERA_MODEL_XIAO_ESP32S3)
#define MICROPY_CAMERA_PIN_PWDN     -1
#define MICROPY_CAMERA_PIN_RESET    -1
#define MICROPY_CAMERA_PIN_XCLK     10
#define MICROPY_CAMERA_PIN_SIODM    40
#define MICROPY_CAMERA_PIN_SIOC     39

#define MICROPY_CAMERA_PIN_D7       48
#define MICROPY_CAMERA_PIN_D6       11
#define MICROPY_CAMERA_PIN_D5       12
#define MICROPY_CAMERA_PIN_D4       14
#define MICROPY_CAMERA_PIN_D3       16
#define MICROPY_CAMERA_PIN_D2       18
#define MICROPY_CAMERA_PIN_D1       17
#define MICROPY_CAMERA_PIN_D0       15
#define MICROPY_CAMERA_PIN_VSYNC    38
#define MICROPY_CAMERA_PIN_HREF     47
#define MICROPY_CAMERA_PIN_PCLK     13

#elif defined(MP_CAMERA_MODEL_ESP32_MP_CAMERA_BOARD)
// The 18 pin header on the board has Y5 and Y3 swapped
#define ESP32_MP_CAMERA_BOARD_HEADER 0 
#define MICROPY_CAMERA_PIN_PWDN    32
#define MICROPY_CAMERA_PIN_RESET   33
#define MICROPY_CAMERA_PIN_XCLK    4
#define MICROPY_CAMERA_PIN_SIODM   18
#define MICROPY_CAMERA_PIN_SIOC    23

#define MICROPY_CAMERA_PIN_D7      36
#define MICROPY_CAMERA_PIN_D6      19
#define MICROPY_CAMERA_PIN_D5      21
#define MICROPY_CAMERA_PIN_D4      39
#if ESP32_MP_CAMERA_BOARD_HEADER
#define MICROPY_CAMERA_PIN_D3      13
#else
#define MICROPY_CAMERA_PIN_D3      35
#endif
#define MICROPY_CAMERA_PIN_D2      14
#if ESP32_MP_CAMERA_BOARD_HEADER
#define MICROPY_CAMERA_PIN_D1      35
#else
#define MICROPY_CAMERA_PIN_D1      13
#endif
#define MICROPY_CAMERA_PIN_D0      34
#define MICROPY_CAMERA_PIN_VSYNC    5
#define MICROPY_CAMERA_PIN_HREF    27
#define MICROPY_CAMERA_PIN_PCLK    25

#elif defined(MP_CAMERA_MODEL_ESP32S3_CAM_LCD)
#define MICROPY_CAMERA_PIN_PWDN     -1
#define MICROPY_CAMERA_PIN_RESET    -1
#define MICROPY_CAMERA_PIN_XCLK     40
#define MICROPY_CAMERA_PIN_SIODM    17
#define MICROPY_CAMERA_PIN_SIOC     18

#define MICROPY_CAMERA_PIN_D7       39
#define MICROPY_CAMERA_PIN_D6       41
#define MICROPY_CAMERA_PIN_D5       42
#define MICROPY_CAMERA_PIN_D4       12
#define MICROPY_CAMERA_PIN_D3       3
#define MICROPY_CAMERA_PIN_D2       14
#define MICROPY_CAMERA_PIN_D1       47
#define MICROPY_CAMERA_PIN_D0       13
#define MICROPY_CAMERA_PIN_VSYNC    21
#define MICROPY_CAMERA_PIN_HREF     38
#define MICROPY_CAMERA_PIN_PCLK     11

#elif defined(MP_CAMERA_MODEL_ESP32S3_EYE) || defined(MP_CAMERA_MODEL_FREENOVE_ESP32S3_CAM)
#define MICROPY_CAMERA_PIN_PWDN     -1
#define MICROPY_CAMERA_PIN_RESET    -1
#define MICROPY_CAMERA_PIN_XCLK     15
#define MICROPY_CAMERA_PIN_SIODM    4
#define MICROPY_CAMERA_PIN_SIOC     5

#define MICROPY_CAMERA_PIN_D0       11
#define MICROPY_CAMERA_PIN_D1       9
#define MICROPY_CAMERA_PIN_D2       8
#define MICROPY_CAMERA_PIN_D3       10
#define MICROPY_CAMERA_PIN_D4       12
#define MICROPY_CAMERA_PIN_D5       18
#define MICROPY_CAMERA_PIN_D6       17
#define MICROPY_CAMERA_PIN_D7       16
#define MICROPY_CAMERA_PIN_VSYNC    6
#define MICROPY_CAMERA_PIN_HREF     7
#define MICROPY_CAMERA_PIN_PCLK     13

#elif defined(MP_CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3) || defined(MP_CAMERA_MODEL_DFRobot_Romeo_ESP32S3)
#define MICROPY_CAMERA_PIN_PWDN     -1
#define MICROPY_CAMERA_PIN_RESET    -1
#define MICROPY_CAMERA_PIN_XCLK     45
#define MICROPY_CAMERA_PIN_SIODM    1
#define MICROPY_CAMERA_PIN_SIOC     2

#define MICROPY_CAMERA_PIN_D7       48
#define MICROPY_CAMERA_PIN_D6       46
#define MICROPY_CAMERA_PIN_D5       8
#define MICROPY_CAMERA_PIN_D4       7
#define MICROPY_CAMERA_PIN_D3       4
#define MICROPY_CAMERA_PIN_D2       41
#define MICROPY_CAMERA_PIN_D1       40
#define MICROPY_CAMERA_PIN_D0       39
#define MICROPY_CAMERA_PIN_VSYNC    6
#define MICROPY_CAMERA_PIN_HREF     42
#define MICROPY_CAMERA_PIN_PCLK     5

#elif defined(MP_CAMERA_MODEL_TTGO_T_CAMERA_PLUS)
#define MICROPY_CAMERA_PIN_PWDN    -1
#define MICROPY_CAMERA_PIN_RESET   -1
#define MICROPY_CAMERA_PIN_XCLK    4
#define MICROPY_CAMERA_PIN_SIODM   18
#define MICROPY_CAMERA_PIN_SIOC    23

#define MICROPY_CAMERA_PIN_D7      36
#define MICROPY_CAMERA_PIN_D6      37
#define MICROPY_CAMERA_PIN_D5      38
#define MICROPY_CAMERA_PIN_D4      39
#define MICROPY_CAMERA_PIN_D3      35
#define MICROPY_CAMERA_PIN_D2      26
#define MICROPY_CAMERA_PIN_D1      13
#define MICROPY_CAMERA_PIN_D0      34
#define MICROPY_CAMERA_PIN_VSYNC   5
#define MICROPY_CAMERA_PIN_HREF    27
#define MICROPY_CAMERA_PIN_PCLK    25

#elif defined(MP_CAMERA_MODEL_NEW_ESPS3_RE1_0)
// aliexpress board with label RE:1.0, uses slow 8MB QSPI PSRAM, only 4MB addressable
#define MICROPY_CAMERA_PIN_PWDN     -1
#define MICROPY_CAMERA_PIN_RESET    -1
#define MICROPY_CAMERA_PIN_XCLK     10
#define MICROPY_CAMERA_PIN_SIODM    21
#define MICROPY_CAMERA_PIN_SIOC     14

#define MICROPY_CAMERA_PIN_D7       11
#define MICROPY_CAMERA_PIN_D6       9
#define MICROPY_CAMERA_PIN_D5       8
#define MICROPY_CAMERA_PIN_D4       6
#define MICROPY_CAMERA_PIN_D3       4
#define MICROPY_CAMERA_PIN_D2       2
#define MICROPY_CAMERA_PIN_D1       3
#define MICROPY_CAMERA_PIN_D0       5
#define MICROPY_CAMERA_PIN_VSYNC    13
#define MICROPY_CAMERA_PIN_HREF     12
#define MICROPY_CAMERA_PIN_PCLK     7

#elif defined(MP_CAMERA_MODEL_XENOIONEX)
#define MICROPY_CAMERA_PIN_PWDN    -1
#define MICROPY_CAMERA_PIN_RESET   -1
#define MICROPY_CAMERA_PIN_XCLK    1 // Can use 
#define MICROPY_CAMERA_PIN_SIODM   8 // Can use other i2c SDA pin, set this to -1 | If not using i2c set to 8 or 47
#define MICROPY_CAMERA_PIN_SIOC    9 // Can use other i2c SCL pin, set this to -1 | If not using i2c set to 9 or 21

#define MICROPY_CAMERA_PIN_D7      3  //D7
#define MICROPY_CAMERA_PIN_D6      18 //D6
#define MICROPY_CAMERA_PIN_D5      42 //D5
#define MICROPY_CAMERA_PIN_D4      16 //D4
#define MICROPY_CAMERA_PIN_D3      41 //D3
#define MICROPY_CAMERA_PIN_D2      17 //D2
#define MICROPY_CAMERA_PIN_D1      40 //D1
#define MICROPY_CAMERA_PIN_D0      39 //D0
#define MICROPY_CAMERA_PIN_VSYNC   45
#define MICROPY_CAMERA_PIN_HREF    38
#define MICROPY_CAMERA_PIN_PCLK    2

#endif // definition of camera pins for different boards
