#include <stdio.h>

void mySwitch(int*, int*);
void disp(int, int);

int main() {
    int i = 10;
    int j = 20;
    disp(i,j);
    mySwitch(&i, &j);
    disp(i,j);
}

void mySwitch(int* i, int* j) {
    int tmp = *i;
    *i = *j;
    *j = tmp;
}

void disp(int i, int j) {
    printf("%d %d\n", i, j);
}