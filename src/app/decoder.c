#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct morse_character {
	char simbol;
	char morze_r[5]; /* Max lenght of char in morse code */
};

static struct morse_character eng_alphabet[] = {
    {'A', "._\0"},
    {'B', "_...\0"},
    {'C', "_._.\0"},
    {'D', "_..\0"},
    {'E', ".\0"},
    {'F', ".._.\0"},
    {'G', "__.\0"},
    {'H', "....\0"},
    {'I', "..\0"},
    {'J', ".___\0"},
    {'K', "_._\0"},
    {'L', "._..\0"},
    {'M', "__\0"},
    {'N', "_.\0"},
    {'O', "___\0"},
    {'P', ".__.\0"},
    {'Q', "__._\0"},
    {'R', "._.\0"},
    {'S', "...\0"},
    {'T', "_\0"},
    {'U', ".._\0"},
    {'V', "..._\0"},
    {'W', ".__\0"},
    {'X', "_.._\0"},
    {'Y', "_.__\0"},
    {'Z', "__..\0"}
};

bool compare(char *str1, char *str2) {
    bool res = true;
    int i = 0;

    while(str1[i] != '\0') {
        res &= str1[i] == str2[i];
        i++;
    }

    return res;
}

char decode(char *character, int character_lenght){
	for (int i = 0; i < 26; i++) {
        int symbol_size = 0;
        while (eng_alphabet[i].morze_r[symbol_size] != '\0')
        {
            symbol_size++;
        }
		if (symbol_size == character_lenght && compare(character, eng_alphabet[i].morze_r)) {
            return eng_alphabet[i].simbol;
		}	
	}
	return '\0';
}