/*
 * Copyright (C) 2011 by Project SESA, Boston University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <l0/lrt/bare/arch/amd64/stdio.h>
#include <l0/lrt/bare/arch/amd64/string.h>

FILE *stdout;
FILE *stdin;
FILE *stderr;

int
fputc(int c, FILE *stream)
{
  unsigned char uc = (unsigned char)c;
  return stream->write(stream->cookie, (char *)&uc, 1);
}

int
fputs(const char *s, FILE *stream)
{
  return stream->write(stream->cookie, s, __builtin_strlen(s));
}

int
putc(int c, FILE *stream)
{
  return fputc(c, stream);
}

int 
putchar(int c)
{
  return putc(c, stdout);
}

int
puts(const char *s)
{
  // DS: Variable length array (C99)
  char str[__builtin_strlen(s) + 1];
  __builtin_strcpy(str, s);
  str[__builtin_strlen(s)] = '\n';
  return stdout->write(stdout->cookie, str, __builtin_strlen(s) + 1);
}

int
printf(const char *format, ...)
{
  int rc;
  va_list ap;
  va_start(ap, format);
  rc = vfprintf(stdout, format, ap);
  va_end(ap);
  return rc;
}

static const unsigned char JUST_LEFT = (1 << 0);
static const unsigned char POSITIVE_SIGN = (1 << 1);
static const unsigned char NO_SIGN_SPACE = (1 << 2);
static const unsigned char POUND_FLAG = (1 << 3);
static const unsigned char PAD_WITH_ZEROES = (1 << 4);
static const unsigned char SHORT = (1 << 5);
static const unsigned char LONG = (1 << 6);
static const unsigned char LONGLONG = (1 << 7);

#define PRINT_BUF_LEN (80)




static int 
prints(const char *str, int width, int precision, unsigned char flags, 
       FILE *stream)
{
  int count = 0;
  char pad;
  int len = 0;
  const char *ptr;
  
  if(precision != -1 && width > precision) {
    width = precision;
  }
  if (flags & PAD_WITH_ZEROES) {
    pad = '0';
  } else {
    pad = ' ';
  }  
  if (width > 0) {
    for(ptr = str; *ptr != '\0'; ptr++, len++) ;
    if(len >= width) {
      width = 0;
    } else {
      width -= len;
    }
  }  
  if (!(flags & JUST_LEFT)) {
    for(; width > 0; width--) {
      putc(pad, stream);
      count++;
    }
  }
  for(; precision != 0 && *str != '\0'; str++, precision--) {
    putc(*str, stream);
    count++;
  }
  for(; width > 0; width--) {
    putc(pad, stream);
    count++;
  }
  return count;
}

static int printi(long long val, unsigned int base, int sign,
		  int width, int precision,
		  unsigned char flags, char cbase, FILE *stream) {
  char print_buf[PRINT_BUF_LEN];
  int pos = 0;
  int neg = 0;
  unsigned long long uval = (unsigned long long) val;
  unsigned long long rem;
  char *s;
  if (val == 0) {
    if (flags & NO_SIGN_SPACE) {
      print_buf[pos++] = ' ';
    }
    if (precision == 0) {
      print_buf[pos++] = '\0';
    } else {
      print_buf[pos++] = '0';
      print_buf[pos++] = '\0';
    }
    return prints(print_buf, width, -1, flags, stream);
  }
  if (sign == 1 && base == 10 && val < 0) {
    neg = 1;
    uval = (unsigned long long) -val;
  }
  s = print_buf + PRINT_BUF_LEN - 1;
  *s = '\0';

  while (uval != 0) {
    rem = uval % base;
    if (rem >= 10) {
      rem += cbase - 10;
    }
    *--s = (char)rem + '0';
    pos++;
    uval = uval / base;
  }
  for(; precision > pos; pos++) {
    *--s = '0';
  }
  if (base == 10) {
    if (neg) {
      *--s = '-';
    } else if (flags & POSITIVE_SIGN) {
      *--s = '+';
    } else if (flags & NO_SIGN_SPACE) {
      *--s = ' ';
    }
  } else if (base == 16) {
    *--s = cbase;
    *s -= 'A';
    *s += 'X';
    *--s = '0';
  }
  return prints(s, width, -1, flags, stream);
}

int
vfprintf(FILE *stream, const char *format, va_list ap)
{
  unsigned char flags;
  unsigned int count = 0;
  unsigned int width;
  int precision;
  char cr[2];
  char *str;

  for (; *format != '\0'; format++) {
    if (*format == '%') {
      if (*(++format) == '%') {
	goto printit;
      }
      flags = 0;
      width = 0;
      precision = -1;
      /* flags */
      if (*format == '-') {
	format++;
	flags |= JUST_LEFT;
      }
      if (*format == '+') {
	format++;
	flags |= POSITIVE_SIGN;
      }
      if (*format == ' ') {
	format++;
	flags |= NO_SIGN_SPACE;
      }
      if (*format == '#') {
	format++;
	flags |= POUND_FLAG;
      }
      if (*format == '0') {
	format++;
	flags |= PAD_WITH_ZEROES;
      }
      /* width */
      if (*format == '*') {
	format++;
	width = va_arg(ap, int);
      } else {
	for (; *format >= '0' && *format <= '9'; format++) {
	  width *= 10;
	  width += *format - '0';
	}
      }
      /* precision */
      if (*format == '.') {
	format++;
	if (*format == '*') {
	  format++;
	  precision = va_arg(ap, int);
	} else {
	  for (precision = 0; *format >= '0' && *format <= '9'; format++) {
	    precision *= 10;
	    precision += *format - '0';
	  }
	}
      }
      /* length */
      if (*format == 'h') {
	format++;
	flags |= SHORT;
      } else if (*format == 'l') {
	format++;
	if(*format == 'l') {
	  format++;
	  flags |= LONGLONG;
	} else {
	  flags |= LONG;
	}
      } else if (*format == 'L') {
	format++;
	flags |= LONG;
      }
      /* specifiers */
      if (*format == 'c') {
	cr[0] = (char)va_arg(ap, int);
	cr[1] = '\0';
	count += prints(cr, width, 1, flags, stream);
      } else if (*format == 's') {
	str = (char *)va_arg(ap, long);
	count += prints(str ? str : "(null)", width, precision, flags, stream);
      } else if (*format == 'd' || *format == 'i') {
	if(flags & SHORT) {
	  count += printi((long long)va_arg(ap, int),
			  10, 1, width, precision, flags, 'a', stream);
	} else if(flags & LONG) {
	  count += printi((long long)va_arg(ap, long),
			  10, 1, width, precision, flags, 'a', stream);
	} else if(flags & LONGLONG) {
	  count += printi((long long)va_arg(ap, long long),
			  10, 1, width, precision, flags, 'a', stream);
	} else {
	  count += printi((long long)va_arg(ap, int),
			  10, 1, width, precision, flags, 'a', stream);
	}
      }
    } else {
    printit:
      cr[0] = *format;
      cr[1] = '\0';
      count += prints(cr, 0, 1, 0, stream);
    }
  }
  return count;
}
