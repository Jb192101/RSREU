
TYPE* rand_generate_arr(TYPE* arr, float mn, float mx) {
	TYPE len = LEN(arr);   											
	int i = 0;
	for(i; i < len; i++){													
		ELEM(arr, i) = (TYPE) ((float)rand() / RAND_MAX * (mx - mn) + mn);
	};
	return arr;
};

TYPE* del_elements(TYPE* line, const int N, const int K) {
	if (K < 1 || K + N > LEN(line) || N < 0) {
    	exit(1);
    }
	
	int i = K-1;
	for (i; i < LEN(line); i++) {
    	ELEM(line, i) = ELEM(line, i + N);
    }
    
  	LEN(line) -= N;

  	line = (TYPE*)realloc(line, LEN(line) * sizeof(TYPE));
  	return line;
}

TYPE* add_elements(TYPE* line, const int N, const int K, const TYPE min, const TYPE max)
{
	if (K < 1 || K > LEN(line) || N < 0) {
    	exit(1);
  	}
  	
  	line = (TYPE*) realloc(line, (LEN(line) + N) * sizeof(TYPE));
	
	int i = LEN(line) + N;
	for (i; i > K + N; i--) {
    	ELEM(line, i - 1) = ELEM(line, i - N - 1);
    }
    ELEM(line, N + K - 1) = ELEM(line, K - 1);
    
    srand(time(0));
	int j = K-1;
	for(j; j < N + K - 1; j++) {
		ELEM(line, j) = (TYPE) ((float)rand() / RAND_MAX * (max - min) + min);
	}
    
    LEN(line) += N;
    return line;
}

TYPE* shift_elements(TYPE* line, int M)
{
	if (line[0] < 1 || M < 0) {
    		return line;
  	}

  	M %= LEN(line); 
	
	int i = 0;
	#if VAR_FUNC1 == 1
  		for (i; i < M; i++) {
    		TYPE temp = ELEM(line, LEN(line) - 1);
    		TYPE j = LEN(line) - 1;
    		for (j; j > 0; j--) {
				ELEM(line, j) = ELEM(line, j-1);
    		}
			ELEM(line, 0) = temp;
  		}
	#else
  		for (i; i < M; i++) {
    		TYPE temp = ELEM(line, LEN(line) - 1);
    		TYPE j = LEN(line) - 2;
    		for (j; j >= 0; j--) {
				ELEM(line, j + 1) = ELEM(line, j);
    		}
			ELEM(line, 0) = temp;
  		}
	#endif

	return line;
}

void nule_last_element(TYPE* line)
{
	if (LEN(line) < 1) {
    		exit(1);
  	}

	int last_elem_index = -1;
	int i = 0;
	for (i; i < LEN(line); i++) {
		#if VAR_FUNC2 == 1
			if (ELEM(line, i) < 0) {
				last_elem_index = i;
			}
		#else
			if (ELEM(line, i) > 0) {
				last_elem_index = i;
			}
		#endif
	}

	if (last_elem_index != -1) {
		ELEM(line, last_elem_index) = 0;
  	}
}
