#include "pico/buttons.h"
#include "pico/buttons/config.h"
#include "hardware/gpio.h"

void buttons_init(void)
{
    gpio_init(BUTTONS_UP_PIN);
    gpio_set_dir(BUTTONS_UP_PIN, GPIO_IN);
    gpio_pull_up(BUTTONS_UP_PIN);

    gpio_init(BUTTONS_DOWN_PIN);
    gpio_set_dir(BUTTONS_DOWN_PIN, GPIO_IN);
    gpio_pull_up(BUTTONS_DOWN_PIN);

    gpio_init(BUTTONS_LEFT_PIN);
    gpio_set_dir(BUTTONS_LEFT_PIN, GPIO_IN);
    gpio_pull_up(BUTTONS_LEFT_PIN);

    gpio_init(BUTTONS_RIGHT_PIN);
    gpio_set_dir(BUTTONS_RIGHT_PIN, GPIO_IN);
    gpio_pull_up(BUTTONS_RIGHT_PIN);

    gpio_init(BUTTONS_A_PIN);
    gpio_set_dir(BUTTONS_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTONS_A_PIN);

    gpio_init(BUTTONS_B_PIN);
    gpio_set_dir(BUTTONS_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTONS_B_PIN);

    gpio_init(BUTTONS_MENU_PIN);
    gpio_set_dir(BUTTONS_MENU_PIN, GPIO_IN);
    gpio_pull_up(BUTTONS_MENU_PIN);
}

uint8_t buttons_read(void)
{
    return (uint8_t)(!gpio_get(BUTTONS_DOWN_PIN) << 7 |
                     !gpio_get(BUTTONS_UP_PIN) << 6 |
                     !gpio_get(BUTTONS_RIGHT_PIN) << 5 |
                     !gpio_get(BUTTONS_LEFT_PIN) << 4 |
                     !gpio_get(BUTTONS_B_PIN) << 1 |
                     !gpio_get(BUTTONS_A_PIN) << 0);
}
