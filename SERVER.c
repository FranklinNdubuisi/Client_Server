#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int isPrime(int num) {
    if (num <= 1) return 0;
    if (num == 2) return 1;
    if (num % 2 == 0) return 0;
    int i;
    for (i = 3; i * i <= num; i += 2) {
        if (num % i == 0) return 0;
    }
    return 1; //logic to find the prime numbers.
}

int main() {
    int fd;
    char* myfifo = "/tmp/myfifo"; //pipe name.
    mkfifo(myfifo, 0666); //pipe creation

    char clientMessage[2048], serverResponse[2048]; //length of buffers
    int start = 0, end = 0, pos, num;
    int foundRange;

    while (1) {
        fd = open(myfifo, O_RDONLY); //opens a read only file
        read(fd, clientMessage, 2048);
        close(fd);

        //checks for the accuracy of the user(client) ensures that the first 5 characters spell out the word range in upper or lower case.
        foundRange = 0;
        if ((clientMessage[0] == 'R' || clientMessage[0] == 'r') &&
            (clientMessage[1] == 'A' || clientMessage[1] == 'a') &&
            (clientMessage[2] == 'N' || clientMessage[2] == 'n') &&
            (clientMessage[3] == 'G' || clientMessage[3] == 'g') &&
            (clientMessage[4] == 'E' || clientMessage[4] == 'e') && clientMessage[5] == ' ') { 
            pos = 6;
            start = 0;
           //extracting the integers for the range (the start and the end)
            while (clientMessage[pos] >= '0' && clientMessage[pos] <= '9') {
                start = start * 10 + (clientMessage[pos] - '0'); // receives numeric values in characters (extraction method)
                pos++;
            }
            if (clientMessage[pos] == ' ') {
                pos++;
                end = 0;
                while (clientMessage[pos] >= '0' && clientMessage[pos] <= '9') {
                    end = end * 10 + (clientMessage[pos] - '0'); // receives numeric values in characters (extraction method)
                    pos++;
                }
                foundRange = 1;
            }
        }

        // Generate server response based on client request
        int len = 0;
        if (foundRange && start <= end) {
            strcpy(serverResponse, "Primes between ");
            len = strlen(serverResponse);

            len += sprintf(serverResponse + len, "%d and %d: ", start, end);

            int firstPrimeFound = 0;
            for (num = start; num <= end; num++) {
                if (isPrime(num)) {
                    if (firstPrimeFound) serverResponse[len++] = ' ';
                    len += sprintf(serverResponse + len, "%d", num);
                    firstPrimeFound = 1;
                }
            }
            if (!firstPrimeFound) len += sprintf(serverResponse + len, "None found");
            serverResponse[len++] = '\n';
            serverResponse[len] = '\0';
        }
        else {
            strcpy(serverResponse, "malformed request\n");
        }

        // Write server response to FIFO
        fd = open(myfifo, O_WRONLY);
        write(fd, serverResponse, strlen(serverResponse) + 1);
        close(fd);
    }
    return 0;
}
