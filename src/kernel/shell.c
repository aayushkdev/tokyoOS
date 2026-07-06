#include <kb.h>
#include <kprintf.h>
#include <shell.h>
#include <stdint.h>
#include <system.h>
#include <timer.h>

#define SHELL_LINE_MAX 128

static char line[SHELL_LINE_MAX];
static uint32_t line_length;

static int string_equal(const char *left, const char *right) {
    while (*left != '\0' && *right != '\0') {
        if (*left != *right) {
            return 0;
        }

        left++;
        right++;
    }

    return *left == *right;
}

static int string_starts_with(const char *value, const char *prefix) {
    while (*prefix != '\0') {
        if (*value != *prefix) {
            return 0;
        }

        value++;
        prefix++;
    }

    return 1;
}

static void shell_prompt(void) {
    kprintf("> ");
}

static void shell_clear_line(void) {
    line_length = 0;
    line[0] = '\0';
}

static void shell_execute(const char *command) {
    if (command[0] == '\0') {
        return;
    }

    if (string_equal(command, "help")) {
        kprintf("commands: help clear ticks echo reboot\n");
        return;
    }

    if (string_equal(command, "clear")) {
        kclear();
        return;
    }

    if (string_equal(command, "ticks")) {
        kprintf("ticks: %d\n", (int)timer_ticks());
        return;
    }

    if (string_starts_with(command, "echo ")) {
        kprintf("%s\n", command + 5);
        return;
    }

    if (string_equal(command, "reboot")) {
        kprintf("rebooting...\n");
        reboot();
        return;
    }

    kprintf("unknown command: %s\n", command);
}

static void shell_handle_char(char c) {
    if (c == '\n') {
        kprintf("\n");
        line[line_length] = '\0';
        shell_execute(line);
        shell_clear_line();
        shell_prompt();
        return;
    }

    if (line_length + 1 >= SHELL_LINE_MAX) {
        return;
    }

    line[line_length++] = c;
    line[line_length] = '\0';
    kprintf("%c", c);
}

static void shell_handle_key(kb_event_t event) {
    if (!event.pressed) {
        return;
    }

    if (event.key == KEY_BACKSPACE) {
        if (line_length > 0) {
            line_length--;
            line[line_length] = '\0';
            kbackspace();
        }
        return;
    }

    char printable = kb_event_to_char(event);
    if (printable != 0) {
        shell_handle_char(printable);
    }
}

void shell_run(void) {
    shell_clear_line();
    shell_prompt();

    while (1) {
        kb_event_t event;
        while (kb_read_event(&event)) {
            shell_handle_key(event);
        }

        asm volatile("hlt");
    }
}
