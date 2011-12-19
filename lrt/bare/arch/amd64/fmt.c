#include <stdint.h>
#include <stdarg.h>
#include "fmt.h"

Stream stdout;

void fprints(Stream *out, char *s);
void fprintnum(Stream *out, uintptr_t n, int base);
void vfprintf(Stream *out, char *fmt, va_list ap);

void printf(char *fmt, ...){
	va_list ap;
	va_start(ap,fmt);
	vfprintf(&stdout, fmt, ap);
	va_end(ap);
}

void fprintf(Stream *out, char *fmt, ...){
	va_list ap;
	va_start(ap,fmt);
	vfprintf(out, fmt, ap);
	va_end(ap);
}

void vfprintf(Stream *out, char *fmt, va_list ap){
	while(*fmt){
		switch(*fmt){
		case '%':
			fmt++;
			switch(*fmt){
			case 's':
				fprints(out, va_arg(ap, char *));
				fmt++;
				break;
			case 'd':
				fprintnum(out, va_arg(ap, uintptr_t), 10);
				fmt++;
				break;
			case 'x':
				fprintnum(out, va_arg(ap, uintptr_t), 16);
				fmt++;
				break;
			case 'o':
				fprintnum(out, va_arg(ap, uintptr_t), 8);
				fmt++;
				break;
			case 'b':
				fprintnum(out, va_arg(ap, uintptr_t), 2);
				fmt++;
				break;	
			default:
				out->putch(out->buf, *fmt);
				fmt++;
				break;
			}
		default:
			out->putch(out->buf, *fmt);
			fmt++;
			break;
		}
	}
}

void fprints(Stream *out, char *s){
	while(*s){
		out->putch(out->buf, *s);
		s++;
	}
}

void fprintnum(Stream *out, uintptr_t n, int base){
	char s[sizeof(uintptr_t)*8 + 1];
	int i;
	i = sizeof(uintptr_t)*8;
	s[i] = '\0';

	while(n > 0){
		i--;
		s[i] = "0123456789abcdef"[n % base];
		n /= base;
	}
	if(s[i] == '\0'){
		i--;
		s[i] = '0';
	}

	fprints(out, &s[i]);	
}
