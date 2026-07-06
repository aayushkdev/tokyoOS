#include <apic.h>
#include <kb.h>
#include <kb_map.h>
#include <stdint.h>
#include <system.h>

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define PS2_STATUS_OUTPUT_FULL 0x01
#define PS2_KEYBOARD_IRQ 1

#define SCANCODE_EXTENDED_PREFIX 0xE0
#define SCANCODE_RELEASED_MASK 0x80

static kb_event_t event_buffer[KB_BUFFER_SIZE];
static uint8_t read_index;
static uint8_t write_index;
static uint8_t event_count;

static uint8_t last_scancode;
static uint8_t current_modifiers;
static uint8_t extended_pending;

static void push_event(kb_event_t event) {
    event_buffer[write_index] = event;
    write_index = (write_index + 1) % KB_BUFFER_SIZE;

    if (event_count == KB_BUFFER_SIZE) {
        read_index = (read_index + 1) % KB_BUFFER_SIZE;
        return;
    }

    event_count++;
}

static void update_modifier(kb_key_t key, uint8_t pressed) {
    uint8_t mask = 0;

    switch (key) {
        case KEY_SHIFT:
            mask = KB_MOD_SHIFT;
            break;
        case KEY_CTRL:
            mask = KB_MOD_CTRL;
            break;
        case KEY_ALT:
            mask = KB_MOD_ALT;
            break;
        case KEY_SUPER:
            mask = KB_MOD_SUPER;
            break;
        case KEY_CAPS_LOCK:
            if (pressed) {
                current_modifiers ^= KB_MOD_CAPS;
            }
            return;
        default:
            return;
    }

    if (pressed) {
        current_modifiers |= mask;
    } else {
        current_modifiers &= ~mask;
    }
}

static void handle_scancode(uint8_t scancode) {
    if (scancode == SCANCODE_EXTENDED_PREFIX) {
        extended_pending = 1;
        return;
    }

    uint8_t pressed = (scancode & SCANCODE_RELEASED_MASK) == 0;
    uint8_t base_scancode = scancode & ~SCANCODE_RELEASED_MASK;
    uint8_t extended = extended_pending;
    kb_key_t key = kb_map_scancode(base_scancode, extended);

    extended_pending = 0;
    if (key == KEY_NONE) {
        return;
    }

    update_modifier(key, pressed);

    kb_event_t event = {
        .key = key,
        .raw_scancode = base_scancode,
        .modifiers = current_modifiers,
        .pressed = pressed,
        .extended = extended,
    };

    push_event(event);

}

int init_kb(void) {
    read_index = 0;
    write_index = 0;
    event_count = 0;
    last_scancode = 0;
    current_modifiers = 0;
    extended_pending = 0;

    return ioapic_route_irq(PS2_KEYBOARD_IRQ, KB_VECTOR);
}

void kb_handle_irq(void) {
    if ((inb(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_FULL) != 0) {
        last_scancode = inb(PS2_DATA_PORT);
        handle_scancode(last_scancode);
    }

    lapic_eoi();
}

uint8_t kb_last_scancode(void) {
    return last_scancode;
}

int kb_read_event(kb_event_t *event) {
    if (event == 0 || event_count == 0) {
        return 0;
    }

    *event = event_buffer[read_index];
    read_index = (read_index + 1) % KB_BUFFER_SIZE;
    event_count--;
    return 1;
}

char kb_event_to_char(kb_event_t event) {
    return kb_map_event_to_char(event);
}
