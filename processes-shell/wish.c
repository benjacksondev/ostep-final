#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SHELL_NAME "wish"
#define LOGGING_ENABLED 0


void log_it(char *key, char *str) {
    if (LOGGING_ENABLED) {
        printf("%s: %s\n", key, str); 
    }
}

void remove_new_line(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

char*** parse_input(char *input) {
    char*** tokens = (char ***)malloc(10 * sizeof(char **));
    if (tokens == NULL) {
        perror("malloc");
        exit(1);
    }

    char *delimiter = " ";
    char *logicalAnd = "&&";
    int line_count = 0;
    char *line;
    char *tmp = input;    

    while ((line = strsep(&tmp, logicalAnd)) != NULL) {
        tokens[line_count] = (char **)malloc(10 * sizeof(char *));
        if (tokens[line_count] == NULL) {
            perror("malloc");
            exit(1);
        }
        int token_count = 0;
        char *token;

        while ((token = strsep(&line, delimiter)) != NULL) {
            remove_new_line(token);
            if (strcmp(token, "") == 0) {
                continue; 
            }
            tokens[line_count][token_count] = token;
            if (tokens[line_count][token_count] == NULL) {
                perror("malloc");
                exit(1);
            }
            token_count++;
        }
        
        tokens[line_count][token_count] = NULL;
       
        if (tokens[line_count][0] != NULL) {
            line_count++;
        } 
    }

    tokens[line_count] = NULL;
    return tokens;
}

int main(int argc, char *argv[]) {
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    printf("%s>", SHELL_NAME);
    while ((linelen = getline(&line, &linecap, stdin)) > 0) {
        char ***tokens = parse_input(line);
        pid_t child_pid;
        int status;

        int i = 0; 
        for (i = 0; tokens[i] != NULL; i++) {
            if (strcmp(tokens[i][0], "exit") == 0) {
                printf("Goodbye\n");
                free(line);
                exit(0);
            }

            if (strcmp(tokens[i][0], "clear") == 0) { 
                system("clear"); // Clear the terminal screen
            } else {
                child_pid = fork();
                if (child_pid == 0) {
                    execvp(tokens[i][0], tokens[i]); // Use parsed command and arguments
                    perror("execvp");
                    free(tokens[i]);
                    exit(1);
                } else {
                    wait(&status);
                }
            }

            free(tokens[i]);
        }
        free(tokens);
         
        printf("%s>", SHELL_NAME);
    }

    free(line); // Free dynamically allocated memory
    return 0;
}

