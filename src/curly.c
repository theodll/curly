#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>    
#include <sys/socket.h> 
#include <netdb.h>  

#define PORT 8080


typedef struct 
{

    struct addrinfo hints; 
    struct addrinfo *res;

    const char* hostname;
    const char* port;
    int sock;

} connectionData;

char* get(connectionData* data);

int main(int argc, const char** argv[]) 
{

    if (argc < 2) 
    {
        perror("Please Specify a hostname");
        printf("Usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    connectionData *data;
    data->port = "80";
    data->hostname = argv[1];
    data->sock = socket(data->res->ai_family, data->res->ai_socktype, data->res->ai_protocol);
    data->hints.ai_family = AF_INET;
    data->hints.ai_socktype = SOCK_STREAM;


    char* content = get(data); 


    close(data->sock);

    freeaddrinfo(data->res);

    return 1;
};


char* get(connectionData* data)
{

    if (getaddrinfo(data->hostname, data->port, &data->hints, &data->res) != 0)
    {
        perror("getaddrinfo");
        return NULL;
    }

    if (data->sock < 0)
    {
        perror("socket");
        return NULL;
    }

    if (connect(data->sock, data->res->ai_addr, data->res->ai_addrlen) < 0)
    {
        perror("connect");
        return NULL;
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

    char buffer[8192];
    ssize_t bytes;

    while ((bytes = read(data->sock, buffer, sizeof(buffer)-1)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    return buffer;

}