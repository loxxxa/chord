#include <stdio.h>
#include <limits.h>

int main()
{
    int n = htonl(1234567890);
    int net_n = hton(n);

    fprintf(stderr, "%s", const (void *)(&net_n));
    return 0;
}
