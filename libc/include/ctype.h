#ifndef CTYPE_H
#define CTYPE_H

// checks for an alphanumeric character; it is equivalent to (isalpha(c) || isdigit(c)).
[[nodiscard]] int isalnum(int c);

// checks for an alphabetic character; It is equivalent to (isupper(c) || islower(c)).
[[nodiscard]] int isalpha(int c);

// checks for a digit (0 through 9).
[[nodiscard]] int isdigit(int c);

// checks for a lowercase character.
[[nodiscard]] int islower(int c);

// checks for any printable character including space.
[[nodiscard]] int isprint(int c);

// checks for white-space characters. These
// are: space, form-feed ('\f'), newline ('\n'), carriage return ('\r'),
// horizontal tab ('\t'), and vertical tab ('\v').
[[nodiscard]] int isspace(int c);

// checks for an uppercase letter.
[[nodiscard]] int isupper(int c);

// checks whether c is a 7-bit unsigned char value that fits into the ASCII character set.
[[nodiscard]] int isascii(int c);

#endif // CTYPE_H
