# Ling

Ling, in its current state, is a really simple and limited language. It is only meant to be a base for more complex future versions of the project.

## Syntax

Ling treats programs as an array of statements, meaning there is no `main` function. In fact, in its current state, Ling does not implement functions yet. This is going to change in future versions of the language.

### Statements

Ling, in its current state, allows 5 distinct types of statements.

-   Variable declaration - Declares a variable. As of now, the only available variables' types are integers.
    Example:
    ```
    let x = 69;
    ```
-   Variable assignment - Takes an already declared variable and assigns it a value.
    Example:
    ```
    x = 420;
    ```
-   If statement - Evaluates the given condition and performs the provided statement only if the value of the condition is not equal to 0.
    Example
    ```
    if(x)
        x = -1;
    ```
-   While statement - Evaluates the given condition and performs the provided statement as long as the value of the condition is not equal to 0.
    Example:
    ```
    while(x - 10)
        x = x + 1;
    ```
-   Display statement - Since Ling doesn't currently support functions nor syscalls, it is required to have at least one keyword that can display informations on the terminal. Currently the `display` keyword is used for that case but it will be deleted once functions and syscalls are introduced to the language. The display statement lets the user display a value of provided variable.
    Example:
    ```
    display x;
    ```

Those are all the statements that the Ling compiler will recognize. Notice there is **no** code-block statement and thus the if and while statements are only able to perform a single statement as their bodies. This is a significant limitation of the language's simplicity but it will be reworked in the future versions.

### Expressions

The Ling language defines the following expressions.

-   Literal - a constant, hard-coded value. Examples: `2137`, `67`
-   Variable - an identificator of previously declared variable. Retrieves the value stored in the variable. Examples: `x`, `counter`
-   Binary operator - if `p` and `q` are both expressions, the binary operator lets the user combine them in a desired way. The following binary operators are defined in Ling.
    -   `+` - representing addition
    -   `-` - representing subtraction
    -   `*` - representing multiplication
    -   `/` - representing division
    Note that boolean operators, such as `&&` or `||` are not implemented and neither are bitwise operators. Again, this will be reworked in a future version.
-   Unary operator - if `p` is an expression, the unary operator lets the user manipulate it in a desired way. The following unary operators are defined in Ling.
    -   `+` - representing identity
    -   `-` - representing arithmetic negation
    Note that boolean operators, such as `!` are not implemented and neither are bitwise operators. Again, this will be reworked in a future version.