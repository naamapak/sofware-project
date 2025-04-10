# include <stdio.h>
# include <stdlib.h>
# include <math.h>


int _get_base(int kind) {
/* Gets a base as input from the user. Using 's' for source base
and 't' for target base in our program. Returns 0 for invalid base */
    int base;
    switch(kind){
        case 0:
        printf("enter source base\n");
        break;
        case 1:
        scanf("enter target base\n");
    }
    scanf("%d", &base);
    if((base < 2) || (base > 16)){
        printf("invalid base: %d", &base);
        return 0;
    }
    else {
        return base;
    }
}

/*
char* compute_base(int source, int target, char* value){
    char[] digits = "0123456789ABCDEF"
    int base10 = 0;
} */
/* How do I not use arrays? Just malloc everything? */