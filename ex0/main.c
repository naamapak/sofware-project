# include <math.h>
# include <stdio.h>
# include <stdlib.h>

int convert_digit(char as_char){
    /* Using known ASCII values of 0-9, and A-F */
    int digit = (int) as_char;
    if(digit < 48 || digit > 70 || (digit < 65 && digit > 57)){
        printf("Illegal charachter\n");
        return 1;
    } else {
        if(digit <= 57){ // In this case digit is '0'-'9'
            return (digit - 48);
        }
        return(digit - 55); // If reached here, digit is 'A'-'F'
    }
    
}
char *_get_value(){
    char *input = NULL;
    char* temp = NULL;
    char ch = getchar();
    size_t size = 0;
    while((ch != '\n') && ch != EOF){
        temp = realloc(input, size + 1);
        if (temp == NULL){
            free(input);
            return NULL;
        }

        input = temp;
        input[size] = ch;
        size++;
        ch = getchar();
    }

    temp = realloc(input, size + 1);
    if (temp == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    input = temp;
    input[size] = '\0';
    return input;
}

int _get_base(){
    /* Gets input from user and checks if it is a legal base */
    char *value = _get_value();
    int result = atoi(value);
    free(value);
    if(result > 16 || result < 2){
        printf("Invalid base\n");
        return 0;
    }
    return result;
}

int get_source_base(){
    printf("Enter source base:\n");
    return _get_base();
}

int get_target_base(){
    printf("Enter target base:\n");
    return _get_base();
}


int to_base_10(char* value, int base){
    /* Converts a number in base "base" to int */
    int result = 0;
    for(int i = 0; value[i] != EOF; ++i){
        double curr = pow(base, i) * convert_digit(value[i]);
        result += (int) curr;
    }
    return result;
}

char* from_base_10(int number, int base){
    char* result = malloc(sizeof(char));
    while(number > 0){
        /* compute the rest of the stuff and do realloc for each digit */
    }
}
/*
char* compute_base(int source, int target, char* value){
    char* result = malloc(floor(sizeof(value) * log10(source) / log10(target)) + 1);
}*/
/* How do I not use arrays? Just malloc everything? */

int main(){
    int source = get_source_base();
    int target = get_target_base();
    if(source * target == 0){
        printf("Invalid input\n");
        return 1;
    }
    printf("Enter value to be converted:\n");
    char *value = _get_value();
    // do actual base conversion
    return 0;
}