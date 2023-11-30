//CHECK MAKEFILE
//CLOSE FILE
//DOES SCANF NEED A CHECK
//DOES THE SIZE OF REALLOC ADD TO EXISTING SIZE OR REDEFINE
//CHECK WHICH LIBRARIES ARE NEEDED
//size_t TO int
//-1 CONDTION IN CONTAINS
//CHECKS FOR PTHREAD
//FREE THINGS

//num words as input


//options:
//create an array of max sizes and curr sizes

//to do:
//contains to consider spaces

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

//max size for file input name
#define FILENAMESIZE 300
//max size for array to hold all chars
#define ARRSIZE 3
//size for location markers
#define LOCARRSIZE 50
//number of threads
#define MAXTHRED 4


typedef struct threadInfo{
    char* charArr;
    int start;
    int end;
    int *perLoc;
    int numDesWords;
    char* desiredWords;
} threadInfo_t;


//return 0 if the word appears in the sentence and 1 if it does

int wordAppears(char* word, char* sent, int start, int end){
    //size_t sentLength = strlen(sent);
    size_t wordLength = strlen(word);
    //location of start of word if found, 0 if not
    int contains = 0;
    //keep track of location within sentence after start location
    int sentLoc = 0;
    int numWords = 0;
    //iterate through sentence until is could not contain word
    for(int i=start; i<(end-1); i++){
        if(contains != 0){
            break;
        }
        for(int j=0; j<wordLength; j++){
            if(word[j] != sent[i+sentLoc]){
                sentLoc = 0;
                break;
            }
            //MAKE SURE TO TEST THIS
            //if word is found, make sure that it is the end of a word
            if(sentLoc == (wordLength-1) && (sent[i + sentLoc + 1] == ' ')){
                contains = (i);
                break;
            }
            sentLoc++;
        }
    }
    return contains;
}

void* locate(void* infoStruct){
    threadInfo_t* info = (threadInfo_t*) infoStruct;
    int numDesWords = info->numDesWords;
    int* numFoundWords;
    char* words = info->desiredWords;
    //to hold counts of words
    numFoundWords = malloc(numDesWords);
    char word[strlen(words)];
    int wordLoc = 0;
    int wordArrLoc = 0;
    for(int i = 0; i < strlen(info->desiredWords); i++){
        //ADD OTHER PUNC
        while((words[i] != ' ') && (words[i] != '.')){
            word[wordLoc] = words[i];
            wordLoc++;
            i++;
        }
        word[wordLoc] = '\0';
        numFoundWords[wordArrLoc] = wordAppears(word, info->charArr, info->start, info->end);
        for(int j = 0; j < wordLoc; j++){
            word[j] = '\0';
        }
        wordLoc = 0;
        wordArrLoc++;

    }
    return (void*)numFoundWords;
}

int main(int argc, char** argv){
    //to hold text file to ba analyzed
    FILE *ptr;
    //read in desired file
    char filename[FILENAMESIZE];
    //store chars
    char *charArr = NULL;
    charArr = malloc(ARRSIZE);
    //hold chars as they are read in
    char ch;
    //array to keep track of of how full arrays are
    //index 0 - chars from file
    //index 1 - periods
    int arrFull[2] = {0,0};
    //array to keep track of total in arrays, same indices as above
    int totalNum[2] = {0,0};
    //keep track of period locations
    int *periods = NULL;
    periods = malloc(LOCARRSIZE);

    int compCount = 0;
 

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
        if(arrFull[0] == (ARRSIZE - 1)){
            //resize if it is full
            charArr = realloc(charArr, (totalNum[0] + ARRSIZE));
            arrFull[0] = 0;
        }

        if(ch == '.'){
            //check if arry is full
            if(arrFull[1] == (LOCARRSIZE - 1)){
                periods = realloc(periods, totalNum[1]);
                arrFull[1] = 0;
            }
            //set location in periods array to the currentt location within the file
            periods[totalNum[1]] = totalNum[0];
            totalNum[1]++;
            arrFull[1]++;
        }
        //add char to array and increment
        charArr[totalNum[0]] = ch;
        arrFull[0]++;
        totalNum[0]++;
    }
    for(int i=0;i<totalNum[0];i++){
        printf("%c",charArr[i]);
    }

    printf("\n");
    //CHANGE THIS
    char* desiredWords = "this is";
    threadInfo_t* info = (threadInfo_t*)malloc(sizeof(threadInfo_t));
    info->charArr = charArr;
    info->desiredWords = desiredWords;
    info->start = 0;
    info->end = 20;
    info->perLoc = periods;
    info->numDesWords = 1;

    int* numFoundWords = (int*)locate((void*)info);

    printf("found1: %d\n", numFoundWords[0]);
    printf("found2: %d\n", numFoundWords[1]);

    //pthread_t threads[MAX_THREAD]; 
 
    // Creating 4 threads 
    //for (int i = 0; i < MAX_THREAD; i++) 
 //       pthread_create(&threads[i], NULL, sum_array, (void*)charArr); 
 
    // joining 4 threads i.e. waiting for all 4 threads to complete 
   // for (int i = 0; i < MAX_THREAD; i++) 
     //   pthread_join(threads[i], NULL); 
 
    // adding sum of all 4 parts 
    //int total_sum = 0; 
    //for (int i = 0; i < MAX_THREAD; i++) 
      //  total_sum += sum[i]; 

        //needs to be passed:
        // -char array
        // -desired words
        // -start location
        // -end location
        // -period array
    
        //to count oucurrences of a word:
        // -char array
        // -desired word
        // -start
        // -end

        //divide number of sentences and start from beginning of sentence
    //char* testWord = "this";
    //int testContains = wordAppears(testWord, charArr);
    //printf("contains: %d\n", testContains);
    return 0;
}