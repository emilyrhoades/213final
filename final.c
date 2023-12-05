//CHECK MAKEFILE
//CHECK REQUIRED LIBRARIES
//CLOSE FILE
//FREE
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
//LESS SENTENCES THAN 4
//MUST END IN PUNCTUATION
//ERROR FOR REALLOC

//num words as input
//if it comes after another word


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
#include <pthread.h>

//max size for file input name
#define FILENAMESIZE 300
//max size for array to hold all chars
#define ARRSIZE 300
//size for location markers
#define LOCARRSIZE 50
//number of threads
#define MAXTHREAD 4
//max length of word
#define MAXWORD 30
//max number of words to compare
#define MAXWORDCOMP 100

int numFoundWords[MAXWORDCOMP];
pthread_mutex_t lock;


typedef struct threadInfo{
    char* charArr;
    //will be the indices within the periods array
    int start;
    int end;
    int *perLoc;
    //number of desired words
    int numDesWords;
    //word everything will be compared to
    char* compWord;
    //desired words
    char* desiredWords;
    //array to hold association counts
    //int* numFoundWords;
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
            int nextChar = sent[i + sentLoc + 1] == ' ';
            if((sentLoc == (wordLength-1)) && ((nextChar < 65) || (nextChar > 122))){
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
    char* words = info->desiredWords;
    //to hold counts of words
    //numFoundWords = malloc(numDesWords);
    //hold array of word counts
    char word[MAXWORD];
    char* wordStr = malloc(sizeof(char)*MAXWORD);
    int wordLoc = 0;
    int wordArrLoc = 0;
    //hold start and end of desired sentence
    int sentStart = 0;
    int sentEnd = 0;
    char* compWord = info->compWord;
    char* charArr = info->charArr;
    //check for all desired words
    //IS END-1 CORRECT?
    for(int i = info->start; i <= info->end; i++){
        wordArrLoc = 0;
        //get charArr position for beginning and end of sentence
        sentStart = info->perLoc[i];
        sentEnd = info->perLoc[i+1];
        //check if compare word is in sentence, if it is not move to next
        if (wordAppears(compWord, charArr, sentStart, sentEnd) == 0){
            continue;
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
                //((threadInfo_t*) infoStruct)->numFoundWords[wordArrLoc]++;
                pthread_mutex_lock(&lock);
                numFoundWords[wordArrLoc]++;
                pthread_mutex_unlock(&lock);
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
    return NULL;
}

int main(int argc, char** argv){
    //initialize lock  
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
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
    //int compCount = 0;



    //CHANGE THIS

    
    int numDesWords = 2;
    //the word to be compares to others
    char* compWord = "friend";
    //the words to compare to the main word
    char* desiredWords = "dear\ntest\n";
    //char desiredWordsArr[numDesWords][MAXWORD];
    //memset(desiredWordsArr, 0, (numDesWords*MAXWORD*sizeof(char)));
    //strcpy(desiredWordsArr[0], "this");
    //array to store association counts
    //int* numFoundWords;
    //numFoundWords = malloc(numDesWords);


 

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
        if((ch == '.') || (ch == '!') || (ch == '?')){
            //check if arry is full
            if(arrFull[1] == (LOCARRSIZE - 1)){
                periods = realloc(periods, (totalNum[1] + LOCARRSIZE));
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
    // for(int i=0;i<totalNum[0];i++){
    //     printf("%c",charArr[i]);
    // }

    // printf("\n");
    
    //total number of sections divided by max threads, will truncate
    //subtract one becasue totalNum is incremented after last addition
    int section = (int)((totalNum[1] - 1)/MAXTHREAD);

    //declare four info structs for the threads
    threadInfo_t* infoStructs[MAXTHREAD];
    for(int i = 0; i < MAXTHREAD; i++){
        infoStructs[i] = (threadInfo_t*)malloc(sizeof(threadInfo_t));
        infoStructs[i]->charArr = charArr;
        //beginning
        infoStructs[i]->start = i * section;
        //until end of first section, remove
        infoStructs[i]->end = ((i + 1) * section)-1;
        infoStructs[i]->perLoc = periods;
        infoStructs[i]->desiredWords = desiredWords;
        infoStructs[i]->numDesWords = numDesWords;
        infoStructs[i]->compWord = compWord;
    }

    //set the end value of the final struct to the end to account for num of sentences not divisible by 4
    //subtract two to account for last incement and program will process next in array
     infoStructs[MAXTHREAD - 1]->end = (totalNum[1]-2);

     pthread_t threads[MAXTHREAD]; 

    //create threads
    for(int i = 0; i < MAXTHREAD; i++){
        pthread_create(&threads[i], NULL, locate, (void*)infoStructs[i]); 
    }

    //join threads
    for(int j = 0; j < MAXTHREAD; j++){
        pthread_join(threads[j], NULL);
    }
    pthread_mutex_destroy(&lock);
    // locate(infoStructs[0]);
    // locate(infoStructs[1]); 
    // locate(infoStructs[2]);
    // locate(infoStructs[3]);

    //printf("start: %c, end: %c\n",charArr[periods[infoStructs[0]->start]+3], 
    //charArr[periods[infoStructs[0]->end]-1]);

    //ISSUE: 3 AT VERY END, 2 ONE SENTENCE BEOFRE END

   // printf("check\n");

    ///pthread_create(&threads[1], NULL, locate, (void*)info2); 
 
    // joining 4 threads i.e. waiting for all 4 threads to complete 
   // for (int i = 0; i < MAX_THREAD; i++) 
    //pthread_join(threads[0], NULL); 
    //pthread_join(threads[1], NULL);

    printf("found1: %d\n", numFoundWords[0]);
    printf("found2: %d\n", numFoundWords[1]);

    // //declare info object and fill fields
    // threadInfo_t* info1 = (threadInfo_t*)malloc(sizeof(threadInfo_t));
    // info1->charArr = charArr;
    // //beginning
    // info1->start = 0;
    // //until end of first section
    // info1->end = section;
    // info1->perLoc = periods;
    // info1->desiredWords = desiredWords;
    // info1->numDesWords = numDesWords;
    // info1->compWord = compWord;
    // //info1->numFoundWords = numFoundWords;

    // threadInfo_t* info2 = (threadInfo_t*)malloc(sizeof(threadInfo_t));
    // info2->charArr = charArr;
    // //beginning of second section
    // info2->start = section;
    // //end of section
    // info2->end = section*2;
    // info2->perLoc = periods;
    // info2->desiredWords = desiredWords;
    // info2->numDesWords = numDesWords;
    // info2->compWord = compWord;
    // //info2->numFoundWords = numFoundWords;

    // threadInfo_t* info2 = (threadInfo_t*)malloc(sizeof(threadInfo_t));
    // info2->charArr = charArr;
    // //beginning of second section
    // info2->start = section;
    // //end of section
    // info2->end = section*2;
    // info2->perLoc = periods;
    // info2->desiredWords = desiredWords;
    // info2->numDesWords = numDesWords;
    // info2->compWord = compWord;

    //locate((void*)info1);


    //info2->end = periods[totalNum[1]-1];



 
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