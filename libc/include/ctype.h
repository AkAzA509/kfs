#ifndef CTYPE_H
#define CTYPE_H

#include <_attrs.h>

// checks for an alphanumeric character; it is equivalent to (isalpha(c) || isdigit(c)).
__NODISCARD int isalnum(int c);

// checks for an alphabetic character; It is equivalent to (isupper(c) || islower(c)).
__NODISCARD int isalpha(int c);

// checks for a digit (0 through 9).
__NODISCARD int isdigit(int c);

// checks for a lowercase character.
__NODISCARD int islower(int c);

// checks for any printable character including space.
__NODISCARD int isprint(int c);

// checks for white-space characters. These
// are: space, form-feed ('\f'), newline ('\n'), carriage return ('\r'),
// horizontal tab ('\t'), and vertical tab ('\v').
__NODISCARD int isspace(int c);

// checks for an uppercase letter.
__NODISCARD int isupper(int c);

// checks whether c is a 7-bit unsigned char value that fits into the ASCII character set.
__NODISCARD int isascii(int c);

#endif // CTYPE_H
