#include <string.h>

/** R{knar om grader till radianer.
 @var a: vinkeln i grader.
 @return vinkeln i radianer.
 */
double getRadians(int a) {
	return (a+90)*3.14159265/180;
}

/** G|r om en int till en str{ng.
 @var n: siffran som ska omvandlas
 @var s: str}ngen som fylls med bokst{ver.
 @var bas: vilken bas funktionen ska r{kna p}.
 */
void itoa(int n, char s[],int bas) { // Fr}n Kerninghan & Ritchie
	int c,i,j;
	i=0;
	do s[i++]=n%bas+'0'; while ((n/=bas)>0);
	s[i]='\0';
	for (i=0, j=strlen(s)-1;i<j;i++,j--) {
		c=s[i];
		s[i]=s[j];
		s[j]=c;
	}
}

/** Kopierar en str{ng till en annan.
 @var source: str{ngen som ska kopieras.
 @var dest: str{ngen som ska bli kopierad till.
 */
void stringCopy(char* source, char* dest) {
	while ((*dest = *source) != '\0') {
		source++;
		dest++;
	}
}

/** G|r om en IPaddress i SDL till en str{ng-URL.
 @var ipaddress: en int som inneh{ller en SDLnet ipaddress.
 @var URL: Den char-array som ska fyllas.
 */
void IPtoAscii(int ipaddress, char* URL) {
	char temp[4];
	int i;
	itoa((char)ipaddress,temp,10);
	strcat(URL,temp);
	strcat(URL,".");
	itoa((char)(ipaddress>>8),temp,10);
	strcat(URL,temp);
	strcat(URL,".");
	itoa((char)(ipaddress>>16),temp,10);
	strcat(URL,temp);
	strcat(URL,".");
	itoa((char)(ipaddress>>24),temp,10);
	strcat(URL,temp);
}
