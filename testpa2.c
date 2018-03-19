#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 1024

int main(void)
{
    int returnValue, device, stringLength;
    char *receive, *stringToSend;
    int c = 0;

    receive      = malloc(sizeof(char) * BUFFER_LENGTH);
    stringToSend = malloc(sizeof(char) * BUFFER_LENGTH);

    printf("TESTPA2: Starting device test code example.\n");

    device = open("/dev/pa2", O_RDWR);

    if (device < 0)
    {
        perror("TESTPA2: Failed to open the device.\n");

        return errno;
    }

    printf("TESTPA2: Type in a short string to send to the kernel module:\n");

    // scanf("%1024[^\n]%*c", stringToSend);
    fgets(stringToSend, BUFFER_LENGTH, stdin);

    strtok(stringToSend, "\n");

    while ((c = getchar()) != '\n' && c != EOF);

    printf("TESTPA2: Writing message to the device [%s].\n", stringToSend);

    stringLength = strlen(stringToSend) >= BUFFER_LENGTH ? BUFFER_LENGTH : strlen(stringToSend);
    returnValue = write(device, stringToSend, stringLength);

    if (returnValue < 0)
    {
        perror("TESTPA2: Failed to write the message to the device.\n");

        return errno;
    }

    printf("TESTPA2: Press ENTER to read back from the device.\n");
    getchar();

    printf("TESTPA2: Reading from the device.\n");

    returnValue = read(device, receive, BUFFER_LENGTH);

    if (returnValue < 0)
    {
        perror("TESTPA2: Failed to read the message from the device.\n");

        return errno;
    }

    printf("TESTPA2: The received message is: [%s].\n", receive);
    printf("TESTPA2: End of the device test code example.\n");

    free(receive);
    free(stringToSend);

    return 0;
}
