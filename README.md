# Recursive-Descent-Parse
Recursive Descent Parser for Control Structures and Expressions in C

## 📌 Project Overview
This project is a **recursive descent parser** written in C.  
It performs **lexical analysis (tokenization)** and **parsing** for a simplified programming language that supports:

- Control structures: `if`, `else`, `while`  
- Blocks: `{ ... }`  
- Expressions with operators: `+`, `-`, `*`, `/`  
- Relational operators: `==`, `!=`, `<`, `<=`, `>`, `>=`  
- Identifiers, numbers, and a special token `LTD` (last 3 digits of student ID → **115** in this code)  

It checks for **syntax correctness** and reports errors with details.


## ⚡ Features
- 🔎 Lexer that identifies keywords, identifiers, numbers, operators, and symbols  
- 🚨 Detects **invalid identifiers** (e.g., `3abc`)  
- 🚨 Detects **missing semicolons and mismatched braces**  
- 🌀 Recursive descent parsing for:
  - Expressions
  - Conditions
  - Statements (`if`, `while`, expression-like statements)  
- 📝 Error messages with detailed feedback  


## 🛠️ How It Works
1. The program first reads the **entire input source code** from the user (until `Ctrl+Z` on Windows / `Ctrl+D` on Linux/Mac).  
2. The **lexer** breaks it down into tokens.  
3. The **parser** checks if the sequence of tokens matches the grammar rules.  

## 📜 Example Input
{
 if (a == LTD) {
     while (b < 100) {
         (a + b) * (b - LTD);
     }
 } else {
     (x + y) * (a - b);
 }
}
✅ Output
Valid: Program parsed successfully.


❌ Example With Error

##Input (missing semicolon):
{
    a + b
}
❌Output:

 Syntax Error: Missing semicolon

##Input (mismatched bracket):
{
    if (a == b) {
        a + b;
❌Output:

 Syntax Error: Mismatched brackets: missing '}'

##Input (invalid identifier):
{
    3a + 5;
}
❌Output:

 Syntax Error: Invalid identifier

