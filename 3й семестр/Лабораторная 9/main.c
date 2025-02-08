#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Сумма целых чисел в строке
int sum_numbers_in_string(const char *str) {
  	int sum = 0;
  	int num = 0;
  	int index = 0;
  	int ln;
  	int count = 0;
  	
  	char* word = strtok(str, " .!?,");
	
	while (word != NULL)
	{
		//printf("%s \n", word);
		index = 0;
		ln = strlen(word);
		count = 0;
		while (index < ln)
		{
			if (isdigit(word[index]))
			{
				count++;
			}
			index++;
		}
		
		if (count == ln)
		{
			sum += atoi(word);
		}
		
		word = strtok(NULL, " .!?,");
	}

  	return sum;
}

// Массив слов, по длине больше K
char **get_long_words(char *str, int k, int *word_count) {
    int count = 0;
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == ' ' || str[i] == '.' || str[i] == ',' || str[i] == '!' || str[i] == '?') {
            count++;
        }
        i++;
    }
    count++;

    char **words = (char**)malloc((count + 1) * sizeof(char*));
    if (words == NULL) {
    	free(words);
        return NULL;
    }

    int word_index = 0;
    char *word = strtok(str, " .,?!");
    while (word != NULL) {
    	bool is_word = true;
    	int j = 0;
        for (j; word[j] != '\0'; j++) {
            if (isdigit(word[j])) {
                is_word = false;
                break;
            }
        }
    	
        if (is_word && strlen(word) > k) {
            words[word_index] = (char*)malloc((strlen(word) + 1) * sizeof(char));
            if (words[word_index] == NULL) {
                int j = 0;
                for (j; j < word_index; j++) {
                    free(words[j]);
                }
                free(words);
                return NULL;
            }
            strcpy(words[word_index], word);
            word_index++;
        }
        word = strtok(NULL, " .,?!");
    }

    words[word_index] = NULL;

    *word_count = word_index;

    return words;
}

char** long_words(char* str, int k, int* word_count)
{
	
	char **words = (char**)malloc(300 * sizeof(char*));
    if (words == NULL) {
    	free(words);
        return NULL;
    }
    
	char* word = strtok(str, " .,?!");
	int count = 0;
	int count1 = 0;
	int index = 0;
	int ln = 0;
	
	int word_index = 0;
	printf("Words: \n");
	while (word != NULL)
	{
		//printf("%s \n", word);
		index = 0;
		ln = strlen(word);
		count1 = 0;
		while (index < ln)
		{
			if (isdigit(word[index]))
			{
				count1++;
			}
			index++;
		}
		word[index] = '\0';
		
		if (count1 == 0 && strlen(word) > k)
		{
			words[word_index] = (char*)malloc((strlen(word) + 1) * sizeof(char));
            if (words[word_index] == NULL) {
                int j = 0;
                for (j; j < word_index; j++) {
                    free(words[j]);
                    printf("a");
                }
                free(words);
                return NULL;
            }
            strcpy(words[word_index], word);
            printf("%s \n", word);
            //printf("Dobavlen");
			count++;
		}
		
		word = strtok(NULL, " .,?!");
		//printf("%s\n", word);
	}
	
	*word_count = count;
	
	return words;
}

// Удаление всех первых чисел в предложениях строки
char* delete_first_numbers(char* str) {
	if (str == NULL) {
		return NULL;
	}
	
	char* strout = (char*) malloc(300);
	
	if (!strout) {
		return NULL;
	}

    int indexIN = 0;
    int indexOUT = 0;
    
    int isDel = 0;
    
    bool isStart = false;
    bool isEnd = false;
    int count = 0;

    while (str[indexIN] != '\0')
    {
    	if (str[indexIN] == '.' || str[indexIN] == '!' || str[indexIN] == '?') 
		{
			isEnd = false;
			isStart = false;
			strout[indexOUT] = str[indexIN];
			indexIN++;
			indexOUT++;
		}
    	else if (isdigit(str[indexIN]) && !isStart && !isEnd)
		{
			isStart = true;
			indexIN++;
			count++;
		}
		else if (isdigit(str[indexIN]) && isStart && !isEnd)
		{
			indexIN++;
			count++;
		}
		else if (!isdigit(str[indexIN]) && !isStart && !isEnd)
		{
			strout[indexOUT] = str[indexIN];
			indexIN++;
			indexOUT++;
		}
		else if (!isdigit(str[indexIN]) && isStart && !isEnd)
		{
			isEnd = true;
			strout[indexOUT] = str[indexIN];
			indexIN++;
			indexOUT++;
		}
		else if (isdigit(str[indexIN]) && isStart && isEnd)
		{
			strout[indexOUT] = str[indexIN];
			indexIN++;
			indexOUT++;
		}
        else
		{
        	strout[indexOUT] = str[indexIN];
			indexIN++;
			indexOUT++;
		}
    }

    strout[indexOUT] = '\0';
    
  	return strout;
}

