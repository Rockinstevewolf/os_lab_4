#include "itoa.h"

int itoa(int n, char *s){//Возвращает длину строки
	int k, sign;
	int i, j;
	char c;
	if((sign = n) < 0)
		n = -n;

	i = 0;
	do{
		s[k++] = n % 10 + '0';
	}while ((n /= 10) > 0);

	if(sign < 0)
		s[k++] = '-';
	
	s[k] = '\0';
	for(i = 0, j = k-1; i < j; i++, j--){
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
	return k;
}
