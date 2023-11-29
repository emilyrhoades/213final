//CHECK MAKEFILE
//CLOSE FILE
//DOES SCANF NEED A CHECK
//DOES THE SIZE OF REALLOC ADD TO EXISTING SIZE OR REDEFINE

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

//max size for file input name
#define FILENAMESIZE 300
//max size for array to hold sentence
#define SENTENCESIZE 3

int main(int argc, char** argv){
    //to hold text file to ba analyzed
    FILE *ptr;
    //read in desired file
    char filename[FILENAMESIZE];
    //store sentence
    char *sent = NULL;
    sent = malloc(SENTENCESIZE);
    //hold chars as they are read in
    char ch;
    //keep track of number of chars to expand array
    int arrChars = 0;
    //keep track of total chars in array
    int numChars = 0;

    //DOES THIS NEED A CHECK
    scanf("%s",filename);
    //
    if ((ptr = fopen(filename,"r")) == NULL){
       printf("Error! opening file\n");
       exit(1);
    }
    //read in chars stop when file is complete
    while(1){
        //check file complete
        if((ch = fgetc(ptr)) == EOF){
            break;
        }
        //check if array is full
        if(arrChars == (SENTENCESIZE - 1)){
            //resize if it is full
            sent = realloc(sent, (numChars + SENTENCESIZE));
            arrChars = 0;
        }
        //add to array
        sent[numChars] = ch;
        numChars++;
        arrChars++;
    }
    for(int i=0;i<numChars;i++){
        printf("%c",sent[i]);
    }
    printf("\n");
    return 0;
}