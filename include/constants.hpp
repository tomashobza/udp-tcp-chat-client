/**
 * @file constants.hpp
 * @author Tomáš Hobza (xhobza03)
 * @brief Constants for the project
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/** Byte offset for each UDP message */
#define BEG_OFFSET (sizeof(MessageType) + sizeof(MessageID))
/** NULL termination byte offset */
#define STR_OFFSET sizeof(uint8_t)

#endif // CONSTANTS_H