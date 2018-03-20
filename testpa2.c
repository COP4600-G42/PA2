#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 1024

int main(void)
{
    char inputChoice, c, receiveLength;
    int device, stringLength, returnValue;
    char *stringToSend, *stringToReceive, *stringToReceiveLength;
    char *ptr;

    stringToSend    = malloc(sizeof(char) * BUFFER_LENGTH);
    stringToReceive = malloc(sizeof(char) * BUFFER_LENGTH);
    stringToReceiveLength = malloc(sizeof(char) * BUFFER_LENGTH);

    device = open("/dev/pa2", O_RDWR);

    if (device < 0)
    {
        perror("TESTPA2: Failed to open the device.\n");

        return errno;
    }

    printf("TESTPA2: Type W to write.\n");
    printf("TESTPA2: Type R to read.\n");
    printf("TESTPA2: Type Q to quit.\n");

    do {
        printf("TESTPA2: What do you want to do? ");
        inputChoice = getchar();

        // Clear the stdin buffer
        while ((c = getchar()) != '\n' && c != EOF);

        switch (inputChoice)
        {
            case 'w':
            case 'W':
                printf("TESTPA2: Enter a string to write:\n");
                printf("TESTPA2: ");
                fgets(stringToSend, BUFFER_LENGTH, stdin);
                strtok(stringToSend, "\n");
                stringLength = strlen(stringToSend) >= BUFFER_LENGTH ? BUFFER_LENGTH : strlen(stringToSend);
                returnValue = write(device, stringToSend, stringLength);
                break;
            case 'r':
            case 'R':
                printf("TESTPA2: How many bytes would you like to read?\n");
                printf("TESTPA2: ");
                fgets(stringToReceiveLength, 4, stdin);
                receiveLength = atoi(stringToReceiveLength);
                returnValue = read(device, stringToReceive, receiveLength);
                printf("TESTPA2: %s\n", stringToReceive);
                strcpy(stringToReceive, "");
                break;
            case 'q':
            case 'Q':
                printf("TESTPA2: Bye-bye.\n");
                break;
            default:
                printf("TESTPA2: Invalid input option.\n");
                break;
        }
    } while (inputChoice != 'Q' && inputChoice != 'q');

    free(stringToSend);
    free(stringToReceive);
    free(stringToReceiveLength);

    return 0;
}
