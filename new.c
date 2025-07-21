#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define PACMAN 'C'
#define WALL '#'
#define FOOD '.'
#define EMPTY ' '
#define DEMON 'X'

int res = 0;
int score = 0;
int pacman_x, pacman_y;
char board[HEIGHT][WIDTH];
int food = 0;
int curr = 0;

// Function to disable buffered input and echo
void enableRawMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Function to reset terminal settings
void disableRawMode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Clear the screen using ANSI escape code
void clearScreen() {
    printf("\033[2J\033[1;1H");
}

void initialize() {
    srand(time(NULL));  // Seed the RNG

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || j == 0 || i == HEIGHT - 1 || j == WIDTH - 1)
                board[i][j] = WALL;
            else
                board[i][j] = EMPTY;
        }
    }

    int count = 50;
    while (count--) {
        int i = rand() % HEIGHT;
        int j = rand() % WIDTH;
        if (board[i][j] == EMPTY)
            board[i][j] = WALL;
    }

    int val = 5;
    while (val--) {
        int row = rand() % HEIGHT;
        for (int j = 3; j < WIDTH - 3; j++) {
            if (board[row][j] == EMPTY)
                board[row][j] = WALL;
        }
    }

    count = 10;
    while (count--) {
        int i = rand() % HEIGHT;
        int j = rand() % WIDTH;
        if (board[i][j] == EMPTY)
            board[i][j] = DEMON;
    }

    pacman_x = WIDTH / 2;
    pacman_y = HEIGHT / 2;
    board[pacman_y][pacman_x] = PACMAN;

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i % 2 == 0 && j % 2 == 0 &&
                board[i][j] == EMPTY) {
                board[i][j] = FOOD;
                food++;
            }
        }
    }
}

void draw() {
    clearScreen();
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
}

void move_pacman(int move_x, int move_y) {
    int x = pacman_x + move_x;
    int y = pacman_y + move_y;

    if (board[y][x] != WALL) {
        if (board[y][x] == FOOD) {
            score++;
            food--;
            curr++;
            if (food == 0) {
                res = 2;
                return;
            }
        } else if (board[y][x] == DEMON) {
            res = 1;
        }

        board[pacman_y][pacman_x] = EMPTY;
        pacman_x = x;
        pacman_y = y;
        board[pacman_y][pacman_x] = PACMAN;
    }
}

int main() {
    enableRawMode();
    atexit(disableRawMode); // Reset terminal when program exits

    initialize();
    char ch;
    food -= 35;
    int totalFood = food;

    printf("Use W (up), A (left), S (down), D (right)\n");
    printf("Press Q to quit\nPress Y to start: ");
    ch = getchar();
    if (ch != 'Y' && ch != 'y') {
        printf("\nExit Game!\n");
        return 0;
    }

    while (1) {
        draw();
        printf("Total Food: %d, Eaten: %d\n", totalFood, curr);

        if (res == 1) {
            clearScreen();
            printf("Game Over! You were killed by a Demon.\nScore: %d\n", score);
            break;
        } else if (res == 2) {
            clearScreen();
            printf("You Win! Final Score: %d\n", score);
            break;
        }

        ch = getchar();

        switch (ch) {
            case 'w':
            case 'W':
                move_pacman(0, -1);
                break;
            case 's':
            case 'S':
                move_pacman(0, 1);
                break;
            case 'a':
            case 'A':
                move_pacman(-1, 0);
                break;
            case 'd':
            case 'D':
                move_pacman(1, 0);
                break;
            case 'q':
            case 'Q':
                printf("Game Over! Score: %d\n", score);
                return 0;
        }

        usleep(100000);  // Add a small delay to slow down the loop
    }

    return 0;
}
