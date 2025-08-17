// --- File: kernel.c ---
// This is our simple C kernel. It gets called by the bootloader
// after the CPU is in 32-bit protected mode.

// Function to write a single character to a specific screen location
void print_char(char character, int col, int row, char attribute_byte) {
    // The VGA text mode buffer is a 2D array of characters and attributes
    // located at memory address 0xB8000.
    unsigned char *vidmem = (unsigned char *) 0xB8000;

    // Each character cell is 2 bytes: one for the character, one for attributes.
    int offset;
    if (col >= 0 && row >= 0) {
        offset = (row * 80 + col) * 2;
    } else {
        // A simple way to get a "cursor" position if not specified.
        // This is a placeholder for a real cursor implementation.
        offset = 700; // An arbitrary position on screen
    }

    // If attribute_byte is zero, use a default white-on-black style.
    if (!attribute_byte) {
        attribute_byte = 0x0F; // White text on black background
    }

    vidmem[offset] = character;
    vidmem[offset + 1] = attribute_byte;
}

// Main function for our kernel
void main() {
    const char *message = "Welcome to LightOS!";
    int col = 0;
    
    // Loop through the message and print each character to the screen.
    while (*message != 0) {
        print_char(*message, col, 10, 0x0F); // Print at row 10
        message++;
        col++;
    }
}
