#include "shell.h"
#include "../drivers/screen/screen.h"
#include "../drivers/keyboard/keyboard.h"

#define SHELL_BUFFER_SIZE 128

static char command_buffer[SHELL_BUFFER_SIZE];
static size_t command_length = 0;

static int shell_streq(const char* a, const char* b)
{
    size_t i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
        {
            return 0;
        }
        i++;
    }
    return a[i] == b[i];
}

static int shell_starts_with(const char* text, const char* prefix)
{
    size_t i = 0;
    while (prefix[i] != '\0')
    {
        if (text[i] != prefix[i])
        {
            return 0;
        }
        i++;
    }
    return 1;
}

static void shell_print_prompt(void)
{
    print_str("> ");
}

static void shell_run_echo(const char* command)
{
    const char* cursor = command;
    int no_newline = 0;

    while (*cursor == ' ')
    {
        cursor++;
    }

    if (shell_starts_with(cursor, "-n"))
    {
        no_newline = 1;
        cursor += 2;

        while (*cursor == ' ')
        {
            cursor++;
        }
    }

    if (*cursor == '\0')
    {
        if (!no_newline)
        {
            print_char('\n');
        }
        return;
    }

    while (*cursor != '\0')
    {
        const char* start = cursor;
        while (*cursor != '\0' && *cursor != ' ')
        {
            cursor++;
        }

        if (cursor > start)
        {
            for (const char* p = start; p < cursor; p++)
            {
                print_char(*p);
            }
        }

        while (*cursor == ' ')
        {
            cursor++;
        }

        if (*cursor != '\0')
        {
            print_char(' ');
        }
    }

    if (!no_newline)
    {
        print_char('\n');
    }
}

static void shell_execute_command(const char* command)
{
    const char* arg = command;

    while (*arg == ' ')
    {
        arg++;
    }

    if (*arg == '\0')
    {
        shell_print_prompt();
        return;
    }

    if (shell_streq(arg, "help"))
    {
        print_str("Available commands:\n");
        print_str("  help\n");
        print_str("  clear\n");
        print_str("  echo [-n] <text>\n");
        print_str("  about\n");
    }
    else if (shell_streq(arg, "clear"))
    {
        clear_screen();
        shell_print_prompt();
        return;
    }
    else if (shell_streq(arg, "about"))
    {
        print_str("6ix OS - simple keyboard shell\n");
    }
    else if (shell_starts_with(arg, "echo"))
    {
        shell_run_echo(arg + 4);
    }
    else
    {
        print_str("Unknown command: ");
        print_str((char*)arg);
        print_char('\n');
    }

    shell_print_prompt();
}

void shell_init(void)
{
    shell_print_prompt();
}

void shell_handle_input(char c)
{
    if (c == '\n')
    {
        print_char('\n');
        command_buffer[command_length] = '\0';
        shell_execute_command(command_buffer);
        command_length = 0;
        return;
    }

    if (c == '\b')
    {
        if (command_length > 0)
        {
            command_length--;
            print_char('\b');
        }
        return;
    }

    if (command_length < SHELL_BUFFER_SIZE - 1)
    {
        command_buffer[command_length++] = c;
        print_char(c);
    }
}
