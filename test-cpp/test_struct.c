#include <stdio.h>
int read() {
    int r;
    scanf("%d", &r);
    return r;
}
void write(int r) {
    printf("%d\n", r);
}

struct asd {
    int a;
    int b[123];
} asd1;
struct node {
    struct node * prev;
    struct node * next;
    struct asd qqq;
} noderoot;

int main() {
    struct node b;
    struct node {
        struct node * prev;
        struct node * next;
        struct asd qqq;
    } a;
//    b = noderoot;
    a = b;
    a.qqq.b[0] = 123;
    a.prev = &a;
    a.next = a.prev;
    struct node * ptr = &a;
    if (a.prev) {
        a.prev->qqq.a++;
    } else a.prev->qqq.b[3]++;
    ptr->qqq.a = 123;
    ptr->qqq.b[0] = a.qqq.b[0];
    ptr->qqq.b[2] = read();
    write(ptr->qqq.a);
}