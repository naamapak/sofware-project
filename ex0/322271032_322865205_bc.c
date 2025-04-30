#include <stdio.h>

int read_number(int source_base);
void print_in_base(int number, int target_base);

int main() {
    int source_base;
    int target_base;
    int number;

    /*get the inputs*/
    printf("enter the source base: ");
    scanf("%d", &source_base);

    printf("enter the target base: ");
    scanf("%d", &target_base);

    /*read the number and if valid convert to base 10*/
    number = read_number(source_base);
    if (number == -1) {
        return 1; /* error occurred*/
    }
    
    /*now print the actual number in the target base*/
    printf("The number in base %d is: ", target_base);
    if (number == 0)
        printf("0");
    else
        print_in_base(number, target_base);
    printf("\n");
    return 0;
}

int read_number(int source_base){ /*reads onlly the number and converts it to base 10*/

    int value = 0; /* will hold the final number in base 10*/
    int digit; /* to store the value of each char while calculating*/ 
    char c; /* to read one char at a time*/

    c = getchar(); /*reads the first char, if empty c='\n' immidietly */
    while (c !='\n' && c != EOF){ /*read if not in the end of the line and not the end of the file */
        c = getchar();
    }
     printf("enter a number in base %d: ", source_base);
    
     c = getchar(); /*read the first real input*/

     while (c != '\n' && c != EOF) {
         /*convert char to numeric value*/
         if (c >= '0' && c <= '9') {
             digit = c - '0';
         }
         else if (c >= 'a' && c <= 'f') {
             digit = c - 'a' + 10;
         }
         else {
             printf("invalid character '%c'\n", c);
             return -1;  /*error*/
         }
     
         /*check if digit is valid for source base*/
         if (digit >= source_base) {
             printf("digit '%c' is not valid in base %d\n", c, source_base);
             return -1;  /*error*/
         }
     
         /*update the value*/
         value = value * source_base + digit;
     
         /*read next character*/
         c = getchar();
     }
     return value;
}

void print_in_base(int number, int target_base){
    int digit;
    /*we will use recursion beacause we can't use arrays so will divide the number by the target base each time and output imm*/
    /*base case*/
    if (number == 0){
        return;
    }
    /*if not dividing and calling again recursivly*/
    print_in_base(number / target_base, target_base);

    digit = number % target_base;
    /*now if the digit is a number(below 10) just print it and if the digit is above print the char in the base related too*/
    if (digit < 10)
        printf("%d", digit);
    
    else 
    printf("%c", 'a' + (digit-10));
}