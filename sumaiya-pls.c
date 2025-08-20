#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_LEN 10000
#define MAX_TOKEN_LEN 100
#define LTD_VALUE "115" // last 3 digit of my ID

typedef enum {
    TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE,
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_SEMI, TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, TOKEN_DIV,
    TOKEN_EQ, TOKEN_NE, TOKEN_LT, TOKEN_GT, TOKEN_LE, TOKEN_GE,
    TOKEN_NUMBER, TOKEN_IDENTIFIER, TOKEN_LTD,
    TOKEN_END, TOKEN_INVALID
} TokenType;

char *token_str[] = {
    "if", "else", "while", "{", "}", "(", ")", ";",
    "+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=",
    "number", "identifier", "LTD", "END", "INVALID"
};

typedef struct {
    TokenType type;
    char lexeme[MAX_TOKEN_LEN];
} Token;

char *input;
char current_char;
int pos = 0;
Token lookahead;

void nextChar() {
    current_char = input[pos++];
}

Token getNextToken() {
    while (isspace(current_char)) nextChar();

    Token token;
    token.lexeme[0] = '\0';

    if (isalpha(current_char)) {
        int i = 0;
        while (isalnum(current_char)) {
            token.lexeme[i++] = current_char;
            nextChar();
        }
        token.lexeme[i] = '\0';

        if (strcmp(token.lexeme, "if") == 0) token.type = TOKEN_IF;
        else if (strcmp(token.lexeme, "else") == 0) token.type = TOKEN_ELSE;
        else if (strcmp(token.lexeme, "while") == 0) token.type = TOKEN_WHILE;
        else if (strcmp(token.lexeme, "LTD") == 0) token.type = TOKEN_LTD;
        else token.type = TOKEN_IDENTIFIER;

    } else if (isdigit(current_char)) {
        int i = 0;
        while (isdigit(current_char)) {
            token.lexeme[i++] = current_char;
            nextChar();
        }

        // Check for invalid identifier: number followed by letter like "3a"
        if (isalpha(current_char)) {
            while (isalnum(current_char)) {  // Consume the invalid sequence
                nextChar();
            }
            token.type = TOKEN_INVALID;
            strcpy(token.lexeme, "invalid_identifier");
        } else {
            token.lexeme[i] = '\0';
            token.type = TOKEN_NUMBER;
        }

    } else {
        switch (current_char) {
            case '{': token.type = TOKEN_LBRACE; strcpy(token.lexeme, "{"); nextChar(); break;
            case '}': token.type = TOKEN_RBRACE; strcpy(token.lexeme, "}"); nextChar(); break;
            case '(': token.type = TOKEN_LPAREN; strcpy(token.lexeme, "("); nextChar(); break;
            case ')': token.type = TOKEN_RPAREN; strcpy(token.lexeme, ")"); nextChar(); break;
            case ';': token.type = TOKEN_SEMI; strcpy(token.lexeme, ";"); nextChar(); break;
            case '+': token.type = TOKEN_PLUS; strcpy(token.lexeme, "+"); nextChar(); break;
            case '-': token.type = TOKEN_MINUS; strcpy(token.lexeme, "-"); nextChar(); break;
            case '*': token.type = TOKEN_MUL; strcpy(token.lexeme, "*"); nextChar(); break;
            case '/': token.type = TOKEN_DIV; strcpy(token.lexeme, "/"); nextChar(); break;
            case '=':
                nextChar();
                if (current_char == '=') { token.type = TOKEN_EQ; strcpy(token.lexeme, "=="); nextChar(); }
                else { token.type = TOKEN_INVALID; strcpy(token.lexeme, "="); }
                break;
            case '!':
                nextChar();
                if (current_char == '=') { token.type = TOKEN_NE; strcpy(token.lexeme, "!="); nextChar(); }
                else { token.type = TOKEN_INVALID; strcpy(token.lexeme, "!"); }
                break;
            case '<':
                nextChar();
                if (current_char == '=') { token.type = TOKEN_LE; strcpy(token.lexeme, "<="); nextChar(); }
                else { token.type = TOKEN_LT; strcpy(token.lexeme, "<"); }
                break;
            case '>':
                nextChar();
                if (current_char == '=') { token.type = TOKEN_GE; strcpy(token.lexeme, ">="); nextChar(); }
                else { token.type = TOKEN_GT; strcpy(token.lexeme, ">"); }
                break;
            case '\0': token.type = TOKEN_END; strcpy(token.lexeme, "EOF"); break;
            default:
                token.type = TOKEN_INVALID;
                sprintf(token.lexeme, "%c", current_char);
                nextChar();
                break;
        }
    }

    return token;
}

