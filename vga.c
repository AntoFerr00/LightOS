// --- File: vga.c ---
// Functions for printing to the VGA text-mode buffer.

unsigned char * const VID_MEMORY = (unsigned char *) 0xB8000;
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 25;
const char DEFAULT_COLOR = 0x0F; // White on black

int cursor_x = 0;
int cursor_y = 0;

void clear_screen() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            int offset = (y * SCREEN_WIDTH + x) * 2;
            VID_MEMORY[offset] = ' ';
            VID_MEMORY[offset + 1] = DEFAULT_COLOR;
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void print_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            int offset = (cursor_y * SCREEN_WIDTH + cursor_x) * 2;
            VID_MEMORY[offset] = ' ';
            VID_MEMORY[offset + 1] = DEFAULT_COLOR;
        }
    } else {
        int offset = (cursor_y * SCREEN_WIDTH + cursor_x) * 2;
        VID_MEMORY[offset] = c;
        VID_MEMORY[offset + 1] = DEFAULT_COLOR;
        cursor_x++;
    }

    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    // For now, we don't handle scrolling.
    if (cursor_y >= SCREEN_HEIGHT) {
        clear_screen();
    }
}

void print_string(const char* str) {
    while (*str != 0) {
        print_char(*str);
        str++;
    }
}
