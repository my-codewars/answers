#include <stdlib.h>
#include <string.h>

char *add(const char *a, const char *b) { 
    int la = strlen(a), lb = strlen(b), lr = (la > lb ? la : lb) + 1;
    char *r = calloc(lr + 1, 1);
    int c = 0,i;
    
    for(i = lr - 1, la--, lb-- ; i >= 0 ;la--, lb--, i--){
        int sum = (la < 0 ? '0' : a[la]) - '0' + (lb < 0 ? '0' : b[lb]) - '0' + c; c = 0;
        if(sum > 9) c = 1, sum -= 10;
        r[i] = sum + '0';
    }
    r = *r == '0' ? memmove(r, r + 1, lr) : r;
    return r;
}