char* remove_first(char* str) {
	if (str == NULL) {
		return NULL;
	}
	
	char* strout = (char*) malloc(300);
	
	if (!strout) {
		return NULL;
	}
	
	int index, ln, count;
	bool isFirst = true;
	char* token = strtok(str, ".!?");
	
	while (token != NULL) {
		char *word = strtok(token, " ");
		isFirst = true;
		
	    while (word != NULL) {
	        index = 0;
	        ln = strlen(word);
	        count = 0;
	        while (index < ln)
	        {
	        	if (isdigit(word[index]))
	        	{
	        		count++;
				}
				index++;
			}
			
			if (count == ln && isFirst)
			{
				isFirst = false;
			}
			else if ((count == ln && !isFirst) || count != ln)
			{
				strcat(strout, word);
				strcat(strout, " ");
				//printf("Dobavlen \n");
			}
			
			word = strtok(NULL, " ");
	    }
	    
	    token = strtok(NULL, ".!?");
	}
	
	ln = strlen(strout);
	strout[ln] = '\0';
	
	return strout;
}

char* remove_first1(char* str) {
	if (str == NULL) {
		return NULL;
	}
	
	char* strout = (char*) malloc(300);
	
	if (!strout) {
		return NULL;
	}
	
	int index, ln, count;
	bool isFirst = true;
	char* word = strtok(str, " ");
	
	while (word != NULL)
	{
		index = 0;
		ln = strlen(word);
		count = 0;
		while (index < ln)
		{
			if (isdigit(word[index]))
			{
				count++;
			}
			index++;
		}
		//printf("%d \n", index);
		//printf("%c \n", word[index-1]);
		//printf("%d \n", ln);
		//printf("%d \n", count);
		
		if (count == ln && isFirst)
		{
			isFirst = false;
		}
		else if (count - 1 == ln && isFirst && (word[index-1] == '.' || word[index-1] == '!' || word[index-1] == '?'))
		{
			char* ch = word[index-1];
			strcat(strout, ch);
			strcat(strout, " ");
		}
		else if (word[index-1] == '.' || word[index-1] == '!' || word[index-1] == '?')
		{
			isFirst = true;
			strcat(strout, word);
			strcat(strout, " ");
		}
		else if ((count == ln && !isFirst) || count != ln)
		{
				strcat(strout, word);
				strcat(strout, " ");
		}
		
		word = strtok(NULL, " ");
	}
	
	ln = strlen(strout);
	strout[ln] = '\0';
	
	return strout;
}

int main(int argc, char* argv[]) {
	if ((!strcmp(argv[2], "-create") && argc <= 3) || argc <= 2) {
		printf("Error: Insufficient number of variables\n");
		return 1;
	}
	
	if (strlen(argv[1]) > 300) {
		printf("Error: Too long input string");
		return 1;
	}
	
	char * command = argv[2];
	
	char* str = (char*) malloc(300);
	if (str == NULL) {
		free(str);
		return 1;
	}
	
	str = argv[1];
	
	if(!strcmp(command, "-create")) {
		int K = atoi(argv[3]);
		int word_count = 0;
		char **words = long_words(str, K, &word_count);
		if (words == NULL) {
    		printf("Error: words didnt forming\n");
    		return 1;
  		}
	  	free(words);
	  	words = NULL;
  		
	} else if (!strcmp(command, "-info")) {
		int count = 0;
		count = sum_numbers_in_string(str);
		printf("Sum of numbers in line: %d", count);
		
	} else if (!strcmp(command, "-delete")) {
		char* strout = (char*) malloc(300);
		strout = remove_first1(str);
		strout[0] = ' ';
		strout[1] = ' ';
		strout[2] = ' ';
		printf("Result line: %s", strout);
		free(strout);
	}
	
	free(str);
	str = NULL;
	
	return 0;
}
