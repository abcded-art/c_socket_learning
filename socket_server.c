#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(){
    char server_message[256] = "You have reached the server.";

    /** ====================================================
    1. Socket: Create socket
    ===================================================== */
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);


    /** ====================================================
    2. Bind: The bind function asks the kernel to associate the server's socket address in addr with the socket descriptor sockfd.
    ===================================================== */

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    /** ====================================================
    3. Listen: The listen function converts sockfd from an active socket to a listening socket that cat accept connection requests from clients.
    - The backlog argument is a hint about the number of outstanding connection requests that the kernal should queue up before it starts to refuse requests.
    ===================================================== */

    listen(server_socket, 5);
    

    /** ====================================================
    4. Accept: Server wait for connection requests from client by calling the accept function.

    int accept(int listenfd, struct sockaddr *addr, int *addrlen);
    ===================================================== */

    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);

    /** ====================================================
    5. Send
    ===================================================== */

    send(client_socket, server_message, sizeof(server_message), 0);

    /** ====================================================
    6. Close
    ===================================================== */

    close(client_socket);
    close(server_socket);

    return 0;
}