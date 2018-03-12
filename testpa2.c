#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 1024

/* GLOBAL VARIABLES */
static char receive[BUFFER_LENGTH];

int main(void)
{
    int ret, fd;
    char stringToSend[BUFFER_LENGTH];

    printf("TESTPA2: Starting device test code example.\n");

    fd = open("/dev/pa2", O_RDWR);

    if (fd < 0)
    {
        perror("TESTPA2: Failed to open the device.\n");

        return errno;
    }

    printf("TEST PA2: Type in a short string to send to the kernel module:\n");
    scanf("%[^\n]%*c", stringToSend);

    printf("TESTPA2: Writing message to the device [%s].\n", stringToSend);

    ret = write(fd, stringToSend, strlen(stringToSend));

    if (ret < 0)
    {
        perror("TESTPA2: Failed to write the message to the device.\n");

        return errno;
    }

    printf("TESTPA2: Press ENTER to read back from the device.\n");
    getchar();

    printf("TESTPA2: Reading from the device.\n");

    ret = read(fd, receive, BUFFER_LENGTH);

    if (ret < 0)
    {
        perror("TESTPA2: Failed to read the message from the device.\n");

        return errno;
    }

    printf("TESTPA2: The received message is: [%s].\n", receive);
    printf("TESTPA2: End of the device test code example.\n");

    return 0;
}
