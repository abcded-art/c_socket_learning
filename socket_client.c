#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){
    /** ====================================================
    1. Create Socket
    ===================================================== */

    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    /** ====================================================
    2. Connect
    ===================================================== */

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = inet_addr("3.39.100.211");

    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    /** ====================================================
    3. Receive Data from the Server
    ===================================================== */

    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);

    printf("The server sent the data %s\n", server_response);

    /** ====================================================
    4. Close
    ===================================================== */

    close(network_socket);

    return 0;
}