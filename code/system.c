#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/regs/clocks.h"
#include "hardware/pwm.h"

// Input
#define INFRARED_DIGITAL_PIN 3
#define SOUND_DIGITAL_PIN 4

// Output
#define SOUND_LED_PIN 28
#define INFRARED_LED_PIN 26
#define BUZZER_PIN 27
#define SERVO_PIN 16
#define SERVO_LED_PIN 17

// Global variables
unsigned int infrared_sensor_value;
bool openWindowShutter = false;
int currentMillis = 400;
bool direction = true;

void subsystem1()
{
    char received_char = getchar_timeout_us(0);
    if (received_char == '1')
    {
        gpio_put(SOUND_LED_PIN, 1); // Turn on the LED
        // gpio_put(BUZZER_PIN, 1);    // Turn on the BUZZER
    }
    else if (received_char == '0')
    {
        gpio_put(SOUND_LED_PIN, 0); // Turn off the LED
        // gpio_put(BUZZER_PIN, 0);    // Turn off the BUZZER
    }

    sleep_ms(1000);
}

void subsystem2()
{
    if (infrared_sensor_value == 0)
    {
        printf("openWindowShutter: %d\n", openWindowShutter ? 1 : 0); // Testing

        if (!openWindowShutter)
        {

            while (direction)
            {
                currentMillis += (direction) ? 5 : -5;
                if (currentMillis >= 2400)
                    direction = false;
            }
            pwm_set_gpio_level(SERVO_PIN, (currentMillis / 20000.f) * 39062.f); // Rotate Servo Motor 180
            // pwm_set_gpio_level(SERVO_PIN, 128 * 128); / Rotate Servo Motor 90
            gpio_put(INFRARED_LED_PIN, 1); // Turn on the LED

            sleep_ms(500);
            openWindowShutter = true;
        }
        else
        {
            while (!direction)
            {
                currentMillis += (direction) ? 5 : -5;
                if (currentMillis <= 400)
                    direction = true;
            }
            pwm_set_gpio_level(SERVO_PIN, (400 / 20000.f) * 39062.f); // Rotate Servo Motor 180 back
            // pwm_set_gpio_level(SERVO_PIN, - 128 * 128); / Rotate Servo Motor 90 back
            gpio_put(INFRARED_LED_PIN, 0); // Turn off the LED

            sleep_ms(500);
            openWindowShutter = false;
        }
    }
}

int main()
{
    // Initialize standard I/O to use the USB CDC interface.
    stdio_init_all();
    sleep_ms(2000);

    adc_init();

    // Initialize the pins
    gpio_init(INFRARED_DIGITAL_PIN);
    gpio_init(SOUND_DIGITAL_PIN);
    gpio_init(SOUND_LED_PIN);
    gpio_init(INFRARED_LED_PIN);
    gpio_init(BUZZER_PIN);
    gpio_init(SERVO_LED_PIN);

    gpio_set_dir(INFRARED_DIGITAL_PIN, GPIO_IN);
    gpio_set_dir(SOUND_DIGITAL_PIN, GPIO_IN);
    gpio_set_dir(SOUND_LED_PIN, GPIO_OUT);
    gpio_set_dir(INFRARED_LED_PIN, GPIO_OUT);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_set_dir(SERVO_LED_PIN, GPIO_OUT);

    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    // gpio_set_function(SERVO_LED_PIN, GPIO_FUNC_PWM);

    // Figure out which slice we just connected to the pin
    // unsigned int slice_num = pwm_gpio_to_slice_num(SERVO_LED_PIN);
    unsigned int slice_num = pwm_gpio_to_slice_num(SERVO_PIN); // Get new slice number for each pin.

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();

    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 64.f); // Why 4.f here? Is this a "resolution"-thing?
    pwm_config_set_wrap(&config, 39062.f);

    pwm_init(slice_num, &config, true);

    // pwm_init(slice_num, &config, true);

    while (1)
    {
        // unsigned int sound_sensor_value = gpio_get(SOUND_DIGITAL_PIN);
        infrared_sensor_value = gpio_get(INFRARED_DIGITAL_PIN);
        // Sub System 1
        subsystem1();
        // Sub System 2
        subsystem2();
    }

    return 0;
}
