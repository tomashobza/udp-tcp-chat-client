#ifndef MESSAGES_H
#define MESSAGES_H

#include "types.h"

// TODO: add comments

class MessageHelper
{
private:
    // MessageType type;
    // MessageID id;
    // std::vector<uint8_t> data;

public:
    // Message(std::vector<uint8_t> data);
    // ~Message();

    // MessageType get_type();
    // MessageID get_id();
    // std::vector<uint8_t> get_data();

    static ConfirmMessage to_confirm(std::vector<uint8_t> data);
    static ReplyMessage to_reply(std::vector<uint8_t> data);
    static AuthMessage to_auth(std::vector<uint8_t> data);
    static JoinMessage to_join(std::vector<uint8_t> data);
    static MsgMessage to_msg(std::vector<uint8_t> data);
    static ErrMessage to_err(std::vector<uint8_t> data);
    static ByeMessage to_bye(std::vector<uint8_t> data);
};

#endif // MESSAGES_H