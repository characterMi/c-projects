#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8181

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup()\n");
        return -1;
    }

    int s, c;
    int addrlen;
    addrlen = 0;

    struct sockaddr_in srv, cli;
    char buffer[512];
    char *data;

    memset(&srv, 0, sizeof(srv));
    memset(&cli, 0, sizeof(cli));

    // Socket setup
    s = socket(/* IPv4 */ AF_INET, /* TCP */ SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
    {
        printf("socket()\n");
        return -1;
    }

    // Server setup
    srv.sin_family = AF_INET;         // IPv4
    srv.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces, so anyone who can reach this machine can connect
    srv.sin_port = htons(PORT);       // converts host byte order to network byte order.

    // Binding the socket to the address/port
    if (bind(s, (struct sockaddr *)&srv, sizeof(srv)))
    {
        printf("bind()\n");
        closesocket(s);
        return -1;
    }

    // Listening for incoming connections (max 5 queued).
    if (listen(s, 5))
    {
        printf("listen()\n");
        closesocket(s);
        return -1;
    }

    printf("Listening on 0.0.0.0:%d!\n", PORT);

    addrlen = sizeof(cli);
    c = accept(s, (struct sockaddr *)&cli, &addrlen); // accept waits for an incoming connection

    if (c == INVALID_SOCKET)
    {
        printf("accept()\n");
        closesocket(s);
        return -1;
    }

    printf("Client connected!\n");

    int rec = recv(c, buffer, sizeof(buffer) - 1, 0);

    if (rec > 0)
    {
        buffer[rec] = '\0';
        printf("Received from client: %s\n", buffer);
    }

    data = "http response!\n";
    send(c, data, strlen(data), 0);

    closesocket(c);
    closesocket(s);
    WSACleanup();

    return 0;
}
