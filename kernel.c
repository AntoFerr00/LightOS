 /* --- File: kernel.c --- */
/* LightOS with basic shell and command parsing */

// VGA text mode buffer
volatile unsigned short* vga_buffer = (unsigned short*)0xB8000;
const int VGA_COLS = 80;
const int VGA_ROWS = 25;

// Current cursor position
int cursor_row = 4;
int cursor_col = 0;

// Command buffer
#define CMD_BUFFER_SIZE 256
char cmd_buffer[CMD_BUFFER_SIZE];
int cmd_pos = 0;

// System info
const char* OS_NAME = "LightOS";
const char* OS_VERSION = "v0.2";
int uptime_seconds = 0;

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

// String functions
int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

void strcpy(char* dest, const char* src) {
    while ((*dest++ = *src++));
}

// Print functions
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
    
    if (cursor_row >= VGA_ROWS) {
        cursor_row = VGA_ROWS - 1;
        for (int i = 0; i < VGA_COLS * (VGA_ROWS - 1); i++) {
            vga_buffer[i] = vga_buffer[i + VGA_COLS];
        }
        for (int i = VGA_COLS * (VGA_ROWS - 1); i < VGA_COLS * VGA_ROWS; i++) {
            vga_buffer[i] = 0x0F20;
        }
    }
}

void puts(const char* str) {
    while (*str) {
        putc(*str++);
    }
}

void print_colored(const char* str, unsigned char color) {
    int old_row = cursor_row, old_col = cursor_col;
    while (*str) {
        if (*str == '\n') {
            cursor_col = 0;
            cursor_row++;
        } else if (*str >= 32) {
            vga_buffer[cursor_row * VGA_COLS + cursor_col] = (color << 8) | *str;
            cursor_col++;
        }
        if (cursor_col >= VGA_COLS) {
            cursor_col = 0;
            cursor_row++;
        }
        str++;
    }
}

void print_prompt() {
    print_colored(OS_NAME, 0x0E); // Yellow
    putc(' ');
    print_colored("> ", 0x0A); // Light green
}

// Command functions
void cmd_help() {
    puts("Available commands:\n");
    puts("  help     - Show this help message\n");
    puts("  clear    - Clear the screen\n");
    puts("  info     - Show system information\n");
    puts("  echo     - Echo text back\n");
    puts("  uptime   - Show system uptime\n");
    puts("  reboot   - Restart the system\n");
    puts("  calc     - Simple calculator (add two numbers)\n");
    puts("  colors   - Show color test\n");
}

void cmd_clear() {
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        vga_buffer[i] = 0x0F20;
    }
    cursor_row = 0;
    cursor_col = 0;
    print_colored("Screen cleared!\n", 0x0A);
}

void cmd_info() {
    puts("System Information:\n");
    puts("  OS Name: ");
    print_colored(OS_NAME, 0x0E);
    putc('\n');
    puts("  Version: ");
    print_colored(OS_VERSION, 0x0E);
    putc('\n');
    puts("  Architecture: x86 32-bit\n");
    puts("  Memory: 256MB (emulated)\n");
    puts("  Input: PS/2 Keyboard (polling)\n");
}

void cmd_echo(const char* args) {
    if (strlen(args) > 0) {
        puts("Echo: ");
        print_colored(args, 0x0B); // Light cyan
        putc('\n');
    } else {
        puts("Usage: echo <text>\n");
    }
}

void cmd_uptime() {
    puts("System uptime: ");
    // Simple uptime counter (not real time, just loop iterations)
    char uptime_str[32];
    int minutes = uptime_seconds / 60;
    int seconds = uptime_seconds % 60;
    
    // Simple integer to string conversion
    uptime_str[0] = '0' + (minutes / 10);
    uptime_str[1] = '0' + (minutes % 10);
    uptime_str[2] = ':';
    uptime_str[3] = '0' + (seconds / 10);
    uptime_str[4] = '0' + (seconds % 10);
    uptime_str[5] = '\0';
    
    print_colored(uptime_str, 0x0E);
    puts(" (approximate)\n");
}

void cmd_reboot() {
    print_colored("Rebooting system...\n", 0x0C); // Red

    // This is the standard "8042 keyboard controller" reset method.
    // First, we wait for the controller's input buffer to be empty.
    // Bit 1 of the status port (0x64) is the input buffer status.
    // We wait until it's 0.
    unsigned char status = inb(0x64);
    while ((status & 0x02) != 0) {
        // Small delay while we wait
        for (volatile int i = 0; i < 1000; i++);
        status = inb(0x64);
    }
    
    // Once it's ready, we send the reset command (0xFE) to the command port (0x64).
    outb(0x64, 0xFE);

    // If the reset command fails for some reason, the CPU will get stuck
    // in this loop instead of continuing to execute code. This is a failsafe.
    while(1) {
        __asm__ __volatile__ ("hlt");
    }
}

