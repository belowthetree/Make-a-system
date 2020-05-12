#include "string.h"

int strcmp(const char * str1, const char * str2){
	int i = 0;
	while (i < STRLIMIT && str1[i] == str2[i] && str1[i] != '\0')
		i++;

	if (str1[i] == str2[i])
		return 1;
	return 0;
}

int strncmp(const char * str1, const char * str2, int n){
	int i = 0;
	while (i < n && str1[i] == str2[i] && str1[i] != '\0')
		i++;
	if (i >= n || str1[i] == str2[i])
		return 1;
	return 0;
}

int strlen(const char *str){
	int len = 0;
	while (str[len] != '\0')
		len++;
	return len;
}

int strcmpns(const char * str1, const char * str2){
	int left = 0, right = 0;

	while (str1[left] != '\0' && str2[right] != '\0'){
		if (str1[left] == ' '){
			left++;
			continue;
		}
		if (str2[right] == ' '){
			right++;
			continue;
		}
		if (str1[left] != str2[right])
			break;
		left++;
		right++;
	}
	while (str1[left] == ' ')
		left++;
	while (str2[right] == ' ')
		right++;

	if (str1[left] == str2[right])
		return 1;
	return 0;
}