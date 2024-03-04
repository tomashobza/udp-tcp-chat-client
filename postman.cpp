// #include "postman.h"

// bool IPostman::get_reply(Message msg)
// {
//     if (msg.type == MessageType::REPLY && msg.data.size() > 4)
//     {
//         int reply = (int)msg.data.at(3);
//         if (reply == 0)
//         {
//             return false;
//         }
//         else if (reply == 1)
//         {
//             return true;
//         }
//     }

//     throw std::runtime_error("ERROR: not a reply message");
// }