void error(const char *message) {
    fprintf(stderr, " Syntax Error: %s\n", message);
    fflush(stderr);
    exit(1);
}


void match(TokenType expected) {
    if (lookahead.type == expected) {
        lookahead = getNextToken();
    } else {
        if (expected == TOKEN_SEMI && lookahead.type == TOKEN_RBRACE) {
            error("Missing semicolon");
        } else if (expected == TOKEN_RBRACE && lookahead.type == TOKEN_END) {
            error("Mismatched brackets: missing '}'");
        } else if (expected == TOKEN_IDENTIFIER && lookahead.type == TOKEN_INVALID) {
            error("Invalid identifier");
        } else {
            char msg[100];
            sprintf(msg, "Unexpected token: expected %s but got %s", token_str[expected], token_str[lookahead.type]);
            error(msg);
        }
    }
}


// === Parser Functions ===
void expression();

void factor() {
    if (lookahead.type == TOKEN_LPAREN) {
        match(TOKEN_LPAREN);
        expression();
        match(TOKEN_RPAREN);
    } else if (lookahead.type == TOKEN_IDENTIFIER || lookahead.type == TOKEN_LTD || lookahead.type == TOKEN_NUMBER) {
        match(lookahead.type);
    } else if (lookahead.type == TOKEN_INVALID &&
           strcmp(lookahead.lexeme, "invalid_identifier") == 0) {
    error("Invalid identifier");
} else {
    error("Invalid factor");
}

}

void term() {
    factor();
    while (lookahead.type == TOKEN_MUL || lookahead.type == TOKEN_DIV) {
        match(lookahead.type);
        factor();
    }
}

void expression() {
    term();
    while (lookahead.type == TOKEN_PLUS || lookahead.type == TOKEN_MINUS) {
        match(lookahead.type);
        term();
    }
}

void condition() {
    expression();
    if (lookahead.type >= TOKEN_EQ && lookahead.type <= TOKEN_GE) {
        match(lookahead.type);
    } else {
        error("Expected relational operator");
    }
    expression();
}

void statement();

void block() {
    match(TOKEN_LBRACE);
    while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_END) {
        statement();
    }
    match(TOKEN_RBRACE);
}

void ifStatement() {
    match(TOKEN_IF);
    match(TOKEN_LPAREN);
    condition();
    match(TOKEN_RPAREN);
    block();
    if (lookahead.type == TOKEN_ELSE) {
        match(TOKEN_ELSE);
        block();
    }
}

void whileStatement() {
    match(TOKEN_WHILE);
    match(TOKEN_LPAREN);
    condition();
    match(TOKEN_RPAREN);
    block();
}

void statement() {
    if (lookahead.type == TOKEN_IF) {
        ifStatement();
    } else if (lookahead.type == TOKEN_WHILE) {
        whileStatement();
    } else {
        expression();
        match(TOKEN_SEMI);
    }
}

void program() {
    block();
    if (lookahead.type != TOKEN_END) {
        error("Unexpected token after program end");
    }
    printf("Valid: Program parsed successfully.\n");
    fflush(stdout);
}

void parse(const char *src) {
    input = strdup(src);
    pos = 0;
    nextChar();
    lookahead = getNextToken();
    program();
    free((void*)input);
}

int main() {
    char buffer[MAX_INPUT_LEN] = "";
    char line[1024];

    printf("Enter your code (press Ctrl+Z when done):\n");

    while (fgets(line, sizeof(line), stdin)) {
        strcat(buffer, line);
    }

    parse(buffer);
    return 0;
}
