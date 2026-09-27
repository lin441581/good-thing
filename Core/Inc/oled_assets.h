#ifndef OLED_ASSETS_H
#define OLED_ASSETS_H

#include <stdint.h>

#define ICON_WIDTH  32U
#define ICON_HEIGHT 32U
#define ICON_BYTES  (ICON_WIDTH * ICON_HEIGHT / 8U)
#define DINO_WIDTH  16U
#define DINO_HEIGHT 18U
#define DINO_BYTES  (DINO_WIDTH * DINO_HEIGHT / 8U)

extern const uint8_t icon_step[ICON_BYTES];
extern const uint8_t icon_stopwatch[ICON_BYTES];
extern const uint8_t icon_mpu6050[ICON_BYTES];
extern const uint8_t icon_level[ICON_BYTES];
extern const uint8_t icon_led[ICON_BYTES];
extern const uint8_t icon_game[ICON_BYTES];
extern const uint8_t icon_battery[ICON_BYTES];
extern const uint8_t icon_settings[ICON_BYTES];
extern const uint8_t icon_emoji_smile[ICON_BYTES];
extern const uint8_t emoji_frame_3[ICON_BYTES];
extern const uint8_t emoji_frame_4[ICON_BYTES];
extern const uint8_t dino_frame[DINO_BYTES];

#endif
