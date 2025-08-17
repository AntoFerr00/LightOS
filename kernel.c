/* --- File: kernel.c --- */
/* Minimal stable keyboard input kernel */

// VGA text mode buffer
volatile unsigned short* vga_buffer = (unsigned short*)0xB8000;
const int VGA_COLS = 80;
const int VGA_ROWS = 25;

// Current cursor position
int cursor_row = 4;
int cursor_col = 0;

// Simple keyboard scancode to ASCII map
static char scancode_map[128] = {
    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0,
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

// I/O functions
static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "dN"(port));
    return result;
}

static inline void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__ ("outb %1, %0" : : "dN"(port), "a"(data));
}

// Print a character at cursor position
void putc(char c) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else if (c == '\b' && cursor_col > 0) {
        cursor_col--;
        vga_buffer[cursor_row * VGA_COLS + cursor_col] = 0x0F20;
        return;
    } else if (c >= 32) {
        vga_buffer[cursor_row * VGA_COLS + cursor_col] = 0x0F00 | c;
        cursor_col++;
    }
    
    if (cursor_col >= VGA_COLS) {
        cursor_col = 0;
        cursor_row++;
    }
    
    // Simple scroll
    if (cursor_row >= VGA_ROWS) {
        cursor_row = VGA_ROWS - 1;
        // Move everything up
        for (int i = 0; i < VGA_COLS * (VGA_ROWS - 1); i++) {
            vga_buffer[i] = vga_buffer[i + VGA_COLS];
        }
        // Clear bottom line
        for (int i = VGA_COLS * (VGA_ROWS - 1); i < VGA_COLS * VGA_ROWS; i++) {
            vga_buffer[i] = 0x0F20;
        }
    }
}

// Print a string
void puts(const char* str) {
    while (*str) {
        putc(*str++);
    }
}

// Check if keyboard data is available (polling method)
int kbd_data_available() {
    return (inb(0x64) & 0x01);
}

// Read keyboard scancode
unsigned char read_scancode() {
    while (!kbd_data_available()) {
        // Wait for data
    }
    return inb(0x60);
}

// Handle keyboard input (polling-based, no interrupts)
void handle_keyboard() {
    if (!kbd_data_available()) {
        return;
    }
    
    unsigned char scancode = inb(0x60);
    
    // Only handle key press (release has bit 7 set)
    if (scancode & 0x80) {
        return; // Key release, ignore
    }
    
    // Special keys
    if (scancode == 0x1C) { // Enter
        putc('\n');
        putc('>');
        putc(' ');
    } else if (scancode == 0x0E) { // Backspace
        if (cursor_col > 2) { // Don't delete the prompt
            putc('\b');
        }
    } else if (scancode == 0x01) { // ESC
        puts("\n[ESC pressed - LightOS is working!]\n> ");
    } else if (scancode < 128) {
        char c = scancode_map[scancode];
        if (c != 0) {
            putc(c);
        }
    }
}

void kernel_main(void) {
    // Clear screen
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        vga_buffer[i] = 0x0F20;
    }
    
    // Print startup message
    vga_buffer[0] = 0x0F41; // A
    vga_buffer[1] = 0x0F4C; // L
    vga_buffer[2] = 0x0F49; // I
    vga_buffer[3] = 0x0F56; // V
    vga_buffer[4] = 0x0F45; // E
    vga_buffer[5] = 0x0F21; // !
    
    cursor_row = 1;
    cursor_col = 0;
    puts("LightOS - Polling-based keyboard input (stable!)");
    puts("\nType something (Enter for new line, ESC for message):");
    puts("\nThis version uses polling instead of interrupts.");
    
    cursor_row = 4;
    cursor_col = 0;
    puts("> ");
    
    // Keep interrupts disabled - we're using polling
    __asm__ __volatile__ ("cli");
    
    // Main loop - poll keyboard continuously
    while (1) {
        handle_keyboard();
        
        // Small delay to prevent excessive CPU usage
        for (volatile int i = 0; i < 1000; i++) {
            // Do nothing
        }
    }
}