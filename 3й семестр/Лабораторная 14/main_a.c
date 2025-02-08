#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <mem.h>

bool is_number(char* s){
	if (!strlen(s))
		return false;
	int i = 0;
	bool flag = true;
	for (; i < strlen(s); i++){
		if (!isdigit(tolower(s[i])))
			flag = false;
	};
	return flag;	
};

int* all_last_numbers(char* str, int* len){
	int count = 0;
    int i = 1;
    int string_len = strlen(str);
    for (; i <= string_len; i++){
        if ((str[i] == '.' || str[i] == '!' || str[i] == '?' || str[i] == '\0'))
            count++;
    };
    char* temp = malloc(string_len*sizeof(char));
    
    if (temp == NULL) {
		printf("Error: Memory not allocate! \n");
		exit(1);
	};
    
    memset(temp, '\0', (string_len)*sizeof(char));
	int* nums = (int*) calloc(count+1, sizeof(int));
	
	if (nums == NULL) {
		printf("Error: Memory not allocate! \n");
		exit(1);
	};
	
	int j = 0, n = 0;
	i = 0;
	int res = 0;
	while (j <= string_len){
		char current = str[j++];
		if (isdigit(current))
			temp[i++] = current;	
		else if ( isspace(current) || current == ',' || current == '\0' ) {
			if (is_number(temp)) nums[n] = atoi(temp);
			memset(temp, '\0', (string_len)*sizeof(char));
			i = 0;}
		else if ((current == '.' || current == '!' || current == '?' )) {
			if (is_number(temp)) nums[n++] = atoi(temp);
			else n++;
			memset(temp, '\0', (string_len)*sizeof(char));
			i = 0;
		} else{
			memset(temp, '\0', (string_len)*sizeof(char));
			i = 0;
		};
	};
	if (isdigit(temp[0]))
		nums[n] = atoi(temp);
	res = n+1;
	*len = res;
	free(temp);
	temp = NULL;
	return nums;
};

bool is_word(char* s){
	if (!strlen(s))
		return false;
	int i = 0;
	for (; i < strlen(s); i++){
		if (!isalpha(tolower(s[i])) && s[i] != '.' && s[i] != ',' && s[i] != '!' && s[i] != '?')
			return false;
	};
	return true;	
};

char** short_words(char* str, int k, int* result_len){
	int string_len = strlen(str);
    int count = string_len;
    char** result = (char**) calloc((count + 1), sizeof(char*));
    if (result == NULL) {
    	printf("Error: Memory not allocate! \n\n");
        exit(1);
    };
    int i;
    for (i = 0; i < count; i++){
    	result[i] = (char*) malloc((string_len + 1) * sizeof(char));
    	if (result[i] == NULL) {
			printf("Error: Memory not allocate! \n\n");
			exit(1);
		}
    	memset(result[i], '\0', (string_len + 1)*sizeof(char));
	};

    i = 0;
	int j = 0, current = 0;
    while (j <= string_len){
    	if (str[j] == ' ' || str[j] == '.' || str[j] == ',' || str[j] == '!' || str[j] == '?' || str[j] == '\0'){
    		if ((strlen(result[i]) <= k && is_word(result[i])) || (!is_number(result[i]) && !is_word(result[i])) || is_number(result[i])){
    			result[++i][0] = str[j];
    			result[i++][1] = '\0';
			}
    		else{
    			memset(result[i], '\0', (string_len + 1)*sizeof(char));
    			result[i][0] = str[j];
    			result[i++][1] = '\0';
			}	
			j++;
			current = 0;
		}else{
			result[i][current++] = str[j++];
		};
	};
	*result_len = i+1;
    return result;
};

char** words(char* str, int* result_len){
	int count = 1;
    int i = 0;
    int string_len = strlen(str);
    for (; i < string_len; i++){
        if (str[i] == ' ' || str[i] == '.' || str[i] == ',' || str[i] == '!' || str[i] == '?')
            count++;
    }
    
    char** result = (char**) calloc((count + 1), sizeof(char*));
    if (result == NULL) {
    	printf("Error: Memory not allocate! \n\n");
        exit(1);
    };
    for (i = 0; i < count; i++){
    	result[i] = (char*) malloc((string_len + 1) * sizeof(char));
    	if (result[i] == NULL) {
			printf("Error: Memory not allocate! \n\n");
			exit(1);
		}
    	memset(result[i], '\0', (string_len + 1)*sizeof(char));
	};

    i = 0;
	int j = 0, current = 0;
    while (j <= string_len){
    	if (str[j] == ' ' || str[j] == '.' || str[j] == ',' || str[j] == '!' || str[j] == '?' || str[j] == '\0'){
    		if (is_word(result[i]))
    			i++;
    		else
    			memset(result[i], '\0', (string_len + 1)*sizeof(char));
			j++;
			current = 0;
			*result_len = i+1;
		}else{
			result[i][current++] = str[j++];
		};
	};
	
	if (i == *result_len)
		*result_len++;
	else
		memset(result[i], '\0', (string_len + 1)*sizeof(char));
	
    return result;
};

