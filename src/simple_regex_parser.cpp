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
 * -------------------------------------------------------------------
 * |     state    | a      |      b       |      c       |     d     |
 * -------------------------------------------------------------------
 * | Begin_expr   |        |              | Begin_concat |           |
 * -------------------------------------------------------------------
 * | Begin_concat | Concat |              |              |           |
 * -------------------------------------------------------------------
 * | Concat       | Concat | Begin_concat |              | End_expr  |
 * -------------------------------------------------------------------
 * | End_expr     |        |              |              |           |
 * -------------------------------------------------------------------
 *
 * In this table names are state names, and Begin_expr is the initial
 * state, and End_expr is the end state. */

static const uint64_t char_char_class_and_compl =
    (1ULL << static_cast<uint64_t>(Expr_lexem_code::Character)) |
    (1ULL << static_cast<uint64_t>(Expr_lexem_code::Class_complement)) |
    (1ULL << static_cast<uint64_t>(Expr_lexem_code::Character_class));

Simple_regex_parser::Proc Simple_regex_parser::state_proc[] = {
    &Simple_regex_parser::state_begin_expr_proc,
    &Simple_regex_parser::state_begin_concat_proc,
    &Simple_regex_parser::state_concat_proc,
    &Simple_regex_parser::state_end_expr_proc
};

static const char* opening_curly_brace_is_omitted =
    "Error at line %zu: the opening curly brace is omited.\n";

static const char* unexpected_end_of_regexp =
    "Unexpected end of a regular expression at line %zu.\n";

static const char* expected_char_or_char_calss_or_compl =
    "Error at line %zu: a character, a character class, or a character class complement "
    "is expected.\n";

static const char* expected_char_or_char_calss_or_compl_or_cl_br =
    "Error at line %zu: a character, a character class, or a closing "
    "brace is expected.\n";

static const char* char_class_is_not_admissible =
    "Error at line %zu: the character class complement "
    "are not allowed in the identifier definition.\n";

inline uint64_t belongs(Expr_lexem_code e, uint64_t s)
{
    return belongs(static_cast<uint64_t>(e), s);
}

void Simple_regex_parser::state_begin_expr_proc(Command_buffer& buf){
    state = State::Begin_concat;
    if(Expr_lexem_code::Begin_expression == elc){
        return;
    }
    printf(opening_curly_brace_is_omitted, esc_->lexem_begin_line_number());
    et_.ec->increment_number_of_errors();
    if(belongs(elc, char_char_class_and_compl)){
        esc_->back();
    }else if(Expr_lexem_code::End_expression == elc){
        state = State::End_expr;
    }
}

void Simple_regex_parser::state_begin_concat_proc(Command_buffer& buf)
{
    state = State::Concat;
    if(belongs(elc, char_char_class_and_compl)){
        write_char_or_char_class(buf);
        first_concatenated     = buf.size() - 1;
        number_of_concatenated = 1;
        return;
    }
    printf(expected_char_or_char_calss_or_compl, esc_->lexem_begin_line_number());
    et_.ec -> increment_number_of_errors();
    state = (Expr_lexem_code::End_expression == elc) ? State::End_expr:
                                                       State::Begin_concat;
}

void Simple_regex_parser::state_concat_proc(Command_buffer& buf)
{
    if(belongs(elc, char_char_class_and_compl)){
        write_char_or_char_class(buf);
        number_of_concatenated++;
    }else if(Expr_lexem_code::Or == elc){
        write_concatenated(buf);
        write_or_command(buf);
        number_of_concatenated = 0;
        state = State::Begin_concat;
    }else if(Expr_lexem_code::End_expression == elc){
        write_concatenated(buf);
        write_or_command(buf);
        state = State::End_expr;
    }else{
        printf(expected_char_or_char_calss_or_compl_or_cl_br,
               esc_->lexem_begin_line_number());
        et_.ec -> increment_number_of_errors();
    }
}

void Simple_regex_parser::state_end_expr_proc(Command_buffer& buf)
{
    esc_->back();
}

void Simple_regex_parser::write_char_or_char_class(Command_buffer& buf)
{
    Command  command;
    command.action_name = 0;
    switch(elc){
        case Expr_lexem_code::Character:
            command.name        = Command_name::Char;
            command.c           = eli.c;
            break;
        case Expr_lexem_code::Character_class:
            command.name        = Command_name::Char_class;
            command.idx_of_set  = eli.set_of_char_index;
            break;
        case Expr_lexem_code::Class_complement:
            command.name        = Command_name::Char_class_complement;
            command.idx_of_set  = eli.set_of_char_index;
            printf(char_class_is_not_admissible, esc_->lexem_begin_line_number());
            et_.ec -> increment_number_of_errors();
            break;
        default:
            ;
    }
    buf.push_back(command);
    return;
}

void Simple_regex_parser::write_or_command(Command_buffer& buf)
{
    Command  command;
    number_of_ors++;
    if(1 == number_of_ors){
        arg1 = buf.size() - 1;
    }else{
        arg2 = buf.size() - 1;
        command.args.first  = arg1;
        command.args.second = arg2;
        command.name        = Command_name::Or;
        command.action_name = 0;
        buf.push_back(command);
        arg1 = buf.size() - 1;
    }
}

void Simple_regex_parser::write_concatenated(Command_buffer& buf)
{
    if(number_of_concatenated > 1){
        Command command;
        command.args.first  = first_concatenated;
        command.args.second = buf.size() - 1;
        command.name        = Command_name::Multiconcat;
        command.action_name = 0;
        buf.push_back(command);
    }
}

void Simple_regex_parser::compile(Command_buffer& buf)
{
    state                  = State::Begin_expr;
    number_of_ors          = 0;
    first_concatenated     = 0;
    number_of_concatenated = 0;
    arg1 = arg2 = 0;
    while((elc = (eli = esc_->current_lexem()).code) !=
        Expr_lexem_code::Nothing)
    {
        (this->*state_proc[state])(buf);
        if(State::End_expr == state){
            return;
        }
    }
    if(state != State::End_expr){
        printf(unexpected_end_of_regexp, esc_->lexem_begin_line_number());
        et_.ec->increment_number_of_errors();
        if((State::Concat == state) || (State::Begin_concat == state)){
            write_concatenated(buf);
            write_or_command(buf);
        }
    }
    esc_->back();
}