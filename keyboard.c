// --- File: keyboard.c ---
// This file translates keyboard scancodes into ASCII characters.

// This function is defined in keyboard.asm
extern unsigned char read_scancode(void);

// A basic scancode-to-ASCII mapping. This only handles US keyboard layout.
// Unhandled keys are mapped to 0.
const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// A simple function to get a character from the keyboard.
// It waits until a key is pressed and released.
char getchar() {
    unsigned char scancode;
    char ascii_char;

    // Wait for a key press
    // Scancodes for key presses are in the range 0x01 - 0x80
    do {
        scancode = read_scancode();
    } while (scancode > 0x80);

    // Wait for the key to be released (this prevents repeated characters)
    // Scancodes for key releases are the press code + 0x80
    unsigned char release_scancode;
    do {
        release_scancode = read_scancode();
    } while (release_scancode < 0x80);

    ascii_char = scancode_to_ascii[scancode];
    return ascii_char;
}
