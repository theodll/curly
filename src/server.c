#include <stdio.h>      // printf, perror
#include <stdlib.h>     // exit, malloc/free etc.
#include <string.h>     // strlen, memset
#include <unistd.h>     // close(), read(), write()
#include <sys/socket.h> // socket(), connect()
#include <netdb.h>      // getaddrinfo(), struct addrinfo

int main() {
    // Der Hostname, den wir anfragen möchten
    const char *host = "example.com";

    // Der Port für HTTP (nicht HTTPS)
    const char *port = "80";

    // Struktur für die Parameter, die wir für getaddrinfo() setzen wollen
    struct addrinfo hints = {0}; 
    struct addrinfo *res; 

    // Wir wollen eine IPv4-Adresse (AF_INET)
    hints.ai_family = AF_INET;

    // Wir wollen einen TCP-Stream-Socket (SOCK_STREAM)
    hints.ai_socktype = SOCK_STREAM;

    // getaddrinfo: löst Host + Port in eine nutzbare Socket-Adresse auf
    // Beispiel: "example.com" -> IP-Adresse
    if (getaddrinfo(host, port, &hints, &res) != 0) {
        perror("getaddrinfo"); 
        return 1;
    }

    // Einen Socket erzeugen
    // res->ai_family = AF_INET
    // res->ai_socktype = SOCK_STREAM
    // res->ai_protocol = meist IPPROTO_TCP
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // Mit dem Server verbinden
    // connect() baut eine TCP-Verbindung zu host:port auf
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        return 1;
    }

    // HTTP GET Request bauen
    // Wichtig: \r\n sind Pflicht im HTTP-Protokoll
    char request[512];
    snprintf(request, sizeof(request),
             "GET / HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: pure-c-client\r\n"
             "Connection: close\r\n"
             "\r\n",
             host);

    // Request an den Server senden
    // write() sendet die rohen Bytes in den Socket
    write(sock, request, strlen(request));

    // Buffer zum Lesen der Antwort
    char buffer[1024];
    ssize_t bytes;

    // Die Antwort des Servers lesen
    // Solange der Server Daten schickt, gibt read() > 0 zurück
    while ((bytes = read(sock, buffer, sizeof(buffer)-1)) > 0) {
        buffer[bytes] = '\0'; // String terminieren, damit printf funktioniert
        printf("%s", buffer);
    }

    // Socket schließen
    close(sock);

    // Gedächtnis freigeben, das getaddrinfo() vergeben hat
    freeaddrinfo(res);

    return 0;
}
