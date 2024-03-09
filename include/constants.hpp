#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MSG_MAX_RETRIES 3
#define MSG_TIMEOUT 250
#define BEG_OFFSET (sizeof(MessageType) + sizeof(MessageID))
#define STR_OFFSET sizeof(uint8_t)

#endif // CONSTANTS_H