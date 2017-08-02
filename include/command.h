/*
    File:    command.h
    Created: 13 December 2015 at 09:05 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <cstddef>
/* This file defines the commands, in which are compiled regular expressions. */
enum class Command_name{
    Or,                    Concat,  Kleene,     Positive,
    Optional,              Char,    Char_class, Unknown,
    Char_class_complement, Multior, Multiconcat
};

struct Command{
    size_t action_name; /* The index of the identifier that is the name of
                         * the action, in the prefix tree of identifiers. */
    Command_name name;
    union{
        struct {
            size_t first, second;
        } args;
        char32_t   c;
        size_t     idx_of_set;
    };
};

// enum Command_name {
//     Cmd_or,             Cmd_concat,   Cmd_Kleene,
//     Cmd_positive,       Cmd_optional, Cmd_char_def,
//     Cmd_char_class_def, Cmd_unknown,  Cmd_multior,
//     Cmd_multiconcat
// };
//
// enum Char_class {
//     C_Latin,    C_Letter,       C_Russian,
//     C_bdigits,  C_digits,       C_latin,
//     C_letter,   C_odigits,      C_russian,
//     C_xdigits,  C_ndq,          C_nsq
// };
//
// struct Command {
//     size_t action_name; /* The index of the identifier that is the name of
//                          * the action, in the prefix tree of identifiers. */
//     Command_name name;
//     union{
//         struct {
//             size_t first, second;
//         } args;
//         char32_t   c;
//         Char_class cls;
//     };
// };

using Command_buffer = std::vector<Command>;
#endif