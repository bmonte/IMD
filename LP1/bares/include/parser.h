#ifndef _PARSER_H_
#define _PARSER_H_

#include <iostream> // cout, cin
#include <iterator> // std::distance()
#include <vector>   // std::vector
#include <sstream>  // std::istringstream
#include <cstddef>  // std::ptrdiff_t
#include <limits>   // std::numeric_limits, para validar a faixa de um inteiro.
#include <algorithm>// std::copy, para copiar substrings.
#include <cctype>   // std::isspace()

#include "token.h"  // struct Token.

/*
used EBNF grammar:
    <expression>       :=   <term>, { ( "+" | "-" | "*" | "/" | "%" | "^" ), <term> };
    <term>             :=   "(", <expression>, ")" | <integer>;
    <integer>          :=   0 | [ "-" ], <natural_number>;
    <natural_number>   :=   <digit_excl_zero>, {<digit>};
    <digit_excl_zero>  :=   "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";
    <digit>            :=   "0" | <digit_excl_zero>;
*/

class Parser
{
    public:
        /// This struct represents the result of the parsing operation.
        struct ResultType
        {
            //=== Alias
            typedef std::ptrdiff_t size_type; //!< Used for column location determination.

            /// List of possible syntax errors.
            enum code_t {
                    OK = 0, //!< Expression successfuly parsed.
                    UNEXPECTED_END_OF_EXPRESSION,
                    ILL_FORMED_INTEGER,
                    MISSING_TERM,
                    EXTRANEOUS_SYMBOL,
                    INTEGER_OUT_OF_RANGE,
                    MISSING_CLOSE_SCOPE,
                    DIVISION_BY_ZERO,
                    NUMERIC_OVERFLOW
            };

            //=== Members (public).
            code_t type;      //!< Error code.
            size_type at_col; //!< Stores the column number where the error happened.

            /// Default contructor.
            explicit ResultType( code_t type_=OK , size_type col_=0u )
                    : type{ type_ }
                    , at_col{ col_ }
            { /* empty */ }
        };

        //==== Aliases
        typedef short int required_int_type; //!< The interger type we accept as valid for an expression.
        typedef long long int input_int_type; //!< The integer type that we read from the input, which should be larger than  he required integer range (so we can identify input errors).

        //==== Public interface
        /// Parses and tokenizes an input source expression.  Return the result as a struct.
        ResultType parse( std::string e_ );
        /// Retrieves the list of tokens created during the partins process.
        std::vector< Token > get_tokens( void ) const;

        //==== Special methods
        /// Default constructor
        Parser() = default;
        /// Default destructor
        ~Parser() = default;
        /// Turn off copy constructor. We do not need it.
        Parser( const Parser & ) = delete;  // Construtor cópia.
        /// Turn off assignment operator.
        Parser & operator=( const Parser & ) = delete; // Atribuição.

    private:
        /// Terminal symbols table
        enum class terminal_symbol_t{  // The symbols:-
            TS_PLUS,	        //!< code for "+"
            TS_MINUS,	        //!< code for "-"
            TS_MULT,          //!< code for "*"
            TS_DIV,           //!< code for "/"
            TS_POW,           //!< code for "^"
            TS_MOD,           //!< code for "%"
            TS_OS,            //!< code for "("
            TS_CS,            //!< code for ")"
            TS_ZERO,            //!< code for "0"
            TS_NON_ZERO_DIGIT,  //!< code for digits, from "1" to "9"
            TS_WS,              //!< code for a white-space
            TS_TAB,             //!< code for tab
            TS_EOS,             //!< code for "End Of String"
            TS_INVALID	        //!< invalid token
        };

        //==== Private members.
        std::string m_expr;                //!< The source expression to be parsed
        std::string::iterator m_it_curr_symb; //!< Pointer to the current char inside the expression.
        std::vector< Token > m_tk_list; //!< Resulting list of tokens extracted from the expression.
        ResultType m_result; //!< The result for the current expression (either error of OK).

        //=== Support methods.
        terminal_symbol_t lexer( char c_ ) const;// Get the corresponding code for a given input char.
        void next_symbol( void );                // Advances iterator to the next char in the expression.
        //bool peek( terminal_symbol_t c_ ) const; // Peeks the current character.
        bool accept( terminal_symbol_t c_ );     // Tries to accept the requested symbol.
        //bool expect( terminal_symbol_t c_ );        // Skips any WS/Tab and tries to accept the requested symbol.
        void skip_ws( void );                    // Skips any WS/Tab ans stops at the next character.
        bool end_input( void ) const;            // Checks whether we reached the end of the expression string.

        //=== NTS methods.
        bool expression();
        bool term();
        bool integer();
        bool natural_number();
        bool digit_excl_zero();
        bool digit();
};

#endif
