#include <stdio.h>
int read() {
    int r;
    scanf("%d", &r);
    return r;
}
void write(int r) {
    printf("%d\n", r);
}

struct Student
{
    int ID;
    int score;
};

int main()
{
    struct Student s1, s2;
    s1.ID = 1;
    s1.score = 70;
    s2.ID = 2;
    s2.score = 90;
    write(s2.ID);
    write(s1.score);
    return 0;
}