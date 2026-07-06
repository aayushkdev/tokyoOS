#include <kb.h>
#include <kb_map.h>
#include <stdint.h>

typedef struct {
    kb_key_t key;
    char normal;
    char shifted;
} kb_map_entry_t;

static const kb_map_entry_t kb_map[128] = {
    [0x01] = { KEY_ESCAPE, 0, 0 },
    [0x02] = { KEY_1, '1', '!' },
    [0x03] = { KEY_2, '2', '@' },
    [0x04] = { KEY_3, '3', '#' },
    [0x05] = { KEY_4, '4', '$' },
    [0x06] = { KEY_5, '5', '%' },
    [0x07] = { KEY_6, '6', '^' },
    [0x08] = { KEY_7, '7', '&' },
    [0x09] = { KEY_8, '8', '*' },
    [0x0A] = { KEY_9, '9', '(' },
    [0x0B] = { KEY_0, '0', ')' },
    [0x0C] = { KEY_MINUS, '-', '_' },
    [0x0D] = { KEY_EQUAL, '=', '+' },
    [0x0E] = { KEY_BACKSPACE, 0, 0 },
    [0x0F] = { KEY_TAB, '\t', '\t' },
    [0x10] = { KEY_Q, 'q', 'Q' },
    [0x11] = { KEY_W, 'w', 'W' },
    [0x12] = { KEY_E, 'e', 'E' },
    [0x13] = { KEY_R, 'r', 'R' },
    [0x14] = { KEY_T, 't', 'T' },
    [0x15] = { KEY_Y, 'y', 'Y' },
    [0x16] = { KEY_U, 'u', 'U' },
    [0x17] = { KEY_I, 'i', 'I' },
    [0x18] = { KEY_O, 'o', 'O' },
    [0x19] = { KEY_P, 'p', 'P' },
    [0x1A] = { KEY_LEFT_BRACKET, '[', '{' },
    [0x1B] = { KEY_RIGHT_BRACKET, ']', '}' },
    [0x1C] = { KEY_ENTER, '\n', '\n' },
    [0x1D] = { KEY_CTRL, 0, 0 },
    [0x1E] = { KEY_A, 'a', 'A' },
    [0x1F] = { KEY_S, 's', 'S' },
    [0x20] = { KEY_D, 'd', 'D' },
    [0x21] = { KEY_F, 'f', 'F' },
    [0x22] = { KEY_G, 'g', 'G' },
    [0x23] = { KEY_H, 'h', 'H' },
    [0x24] = { KEY_J, 'j', 'J' },
    [0x25] = { KEY_K, 'k', 'K' },
    [0x26] = { KEY_L, 'l', 'L' },
    [0x27] = { KEY_SEMICOLON, ';', ':' },
    [0x28] = { KEY_APOSTROPHE, '\'', '"' },
    [0x29] = { KEY_GRAVE, '`', '~' },
    [0x2A] = { KEY_SHIFT, 0, 0 },
    [0x2B] = { KEY_BACKSLASH, '\\', '|' },
    [0x2C] = { KEY_Z, 'z', 'Z' },
    [0x2D] = { KEY_X, 'x', 'X' },
    [0x2E] = { KEY_C, 'c', 'C' },
    [0x2F] = { KEY_V, 'v', 'V' },
    [0x30] = { KEY_B, 'b', 'B' },
    [0x31] = { KEY_N, 'n', 'N' },
    [0x32] = { KEY_M, 'm', 'M' },
    [0x33] = { KEY_COMMA, ',', '<' },
    [0x34] = { KEY_DOT, '.', '>' },
    [0x35] = { KEY_SLASH, '/', '?' },
    [0x36] = { KEY_SHIFT, 0, 0 },
    [0x38] = { KEY_ALT, 0, 0 },
    [0x39] = { KEY_SPACE, ' ', ' ' },
    [0x3A] = { KEY_CAPS_LOCK, 0, 0 },
};

static kb_key_t map_extended_scancode(uint8_t scancode) {
    switch (scancode) {
        case 0x1D:
            return KEY_CTRL;
        case 0x38:
            return KEY_ALT;
        case 0x48:
            return KEY_UP;
        case 0x4B:
            return KEY_LEFT;
        case 0x4D:
            return KEY_RIGHT;
        case 0x50:
            return KEY_DOWN;
        case 0x5B:
        case 0x5C:
            return KEY_SUPER;
        default:
            return KEY_NONE;
    }
}

kb_key_t kb_map_scancode(uint8_t scancode, uint8_t extended) {
    if (scancode >= 128) {
        return KEY_NONE;
    }

    return extended ? map_extended_scancode(scancode) : kb_map[scancode].key;
}

char kb_map_event_to_char(kb_event_t event) {
    if (!event.pressed || event.extended || event.raw_scancode >= 128) {
        return 0;
    }

    const kb_map_entry_t *entry = &kb_map[event.raw_scancode];
    uint8_t shifted = (event.modifiers & KB_MOD_SHIFT) != 0;
    uint8_t caps = (event.modifiers & KB_MOD_CAPS) != 0;
    uint8_t letter = event.key >= KEY_A && event.key <= KEY_Z;

    if ((letter && (shifted ^ caps)) || (!letter && shifted)) {
        return entry->shifted;
    }

    return entry->normal;
}
