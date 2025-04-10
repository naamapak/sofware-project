# include <stdio.h>
# include <math.h>

int _get_base(char* kind) {
/* Gets a base as input from the user. Using 's' for source base
and 't' for target base in our program. Returns 0 for invalid base */
    int base;
    switch(kind){
        case 's':
        scanf("enter source base %d\n", &base)
        break;
        case 't':
        scanf("enter target base %d\n", &base)
    }
    if((base < 2) || (base > 16)){
        printf("invalid base: %d", &base);
        return 0;
    }
    else {
        return base;
    }
}

/* How do I not use arrays? Just malloc everything? */