#include "prime.h"
#include <math.h>

// 1 prime
// 0 non-prime
// -1 x <2
int is_prime(const int a){
    if (a <2) {
        return -1;
    }
    if(a <4) {
        return 1;
    }
    if( (a %2) == 0) return 0; 
    for(int i =5; i <= floor(sqrt((double)a)); i +=2) {
        if(a % i == 0){
            return 0; 
        }
    }
    return 1;
}

int next_prime(int a){
    while (is_prime(a) != 1){
        a ++ ;
    }
    return a;
}