#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void encrypt(int theme) {
    FILE *file = fopen("conf.txt", "w");
    
    fprintf(file, "%d", theme);
    fclose(file);

    system( "openssl aes-256-cbc -a -pbkdf2 -in conf.txt -out conf.txt.enc -pass pass:esgi");

    
    system("del conf.txt");
}

int main(int argc, char **argv) {
    printf("What theme do you want ?\n0: Clear \n1: Dark\n");
    int theme =0;
    scanf("%d",&theme);
    encrypt(theme);


    return 0;
}
