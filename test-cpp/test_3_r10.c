#include <stdio.h>
int read() {
    int r;
    scanf("%d", &r);
    return r;
}
void write(int r) {
    printf("%d\n", r);
}

int main()
{
    int n;
    n = read();
    if (n > 0) write(1);
    else if (n < 0) write (-1);
    else write(0);
    return 0;
}
