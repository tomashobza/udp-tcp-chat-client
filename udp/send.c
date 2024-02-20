#include "send.h"

int send_AUTH(int sockfd, struct sockaddr_in *server, char *username, char *display_name, char *password, MessageID msg_id)
{
    size_t data_len = sizeof(MessageType) + sizeof(MessageID) + strlen(username) + 1 + strlen(display_name) + 1 + strlen(password) + 1;
    char *data = malloc(data_len);
    memset(data, AUTH, sizeof(MessageType));
    memcpy(data + sizeof(MessageType), &msg_id, sizeof(MessageID));
    strcpy(data + sizeof(MessageType) + sizeof(MessageID), username);
    strcpy(data + sizeof(MessageType) + sizeof(MessageID) + strlen(username) + 1, display_name);
    strcpy(data + sizeof(MessageType) + sizeof(MessageID) + strlen(username) + 1 + strlen(display_name) + 1, password);

    ssize_t bytestx = sendto(sockfd, data, data_len, 0, (struct sockaddr *)server, sizeof(*server));
    return bytestx;
}