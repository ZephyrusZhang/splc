#include <stdio.h>
int read() {
    int r;
    scanf("%d", &r);
    return r;
}
void write(int r) {
    printf("%d\n", r);
}

int sqr(int i1){
    return i1*i1;
}
int cbc(int i2){
    return i2*i2*i2;
}

int main(){
    int a, r;
    a = read();
    write(a);
    r = sqr(a);
    write(r);
    r = cbc(a);
    write(r);
    return 0;
}
