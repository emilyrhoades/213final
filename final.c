//CHECK MAKEFILE
//CLOSE FILE
//DOES SCANF NEED A CHECK
//DOES THE SIZE OF REALLOC ADD TO EXISTING SIZE OR REDEFINE
//CHECK WHICH LIBRARIES ARE NEEDED
//size_t TO int
//-1 CONDTION IN CONTAINS
//CHECKS FOR PTHREAD
//FREE THINGS
//HOW EFFICIENT DOES IT NEED TO BE
//NOTE MAX WORDS SIZE
//WHAT IS DES AND COMP WORD IS THE SAME
//PLURALS
//DOES strcpy NEED MALLOC

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
//max length of word
#define MAXWORD 30


typedef struct threadInfo{
    char* charArr;
    //will be the indices within the periods array
    int start;
    int end;
    int *perLoc;
    int numDesWords;
    char* compWord;
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
        //if a letter doesn't match, begin looking again
        for(int j=0; j<wordLength; j++){
            if(word[j] != sent[i+sentLoc]){
                sentLoc = 0;
                break;
            }
            //MAKE SURE TO TEST THIS
            //if word is found, make sure that it is the end of a word
            //COULD BE COMMA, SEMICOLON ETC.
            if(sentLoc == (wordLength-1) && (sent[i + sentLoc + 1] == ' ')){
                contains = 1;
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
    //hold array of word counts
    char word[MAXWORD];
    char* wordStr = malloc(sizeof(char)*MAXWORD);
    int wordLoc = 0;
    int wordArrLoc = 0;
    //hold start and end of desired sentence
    int sentStart = 0;
    int sentEnd = 0;
    //check for all desired words
    //IS END-1 CORRECT?
    for(int i = info->start; i < info->end; i++){
        wordArrLoc = 0;
        //get charArr position for beginning and end of sentence
        sentStart = info->perLoc[i];
        sentEnd = info->perLoc[i+1];
        //check if compare word is in sentence, if it is not move to next
        if (wordAppears(info->compWord, info->charArr, sentStart, sentEnd) == 0){
            break;
        }
        for(int k = 0; k < strlen(info->desiredWords); k++){
            //isolate each word
            //desired words are seperated by new line
            while((words[k] != '\n')){
                word[wordLoc] = words[k];
                wordLoc++;
                k++;
            }
            //convert to string with nothing extra at end
            word[wordLoc] = '\0';
            strcpy(wordStr, word);
            //if word appears increment association count
            if(wordAppears(word, info->charArr, sentStart, sentEnd) == 1){
                numFoundWords[wordArrLoc]++;
            }
            //empty word array
            for(int j = 0; j < wordLoc; j++){
                word[j] = '\0';
            }
            //empty string
            memset(wordStr, 0, strlen(wordStr));
            wordLoc = 0;
            wordArrLoc++;

        }
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
    //start periods at 1 becasue first sentence will begin at 0, not loc of first period
    int totalNum[2] = {0,1};
    //keep track of period locations
    int *periods = NULL;
    periods = malloc(LOCARRSIZE);
    //set start location to 0
    periods[0] = 0;
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
        //set first element to 0 so it starts at beginning of sentence
        //periods[0] = 0;
        //totalNum[1]++;
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
    int numDesWords = 1;
    //the word to be compares to others
    char* compWord = "friend";
    //the words to compare to the main word
    char* desiredWords = "dear\ntest\n";
    char desiredWordsArr[numDesWords][MAXWORD];
    memset(desiredWordsArr, 0, (numDesWords*MAXWORD*sizeof(char)));
    strcpy(desiredWordsArr[0], "this");

    //declare info object and fill fields
    threadInfo_t* info = (threadInfo_t*)malloc(sizeof(threadInfo_t));
    info->charArr = charArr;
    info->start = 0;
    info->end = 3;
    info->perLoc = periods;
    info->desiredWords = desiredWords;
    info->numDesWords = numDesWords;
    info->compWord = compWord;

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