void cmd_calc(const char* args) {
    // Very simple calculator - just add two single digits
    if (strlen(args) >= 3 && args[1] == '+') {
        int a = args[0] - '0';
        int b = args[2] - '0';
        if (a >= 0 && a <= 9 && b >= 0 && b <= 9) {
            int result = a + b;
            putc('0' + a);
            puts(" + ");
            putc('0' + b);
            puts(" = ");
            if (result >= 10) {
                putc('1');
                putc('0' + (result - 10));
            } else {
                putc('0' + result);
            }
            putc('\n');
        } else {
            puts("Calculator: Enter two single digits like: 5+3\n");
        }
    } else {
        puts("Calculator: Enter two single digits like: 5+3\n");
    }
}

void cmd_colors() {
    puts("Color test:\n");
    for (int i = 0; i < 16; i++) {
        print_colored("Color ", i);
        putc('0' + (i / 10));
        putc('0' + (i % 10));
        putc(' ');
    }
    putc('\n');
}
// --- Corrected Code ---

void execute_command(const char* cmd) {
    if (strlen(cmd) == 0) {
        return;
    }

    // Find the end of the command word (the first space or null terminator)
    const char* end_of_cmd = cmd;
    while (*end_of_cmd && *end_of_cmd != ' ') {
        end_of_cmd++;
    }

    // 1. Calculate the command's length correctly
    int cmd_len = end_of_cmd - cmd;

    // 2. Find where the arguments start
    const char* args = end_of_cmd;
    if (*args == ' ') {
        args++; // Skip the space to point to the actual arguments
    }
    // If there was no space, 'args' correctly points to a null terminator (an empty string)

    // 3. Copy the command into its own buffer
    char command[32];
    if (cmd_len > 31) cmd_len = 31; // Prevent buffer overflow
    for (int i = 0; i < cmd_len; i++) {
        command[i] = cmd[i];
    }
    command[cmd_len] = '\0';

    // Execute commands (This part remains the same)
    if (strcmp(command, "help") == 0) {
        cmd_help();
    } else if (strcmp(command, "clear") == 0) {
        cmd_clear();
    } else if (strcmp(command, "info") == 0) {
        cmd_info();
    } else if (strcmp(command, "echo") == 0) {
        cmd_echo(args);
    } else if (strcmp(command, "uptime") == 0) {
        cmd_uptime();
    } else if (strcmp(command, "reboot") == 0) {
        cmd_reboot();
    } else if (strcmp(command, "calc") == 0) {
        cmd_calc(args);
    } else if (strcmp(command, "colors") == 0) {
        cmd_colors();
    } else {
        puts("Unknown command: ");
        print_colored(command, 0x0C); // Red
        puts("\nType 'help' for available commands.\n");
    }
}

// Keyboard handling
int kbd_data_available() {
    return (inb(0x64) & 0x01);
}

void handle_keyboard() {
    if (!kbd_data_available()) {
        return;
    }
    
    unsigned char scancode = inb(0x60);
    
    if (scancode & 0x80) {
        return; // Key release
    }
    
    if (scancode == 0x1C) { // Enter
        putc('\n');
        cmd_buffer[cmd_pos] = '\0';
        execute_command(cmd_buffer);
        cmd_pos = 0;
        print_prompt();
    } else if (scancode == 0x0E) { // Backspace
        if (cmd_pos > 0) {
            cmd_pos--;
            putc('\b');
        }
    } else if (scancode < 128) {
        char c = scancode_map[scancode];
        if (c != 0 && cmd_pos < CMD_BUFFER_SIZE - 1) {
            cmd_buffer[cmd_pos++] = c;
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
    cursor_row = 0;
    cursor_col = 0;
    print_colored("Welcome to ", 0x0F);
    print_colored(OS_NAME, 0x0E);
    putc(' ');
    print_colored(OS_VERSION, 0x0A);
    putc('\n');
    puts("Basic shell with command parsing enabled!\n");
    puts("Type 'help' to see available commands.\n\n");
    
    print_prompt();
    
    // Keep interrupts disabled - using polling
    __asm__ __volatile__ ("cli");
    
    // Main loop
    int loop_counter = 0;
    while (1) {
        handle_keyboard();
        
        // Simple uptime counter (very approximate)
        loop_counter++;
        if (loop_counter > 1000000) {
            uptime_seconds++;
            loop_counter = 0;
        }
        
        // Small delay
        for (volatile int i = 0; i < 1000; i++) {
            // Do nothing
        }
    }
}