# include <math.h>
# include <stdio.h>
# include <stdlib.h>

int illegal_base(int base){
    /* Checks if the base entered is illegal*/
    if(base < 2 || base > 16){
        return 1;
    }
    return 0;
}

int convert_digit(char as_char){
    /* Using known ASCII values of 0-9, and A-F */
    int digit = (int) as_char;
    if(digit < 48 || digit > 70 || (digit < 65 && digit > 57)){
        printf("Illegal charachter: %d\n", digit);
        return 1;
    } else {
        if(digit <= 57){ // In this case digit is '0'-'9'
            return (digit - 48);
        }
        return(digit - 55); // If reached here, digit is 'A'-'F'
    }
}

int get_input(int base){
    /* Does the main loop getting input from the user */
    char ch = getchar();
    int result = 0;
    while(ch == EOF || ch == '\n'){ // Waiting for user
        ch = getchar();
    }
    while(ch != '\n'){
        result = result * base;
        result += convert_digit(ch);
        ch = getchar();
    }
    return result;
}

char make_digit(int value){
    if(value <= 9){
        return (char) value + 48;
    } else {
        return (char) value + 65;
    }
}

int convert_and_print(int value, int base){
    /* Recursively converts to target base and prints the following character */
    char curr;
    if(value < base){
        curr = make_digit(value);
        printf(&curr);
        return 0;
    } else {
        curr = make_digit((int) remainder(value, base));
        convert_and_print((value - remainder(value, base)) / base, base);
        printf(&curr);
    }
    return 0;

}

int main(){
    int source = 0;
    int target = 0;
    printf("Enter source base:\n");
    scanf("%d", &source);
    printf("Enter target base:\n");
    scanf("%d", &target);
    if(illegal_base(source) || illegal_base(target)){
        printf("Illegal source or target base\n");
        return 1;
    }
    printf("Enter value to be converted:\n");
    int value = get_input(source);
    convert_and_print(value, target);
    printf("\n");

}