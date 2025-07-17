#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8181

void clear_memory(struct sockaddr_in *srv, struct sockaddr_in *cli);
int socket_setup(SOCKET *s);
void server_setup(struct sockaddr_in *srv);
int bind_socket(struct sockaddr_in *srv, SOCKET *s);
int listen_to_connections(SOCKET *s);
int accept_clients(struct sockaddr_in *cli, SOCKET *s, SOCKET *c);
int respond_to_client(char *buffer, char *data, SOCKET *s, SOCKET *c, int end, int ds);
int read_html(char *buffer, int size);

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup()\n");
        return -1;
    }

    SOCKET s, c;

    struct sockaddr_in srv, cli;
    char buff[512];
    char res[512];

    clear_memory(&srv, &cli);

    // Socket setup
    if (socket_setup(&s))
        return -1;

    // Server setup
    server_setup(&srv);

    // Binding the socket to the address/port
    if (bind_socket(&srv, &s))
        return -1;

    // Listening for incoming connections (max 5 queued).
    if (listen_to_connections(&s))
        return -1;

    // Accepting clients
    if (accept_clients(&cli, &s, &c))
        return -1;

    int rec = recv(c, buff, sizeof(buff) - 1, 0);
    if (rec > 0 && respond_to_client(buff, res, &s, &c, rec, sizeof(res)))
        return -1;

    shutdown(c, SD_SEND);
    closesocket(c);
    shutdown(s, SD_SEND);
    closesocket(s);
    WSACleanup();

    return 0;
}

void clear_memory(struct sockaddr_in *srv, struct sockaddr_in *cli)
{

    memset(srv, 0, sizeof(*srv));
    memset(cli, 0, sizeof(*cli));
}

int socket_setup(SOCKET *s)
{
    *s = socket(/* IPv4 */ AF_INET, /* TCP */ SOCK_STREAM, /* Default protocol */ 0);
    if (*s == INVALID_SOCKET)
    {
        printf("socket failed at socket_setup()\n");
        return 1;
    }
    return 0;
}

void server_setup(struct sockaddr_in *srv)
{
    srv->sin_family = AF_INET;         // IPv4
    srv->sin_addr.s_addr = INADDR_ANY; // listen on all interfaces, so anyone who can reach this machine can connect
    srv->sin_port = htons(PORT);       // converts host byte order to network byte order.
}

int bind_socket(struct sockaddr_in *srv, SOCKET *s)
{
    if (bind(*s, (struct sockaddr *)srv, sizeof(*srv)))
    {
        printf("binding failed at bind_socket()\n");
        closesocket(*s);
        return 1;
    }
    return 0;
}

int listen_to_connections(SOCKET *s)
{
    if (listen(*s, 5))
    {
        printf("listen failed at listen_to_connections()\n");
        closesocket(*s);
        return 1;
    }

    printf("Listening on http://localhost:%d\n", PORT);
    return 0;
}

int accept_clients(struct sockaddr_in *cli, SOCKET *s, SOCKET *c)
{
    int addrlen = sizeof(*cli);

    *c = accept(*s, (struct sockaddr *)cli, &addrlen); // accept waits for an incoming connection

    if (*c == INVALID_SOCKET)
    {
        printf("accept()\n");
        closesocket(*s);
        return 1;
    }

    printf("Client connected!\n");
    return 0;
}

int respond_to_client(char *buffer, char *data, SOCKET *s, SOCKET *c, int end, int ds)
{
    buffer[end] = '\0';
    printf("Received from client: %s\n", buffer);

    int f = read_html(data, ds);

    if (f)
    {
        printf("failed to read the html file.");
        closesocket(*s);
        return 1;
    }

    char response[1024];

    int len = snprintf(response, sizeof(response),
                       "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/html\r\n"
                       "Content-Length: %zu\r\n"
                       "Connection: close\r\n"
                       "\r\n"
                       "%s",
                       strlen(data), data);

    send(*c, response, len, 0);
    return 0;
}

int read_html(char *buffer, int size)
{
    FILE *file = fopen("./index.html", "r");

    if (file == NULL)
    {
        printf("Error opening file!");
        return 1;
    }

    int read = fread(buffer, 1, size - 1, file);
    buffer[read] = '\0';
    fclose(file);
    return 0;
}
