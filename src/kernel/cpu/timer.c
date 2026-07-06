#include <apic.h>
#include <timer.h>
#include <stdint.h>

#define TIMER_CALIBRATION_MS 10
#define TIMER_FREQUENCY_HZ 100

static uint64_t ticks;

int init_timer(void) {
    ticks = 0;

    if (lapic_timer_calibrate(TIMER_CALIBRATION_MS) != 0) {
        return -1;
    }

    lapic_timer_start_periodic(TIMER_FREQUENCY_HZ, TIMER_VECTOR);
    return 0;
}

void timer_handle_tick(void) {
    ticks++;
    lapic_eoi();
}

uint64_t timer_ticks(void) {
    return ticks;
}