bool equal_letters(char* s1, char* s2){
	if (strlen(s1) != strlen(s2))
		return false;
	int i = 0;
	for (; i < strlen(s2); i++){
		if (tolower(s1[i]) != tolower(s2[i]))
			return false;
	};
	return true;
};

int count_substring(char* str, char* s){
	int size;
	char** w = words(str, &size);
	int result = 0;
	int substr_len = strlen(s);
	char* temp = calloc(substr_len+1, sizeof(char));
	int i = 0;
	for (; i < size; i++){
		int j = 0;
		for (;j <= size; j++){
			temp = strncpy(temp, w[i]+j, substr_len);
			temp[substr_len] = '\0';		
			if (equal_letters(temp, s))
				result++;
		};
	};
	
	free(temp);
	temp = NULL;
	return result;
};

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

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
    	printf("Error: Incorrect input format\n\n");
        printf("Use a shablon: <exe file> <input_file_name> <output_file_name>\n\n");
        return 1;
    }

    char *input_filename = argv[1];
    char *output_filename = argv[2];

    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL)
    {
        printf("Error: Cant open input file!\n\n");
        return 1;
    }

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL)
    {
        printf("Error: Cant open output file!\n\n");
        fclose(input_file);
        return 1;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';

        char *start_quote = strchr(line, '"');
        if (start_quote == NULL)
        {
            fprintf(output_file, "Error: No marks\n\n");
            continue;
        }

        start_quote++;
        char *end_quote = strchr(start_quote, '"');
        if (end_quote == NULL)
        {
            fprintf(output_file, "Error: No marks\n\n");
            continue;
        }

        size_t text_length = end_quote - start_quote;
        char *str = calloc(text_length + 1, sizeof(char));
        strncpy(str, start_quote, text_length);
        str[text_length] = '\0';
        
        char *rest = end_quote + 1;
        while (*rest == ' ')
            rest++;

        char command[10];
        char arg[50];
        int num_items = sscanf(rest, "%s %s", &command, &arg);
        
        if (num_items < 1)
        {
            fprintf(output_file, "\"%s\": Error: no command\n\n", str);
            free(str);
            continue;
        }
        
        // -info -create -delete
        if(!strcmp(command, "-create")) {
			int count = 0;
			int* arr = all_last_numbers(str, &count);
			fprintf(output_file, "\n\nIn string \"%s\" \n founded %d digits that finish sentence\n", str, count);
			int i = 0;
			for (i;i < count; i++){
				fprintf(output_file, "%d) %d \n", i+1, arr[i]);
			};
			fprintf(output_file, "\n");
			free(arr);
			arr = NULL;
			
		} else if (!strcmp(command, "-info")) {
			int count = 0;
			fprintf(output_file, "\n\nIn string \"%s\" \nsum of numbers: ", str);
			//char* text = str;
			//fprintf(output_file, text);
			count = sum_numbers_in_string(str);
			fprintf(output_file, "%d\n\n", count);
			
		} else if (!strcmp(command, "-delete")) {
			int count = 0;			
			if (num_items < 2) {
                fprintf(output_file, "\"%s\": Error: input value of K.\n\n", str);
                free(str);
                continue;
            }

			int max_len = atoi(arg);
            if (max_len < 1)
            {
                fprintf(output_file, "\"%s\": Error: arg must be natural!\n\n", str);
                free(str);
                continue;
            }
			
			char** words = short_words(str, max_len, &count);
			int i = 0;
			fprintf(output_file, "\n\nIn string \"%s\" \nall words with length more than %d litters was deleted. Result:\n", str, max_len);
			for(i;i < count; i++){
				fprintf(output_file, "%s", words[i]);
				free(words[i]);
			};
			free(words);
	  		words = NULL;
	  }
        else
            fprintf(output_file, "\"%s\": Error: Unknown command \n\n", str);
        free(str);
    }

    fclose(input_file);
    fclose(output_file);
    
	return 0;
}
