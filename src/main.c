#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

#define CTRL(k) ((k) & 0x1f)

struct termios orig_termios;

int terminal_width;
int terminal_height;

void die(const char* error)
{
    write(STDOUT_FILENO, "\x1b]2j", 4);
    perror(error);
    exit(0);
}

char readKey()
{
    char c;
    int n;

    while((n = read(STDIN_FILENO, &c, 1) != 1))
    {
        if(n == -1)
        {
            die("readKey");
        }
    }

    return c;
}

void disableRawMode();
void enableRawMode();
void screenRefresh();
void getTerminalSize();
int main(int argc, char* argv[1])
{
    enableRawMode();
    getTerminalSize();

    while(1)
    {
    	c = readKey();
	if(c == CTRL('q')) exit(0);
        printf("%c", c);
    }
    return 0;
}

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* raw.c_cc[VMIN] = 0; */
    /* raw.c_cc[VTIME] = 1; */

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void screenRefresh()
{
    write(STDOUT_FILENO, "\x1b[2j", 4);
}

void getTerminalSize()
{
    struct winsize tsize;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &tsize);
    terminal_height = tsize.ws_row;
    terminal_width  = tsize.ws_col;
}
