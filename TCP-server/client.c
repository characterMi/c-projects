#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup()\n");
        return -1;
    }

    SOCKET s;
    struct sockaddr_in srv;
    char buffer[512];

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
    {
        printf("socket()\n");
        return -1;
    }

    srv.sin_family = AF_INET;
    srv.sin_port = htons(8181);
    inet_pton(AF_INET, "127.0.0.1", &srv.sin_addr); // Converting IP address string "127.0.0.1" into binary and storeing it in srv.sin_addr

    if (connect(s, (struct sockaddr *)&srv, sizeof(srv)) == SOCKET_ERROR)
    {
        printf("connect()\n");
        closesocket(s);
        return -1;
    }

    printf("Connected to server!\n");

    send(s, "hello from client!\n", 6, 0);
    int rec = recv(s, buffer, sizeof(buffer) - 1, 0);
    if (rec > 0)
    {
        buffer[rec] = '\0';
        printf("Received from server: %s\n", buffer);
    }

    closesocket(s);
    WSACleanup();
    return 0;
}
