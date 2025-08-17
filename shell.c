// --- File: shell.c ---
// A very simple command-line shell.

// Forward declarations for functions in other files
void print_string(const char* str);
void print_char(char c);
char getchar();

#define MAX_CMD_LEN 100

void launch_shell() {
    char cmd_buffer[MAX_CMD_LEN];
    int cmd_len = 0;

    while (1) {
        print_string("> ");
        cmd_len = 0;

        while (1) {
            char c = getchar();

            if (c == '\n') {
                cmd_buffer[cmd_len] = '\0'; // Null-terminate the string
                print_char('\n');

                // "Execute" the command
                print_string("You typed: ");
                print_string(cmd_buffer);
                print_char('\n');

                break; // Exit inner loop to show a new prompt
            } else if (c == '\b') {
                if (cmd_len > 0) {
                    cmd_len--;
                    print_char('\b'); // Handle backspace on screen
                }
            } else if (cmd_len < MAX_CMD_LEN - 1) {
                cmd_buffer[cmd_len] = c;
                cmd_len++;
                print_char(c); // Echo character to screen
            }
        }
    }
}
