#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SHELL_NAME "wish"

void remove_new_line(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

char **parse_input(char *input) {
    char *token;
    char **tokens = (char **)malloc(100 * sizeof(char *));
    if (tokens == NULL) {
        perror("malloc");
        exit(1);
    }

    char *delimiter = " ";
    int token_count = 0;

    token = strsep(&input, delimiter);
    while (token != NULL) {
        remove_new_line(token);
        tokens[token_count] = token;
        token = strsep(&input, delimiter);
        token_count++;
    }
    tokens[token_count] = NULL; // Null-terminate the array.

    return tokens;
}

int main(int argc, char *argv[]) {
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    printf("Hello, welcome to %s\n", SHELL_NAME);
    printf("%s>", SHELL_NAME);
    while ((linelen = getline(&line, &linecap, stdin)) > 0) {
        char **tokens = parse_input(line);
        pid_t child_pid;
        int status;

        if (strcmp(tokens[0], "exit") == 0) {
            printf("Goodbye\n");
            free(tokens); // Free dynamically allocated memory
            free(line);
            exit(0);
        }

        if (strcmp(tokens[0], "clear") == 0) {
            system("clear"); // Clear the terminal screen
        } else {
            child_pid = fork();
            if (child_pid == 0) {
                execvp(tokens[0], tokens); // Use parsed command and arguments
                perror("execvp");
                exit(1);
            } else {
                wait(&status);
            }
        }

        free(tokens); // Free dynamically allocated memory
        printf("%s>", SHELL_NAME);
    }

    free(line); // Free dynamically allocated memory
    return 0;
}

