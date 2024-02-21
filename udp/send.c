#include "send.h"

#define BEG_OFFSET (sizeof(MessageType) + sizeof(MessageID))
#define STR_OFFSET sizeof(uint8_t)

int send_AUTH(int sockfd, struct sockaddr_in *server, char *username, char *display_name, char *password, MessageID msg_id)
{
    size_t data_len = BEG_OFFSET + strlen(username) + STR_OFFSET + strlen(display_name) + STR_OFFSET + strlen(password) + STR_OFFSET;
    char *data = calloc(data_len, 1);
    memset(data, AUTH, sizeof(MessageType));
    memcpy(data + sizeof(MessageType), &msg_id, sizeof(MessageID));
    strcpy(data + BEG_OFFSET, username);
    strcpy(data + BEG_OFFSET + strlen(username) + STR_OFFSET, display_name);
    strcpy(data + BEG_OFFSET + strlen(username) + STR_OFFSET + strlen(display_name) + STR_OFFSET, password);

    ssize_t bytestx = sendto(sockfd, data, data_len, 0, (struct sockaddr *)server, sizeof(*server));
    return bytestx;
}

int send_JOIN(int sockfd, struct sockaddr_in *server, char *channel_id, char *display_name, MessageID msg_id)
{
    size_t data_len = BEG_OFFSET + strlen(channel_id) + STR_OFFSET + strlen(display_name) + STR_OFFSET;
    char *data = calloc(data_len, 1);
    memset(data, JOIN, sizeof(MessageType));
    memcpy(data + sizeof(MessageType), &msg_id, sizeof(MessageID));
    strcpy(data + BEG_OFFSET, channel_id);
    strcpy(data + BEG_OFFSET + strlen(channel_id) + STR_OFFSET, display_name);

    ssize_t bytestx = sendto(sockfd, data, data_len, 0, (struct sockaddr *)server, sizeof(*server));
    return bytestx;
}