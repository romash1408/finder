#include <stdio.h>
#include <stdlib.h>


// ��������� �� ���������� ����� � ������. 0 ��� ������
char* readFile(const char* _filename)
{
	char *ret = 0;
	FILE *input = fopen(_filename, "rb");
	if(!input) return 0;
	
	fseek(input, 0, SEEK_END);
	long length = ftell(input);
	fseek (input, 0, SEEK_SET);
	
	ret = malloc(length);
	if(ret)
	{
		fread(ret, 1, length, input);
	}
	fclose(input);
	ret[length] = 0;
	return ret;
}

// ���������� ��� ������ �� �������� ������. 0 � �� ���������, 1 � ���������
const char eq(const char* _str1, const char* _str2, const int _length)
{
	int i;
	for(i = 0; i < _length; ++i)
	{
		if(_str1[i] != _str2[i]) return 0;
	}
	return 1;
}

// �������� �� ������ _body, ������� ��� ��������� _pattern �� *
void replace(char* _body, const char* _pattern, const int _patternLength)
{
	int bodySum = 0, patternSum = 0, i;
	for(i = 0; i < _patternLength; ++i)
	{
		if(!_body[i]) return;
		bodySum += _body[i];
		patternSum += _pattern[i];
	}
	
	// cursor ��������� �� �����, ������ ��� ����������, ��� ���������� ����������
	// ��������� ������ ��������� �� ����
	char *cursor = _body, rightAfterEnd = 0;
	do
	{
		if(!rightAfterEnd)
		{
			if(bodySum == patternSum && eq(_body, _pattern, _patternLength))
			{
				*(cursor++) = '*';
				_body += _patternLength;
				bodySum = 0;
				for(i = 0; i < _patternLength; ++i)
				{
					if(!_body[i])
					{
						rightAfterEnd = 1;
						break;
					}
					bodySum += _body[i];
				}
				continue;
			}
			bodySum -= _body[0];
			bodySum += _body[_patternLength];
			if(!_body[_patternLength]) rightAfterEnd = 1;
		}
		*(cursor++) = *(_body++);
	} while(*_body);
	*cursor = 0;
}

int main()
{
	// ��������� �������� �����
	char *text = readFile("input.txt");
	if(!text)
	{
		printf("Coudn't read input.txt");
		return 1;
	}
	
	//printf("%s:\n%s\n\n", "source", text);
	
	// ��������� �������
	FILE *dict = fopen("dictionary.txt", "r");
	char pattern[256];
	int patternLength;
	
	// ������ ����� �� ������� ���������� � pattern
	while(fgets(pattern, 255, dict))
	{
		// ���������� ������ �������� �����
		patternLength = 0;
		while(pattern[patternLength] != 0 && pattern[patternLength] != '\n')
		{
			++patternLength;
		}
		pattern[patternLength] = 0;
		
		// ���������� ������
		replace(text, pattern, patternLength);
		
	
		//printf("%s:\n%s\n\n", pattern, text);
	}
	fclose(dict);
	
	FILE *output = fopen("output.txt", "wb");
	fputs(text, output);
	fclose(output);
	
	free(text);
	return 0;
}