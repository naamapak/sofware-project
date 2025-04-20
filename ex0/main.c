# include <stdio.h>
# include <stdlib.h>
# include <math.h>

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


char* compute_base(int source, int target, char* value){
    char* digits;
    digits = "0123456789ABCDEF";
    char* result = malloc(floor(sizeof(value) * log10(source) / log10(target)) + 1);
    
}
/* How do I not use arrays? Just malloc everything? */

int main(){
    int source = get_source_base();
    int target = get_target_base();
    if(source * target == 0){
        printf("Invalid input\n");
        return 1;
    }
    char *value = _get_value();
    // do actual base conversion
    return 0;
}