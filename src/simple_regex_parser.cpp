/*
    File:    simple_regex_parser.cpp
    Created: 14 December 2015 at 15:25 Moscow time
    Author:  Гаврилов Владимир Сергеевич
    E-mails: vladimir.s.gavrilov@gmail.com
             gavrilov.vladimir.s@mail.ru
             gavvs1977@yandex.ru
*/

#include "../include/command.h"
#include "../include/simple_regex_parser.h"
#include "../include/expr_scaner.h"
#include "../include/belongs.h"
#include <cstdio>

/* If the lexemes 'symbol', 'symbol class' and 'symbol class complement' are denoted
 * by a, the token '|' denote by b, and the lexemes with codes Begin_expression and
 * End_expression are c and d respectively, then the description of the identifier's
 * beginning and description of the identifier body can be written in the form of the
 * regular expression
 *                              caa*(baa*)*d.
 * After constructing a nondeterministic finite state machine (NFA) for this regular
 * expression, then from the obtained NFA the corresponding deterministic finite
 * automaton (DFA), after which, minimizing the constructed DFA, we obtain the
 * following transition table:
 *
 * -------------------------------------------------------------------------------------
 * |   | State_begin_expr   | State_begin_concat | State_concat       | State_end_expr |
 * -------------------------------------------------------------------------------------
 * | a |                    | State_concat       | State_concat       |                |
 * -------------------------------------------------------------------------------------
 * | b |                    |                    | State_begin_concat |                |
 * -------------------------------------------------------------------------------------
 * | c | State_begin_concat |                    |                    |                |
 * -------------------------------------------------------------------------------------
 * | d |                    |                    | State_end_expr     |                |
 * -------------------------------------------------------------------------------------
 * In this table, column names are state names, and State_begin_expr is the initial
 * state, and State_end_expr is the end state. */

// static const unsigned long long set_of_first_lexems_of_expr =
//     1ULL << Character     | 1ULL << Class_Latin   |
//     1ULL << Class_Letter  | 1ULL << Class_Russian |
//     1ULL << Class_bdigits | 1ULL << Class_digits  |
//     1ULL << Class_latin   | 1ULL << Class_letter  |
//     1ULL << Class_odigits | 1ULL << Class_russian |
//     1ULL << Class_xdigits;
//
// static const unsigned long long set_of_non_quotes =
//     1ULL << Class_ndq | 1ULL << Class_nsq;
//
// static const unsigned long long character_and_char_classes =
//     set_of_first_lexems_of_expr | set_of_non_quotes;

Simple_regex_parser::Proc Simple_regex_parser::state_proc[] = {
    &Simple_regex_parser::state_begin_expr_proc,
    &Simple_regex_parser::state_begin_concat_proc,
    &Simple_regex_parser::state_concat_proc,
    &Simple_regex_parser::state_end_expr_proc
};

static const char* opening_curly_brace_is_omitted =
    "Line %zu omits the opening curly brace.\n";

void Simple_regex_parser::state_begin_expr_proc(Command_buffer& buf){
    state = State_begin_concat;
    if(Expr_lexem_code::Begin_expression == elc){
        return;
    }
    printf("Line %zu omits the opening curly brace.\n",
           esc_->lexem_begin_line_number());
    et_.ec->increment_number_of_errors();
//     if(belongs(elc, character_and_char_classes)){
//         esc_->back();
//     }else if(End_expression == elc){
//         state = State_end_expr;
//     }
}

void Simple_regex_parser::state_begin_concat_proc(Command_buffer& buf){
//     state = State_concat;
//     if(belongs(elc, character_and_char_classes)){
//         write_char_or_char_class(buf);
//         first_concatenated     = buf.size() - 1;
//         number_of_concatenated = 1;
//         return;
//     }
//     printf("In line %zu, a character or character class is expected.\n",
//            esc_->lexem_begin_line_number());
//     et_.ec -> increment_number_of_errors();
//     state = (End_expression == elc) ? State_end_expr: State_begin_concat;
}

void Simple_regex_parser::state_concat_proc(Command_buffer& buf){
//     if(belongs(elc, character_and_char_classes)){
//         write_char_or_char_class(buf);
//         number_of_concatenated++;
//     }else if(Or == elc){
//         write_concatenated(buf);
//         write_or_command(buf);
//         number_of_concatenated = 0;
//         state = State_begin_concat;
//     }else if(End_expression == elc){
//         write_concatenated(buf);
//         write_or_command(buf);
//         state = State_end_expr;
//     }else{
//         printf("In line %zu, a character, a character class, or a "
//                "closing brace is expected.\n",
//                esc_->lexem_begin_line_number());
//         et_.ec -> increment_number_of_errors();
//     }
}

void Simple_regex_parser::state_end_expr_proc(Command_buffer& buf){
//     esc_->back();
}

// void Simple_regex_parser::write_char_or_char_class(Command_buffer& buf){
//     Command  command;
//     if(Character == elc){
//         command.name = Cmd_char_def;
//         command.c    = eli.c;
//         command.action_name = 0;
//     }else{
//         command.name = Cmd_char_class_def;
//         command.cls  = static_cast<Char_class>(elc - Class_Latin);
//         command.action_name = 0;
//         if(belongs(elc, set_of_non_quotes)){
//             printf("Error in line %zu: the character classes [:ndq:] and [:nsq:] "
//                    "are not allowed in the identifier definition.\n",
//                    esc_->lexem_begin_line_number());
//             et_.ec -> increment_number_of_errors();
//         }
//     }
//     buf.push_back(command);
//     return;
// }
//
// void Simple_regex_parser::write_or_command(Command_buffer& buf){
//     Command  command;
//     number_of_ors++;
//     if(1 == number_of_ors){
//         arg1 = buf.size() - 1;
//     }else{
//         arg2 = buf.size() - 1;
//         command.args.first = arg1; command.args.second = arg2;
//         command.name = Cmd_or;
//         command.action_name = 0;
//         buf.push_back(command);
//         arg1 = buf.size() - 1;
//     }
// }
//
// void Simple_regex_parser::write_concatenated(Command_buffer& buf){
//     if(number_of_concatenated > 1){
//         Command command;
//         command.args.first  = first_concatenated;
//         command.args.second = buf.size() - 1;
//         command.name        = Cmd_multiconcat;
//         command.action_name = 0;
//         buf.push_back(command);
//     }
// }

void Simple_regex_parser::compile(Command_buffer& buf){
    state              = State_begin_expr;
//     number_of_ors      = 0;
//     first_concatenated = 0;
//     number_of_concatenated = 0;
//     arg1 = arg2 = 0;
//     while((elc = (eli = esc_->current_lexem()).code)){
//         (this->*state_proc[state])(buf);
//         if(State_end_expr == state){
//             return;
//         }
//     }
//     if(state != State_end_expr){
//         printf("Unexpected end of a regular expression in line %zu.\n",
//                esc_->lexem_begin_line_number());
//         et_.ec->increment_number_of_errors();
//         if((State_concat == state) || (State_begin_concat == state)){
//             write_concatenated(buf);
//             write_or_command(buf);
//         }
//     }
//     esc_->back();
}