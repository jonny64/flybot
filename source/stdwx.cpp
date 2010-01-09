#include "stdwx.h"

int random(int max)
{
    if (max <= 0)
        return 0;
        
    int result = 0;
    int i = max;
    while (RAND_MAX < i)
    {
        srand(time(NULL));
        result += rand();
        i -= RAND_MAX;
    }
    srand(time(NULL));
    result += rand() % i;
    
    return result;
}
