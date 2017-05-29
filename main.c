#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "Position.h"

#ifdef DBG
#define DEBUG printf
#else
#define DEBUG printf_echo_off
#endif

void printf_echo_off(const char* _out, ...){}

// Считывает всё содержимое файла в строку. 0 при ошибке
char* readFile(const char* _filename, unsigned long* _length)
{
	char *ret = 0;
	FILE *input = fopen(_filename, "rb");
	if(!input) return 0;
	
	fseek(input, 0, SEEK_END);
	long length = ftell(input);
	fseek (input, 0, SEEK_SET);
	
	ret = malloc(length + 1);
	if(ret)
	{
		fread(ret, 1, length, input);
	}
	fclose(input);
	ret[length] = 0;
	if(_length) *_length = length;
	return ret;
}

// Сравнивает две строки до заданной длинны. 0 — не совпадают, 1 — совпадают
const char eq(const char* _str1, char* _str2, const int _length)
{
	char tmp = _str2[_length];
	_str2[_length] = 0;
	DEBUG("\tEq: %s, %s, %d\n", _str1, _str2, _length);
	_str2[_length] = tmp;
	int i;
	for(i = 0; i < _length; ++i)
	{
		if(_str1[i] != _str2[i]) return 0;
	}
	return 1;
}

// Ищет _needle в _haystack и возвращает все вхождения
unsigned int find(const char* _needle, char* _haystack, Position **_ret)
{
	*_ret = 0;
	
	unsigned long offset = 0, needleSum = 0, haystackSum = 0;
	unsigned int len = 0, count = 0;
	do {
		while(_needle[len])
		{
			if(!_haystack[offset + len])
			{
				return count;
			}
			DEBUG("\t%c %c (%lu, %lu)\n", _needle[len], _haystack[offset + len], needleSum, haystackSum);
			needleSum += _needle[len];
			haystackSum += _haystack[offset + len];
			++len;
		}
		
		if(needleSum == haystackSum && eq(_needle, _haystack + offset, len))
		{
			*_ret = pstPush(offset, *_ret);
			++count;
			offset += len;
			needleSum = haystackSum = len = 0;
		}
		else
		{
			if(!_haystack[offset + len]) break;
			DEBUG("\t%c (%lu, %lu)\n", _haystack[offset], needleSum, haystackSum);
			haystackSum -= _haystack[offset];
			haystackSum += _haystack[offset + len];
			++offset;
		}
	} while(1);
	
	return count;
}

void strCopy(const char* _src, char* _dst, unsigned long _len)
{
	while(_len--)
	{
		*(_dst++) = *(_src++);
	}
}

int main()
{
	// Извлекаем исходный текст
	unsigned long textLength;
	char *text = readFile("input.txt", &textLength);
	if(!text)
	{
		printf("Coudn't read input.txt");
		return 1;
	}
	
	// Открываем словарь
	FILE *dict = fopen("dictionary.txt", "r");
	char pattern[256], *change;
	unsigned int patternLength, changeLength;
	
	// Каждую пару слов из словаря, разделённую символом '-' записываем в pattern
	while(fgets(pattern, 255, dict))
	{
		// Определяем длинну искомого слова
		patternLength = changeLength = 0;
		change = 0;
		while(pattern[patternLength] != '-' && pattern[patternLength] != 0)
		{
			++patternLength;
		}
		// Если не нашли '-', значит у слова нет замены, пропускаем
		if(pattern[patternLength] != '-') continue;
		pattern[patternLength] = 0;
		DEBUG("Finding %s:\n", pattern);
		
		// Ищем все вхождения pattern'а. Если не находим, пропускаем
		Position *pos = 0;
		unsigned int posCount = find(pattern, text, &pos);
		DEBUG("Found %d.\n\n", posCount);
		if(posCount == 0) continue;
		
		// Определяем длинну слова для замены
		change = pattern + patternLength + 1;
		while(change[changeLength] != 0 && change[changeLength] != '\n' && change[changeLength] != '\r')
		{
			++changeLength;
		}
		change[changeLength] = 0;
		DEBUG("Inserting %s:\n", change);
		
		// Вставляем новое слово по найденным позициям
		char* rewritten = malloc(textLength + posCount * (changeLength - patternLength) + 1);
		if(!rewritten) continue;
		
		unsigned long oldOffset = 0, newOffset = 0;
		Position* end = pos->begin;
		do{
			DEBUG("copy from %lu to %lu\n", oldOffset, end->value);
			strCopy(text + oldOffset, rewritten + newOffset, end->value - oldOffset);
			newOffset += (end->value - oldOffset);
			oldOffset = end->value + patternLength;
			strCopy(change, rewritten + newOffset, changeLength);
			newOffset += changeLength;
		} while((end = end->next));
		DEBUG("copy from %lu to end\n\n", oldOffset);
		strCopy(text + oldOffset, rewritten + newOffset, textLength - oldOffset + 1);
		
		// Переносим указатель на новый текст на его место и подчищаем за собой
		free(text);
		text = rewritten;
		textLength += posCount * changeLength;
		textLength -= posCount * patternLength;
		rewritten = 0;
		pstFree(pos);
	}
	fclose(dict);
	
	FILE *output = fopen("output.txt", "wb");
	fputs(text, output);
	fclose(output);
	
	free(text);
	return 0;
}