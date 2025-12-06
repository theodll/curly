#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

typedef struct
{
    struct addrinfo hints;
    struct addrinfo *res;

    const char *hostname;
    const char *port;
    int sock;

} connectionData;

typedef struct
{
    const char* fileName;
    const char* relativeFilePath;
    char* buffer[]; 

} wFileData; 

void http_get(connectionData *data, char* buffer[]);
void write_buffer_to_file(wFileData *data);

int main(int argc, const char **argv)
{
    connectionData data;

    data.hostname = "google.com";     
    data.port = "80";

    memset(&data.hints, 0, sizeof(data.hints));
    data.hints.ai_family   = AF_INET;
    data.hints.ai_socktype = SOCK_STREAM;


    char buffer[8192]; 

    http_get(&data, &buffer);

    return 0;
}


void http_get(connectionData *data, char* buffer[])
{
    if (getaddrinfo(data->hostname, data->port, &data->hints, &data->res) != 0)
    {
        perror("getaddrinfo");
        exit(1);
    }

    data->sock = socket(data->res->ai_family, data->res->ai_socktype, data->res->ai_protocol);
    if (data->sock < 0)
    {
        perror("socket");
        exit(1);
    }

    if (connect(data->sock, data->res->ai_addr, data->res->ai_addrlen) < 0)
    {
        perror("connect");
        exit(1);
    }

    char request[512];
    snprintf(
        request,
        sizeof(request),
        "GET / HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: curly\r\n"
        "Connection: close\r\n"
        "\r\n",
        data->hostname
    );

    write(data->sock, request, strlen(request));

    
    ssize_t bytes;

    while ((bytes = read(data->sock, *buffer, sizeof(*buffer)-1)) > 0)
    {
        buffer[bytes] = '\0';
        printf("%s", *buffer);
    }

    close(data->sock);
    freeaddrinfo(data->res);
}
