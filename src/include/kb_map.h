#pragma once

#include <kb.h>
#include <stdint.h>

kb_key_t kb_map_scancode(uint8_t scancode, uint8_t extended);
char kb_map_event_to_char(kb_event_t event);
