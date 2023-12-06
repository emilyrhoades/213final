//INCLUDE README
//CHECK MAKEFILE
//CHECK REQUIRED LIBRARIES
//CLOSE FILE
//FREE
//DOES SCANF NEED A CHECK
//CHECK WHICH LIBRARIES ARE NEEDED
//-1 CONDTION IN CONTAINS
//CHECKS FOR PTHREAD
//FREE THINGS
//WHAT IF DES AND COMP WORD IS THE SAME
//PLURALS
//DOES strcpy NEED MALLOC
//LESS SENTENCES THAN 4
//MUST END IN PUNCTUATION
//CAPITALIZATION

//num words as input
//if it comes after another word


//options:
//create an array of max sizes and curr sizes

//to do:
//contains to consider spaces

//Ask Leah
//Is it enough
//Makefile
//does scanf need a check


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
//size for array to hold all chars
#define ARRSIZE 300
//size for location markers
#define LOCARRSIZE 50
//number of threads
#define MAXTHREAD 4
//max length of word
#define MAXWORD 30
//max number of words to compare
#define MAXWORDCOMP 100

//store counts of found words
int numFoundWords[MAXWORDCOMP];
//global lock
pthread_mutex_t lock;
//keep track of total number of found words
int totalWords = 0;


typedef struct threadInfo{
    char* charArr;
    //will be the indices within the periods array
    int start;
    int end;
    int *perLoc;
    //word everything will be compared to
    char* compWord;
    //desired words
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
            //if word is found, make sure that it is the end of a word
            int nextChar = sent[i + sentLoc + 1]; 
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
    char* words = info->desiredWords;
    //to hold counts of words
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
                if(k == strlen(info->desiredWords)){
                    break;
                }
            }
            //convert to string with nothing extra at end
            word[wordLoc] = '\0';
            strcpy(wordStr, word);
            //if word appears increment association count
            if(wordAppears(word, info->charArr, sentStart, sentEnd) == 1){
                //((threadInfo_t*) infoStruct)->numFoundWords[wordArrLoc]++;
                pthread_mutex_lock(&lock);
                numFoundWords[wordArrLoc]++;
                totalWords++;
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
    periods = malloc(LOCARRSIZE*sizeof(int));
    //set start location to 0
    periods[0] = 0;
    //the word to be compares to others
    char* compWord = malloc(MAXWORD);
    //the words to compare to the main word
    char desiredWordsArray[MAXWORD*MAXWORDCOMP];
    //convert so it can be used in the struct
    char* desiredWords = malloc(MAXWORD*MAXWORDCOMP);
    //get file name
    printf("Please enter the text file you would like to analyze:\n");
    scanf("%s",filename);
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
            //printf("total num: %d, arrsize: %d\n", totalNum[0], ARRSIZE);
            charArr = realloc(charArr, (totalNum[0] + ARRSIZE));
            arrFull[0] = 0;
        }
        //set first element to 0 so it starts at beginning of sentence
        if((ch == '.') || (ch == '!') || (ch == '?')){
            //check if arry is full
            if(arrFull[1] == (LOCARRSIZE - 1)){
                printf("new: %lu,\ncur size: %lu\narrful: %d\n", ((LOCARRSIZE + totalNum[1])*sizeof(int)), sizeof(periods), arrFull[1]);
                periods = realloc(periods, ((LOCARRSIZE + totalNum[1])*sizeof(int)));
                arrFull[1] = 0;
                //allocate space
                //int *tmp = realloc(periods, (totalNum[1] + LOCARRSIZE));
                //check that it was assigned
                // if(tmp == NULL){
                //     printf("error with allocation\n");
                //     return 0;
                // }
                // else{
                //     //assign periods
                //     periods = tmp;
                //     arrFull[1] = 0;
                // }
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
    fclose(ptr);
    printf("\n");

    //read in list of desired words
    printf("Please enter the a file with words you would like to compare\n");
    printf("A word can be at most %d characters long and at most %d words can be entered\n", MAXWORD, MAXWORDCOMP);
    printf("The words should be formatted with one word on each line and no punctuation in between\n");

    
    FILE *dWords;
    //read in desired file
    char filenameWords[FILENAMESIZE];
    scanf("%s",filenameWords);
    if ((dWords = fopen(filenameWords,"r")) == NULL){
       printf("Error! opening file\n");
       exit(1);
    }

    //keep track of loc withing array
    int desWordArrLoc = 0;
    while((ch = fgetc(dWords)) != EOF){
        desiredWordsArray[desWordArrLoc] = ch;
        desWordArrLoc++;
    }
    //add null terminator to create string
    desiredWordsArray[desWordArrLoc] = '\0';
    //copy to struct usable format
    strcpy(desiredWords, desiredWordsArray);
    fclose(dWords);
    printf("\n");

    //read in comp words
    char tempCompWord[MAXWORD];
    printf("Enter the word you would like to compare to others\n");
    printf("It can only be %d characters long and must contain only characters\n", MAXWORD);
    scanf("%s", tempCompWord);
    //copy to actual storage for struct
    strcpy(compWord, tempCompWord);

    
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
        //until end of first section, remove one to avoid overlap
        infoStructs[i]->end = ((i + 1) * section)-1;
        infoStructs[i]->perLoc = periods;
        infoStructs[i]->desiredWords = desiredWords;
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
    printf("%d\n", totalWords);

    

    // printf("found1: %d\n", numFoundWords[0]);
    // printf("found2: %d\n", numFoundWords[1]);
    // printf("found3: %d\n", numFoundWords[2]);

    //create file with owner read/write permissions and group read permissions
    //code taken from files demo
    int fd = open("results.txt", O_RDWR | O_CREAT | O_APPEND, 0640);

    //account for size words, spaces, new lines, and values
    char str[(MAXWORD + 12)*MAXWORDCOMP];
    sprintf(str, "The selected word is '%s'\n", compWord);

    //keep track of location withing current word
    int wordLoc = 0;
    //store current word
    char word[MAXWORD];
    //keep track of num words processed
    int wordsProc = 0;

    //iterate through all desired words
    for(int k = 0; k < strlen(desiredWords); k++){
            //isolate each word
            //desired words are seperated by new line
            while((desiredWords[k] != '\n')){
                word[wordLoc] = desiredWords[k];
                wordLoc++;
                k++;
                //check if at end
                if(k == strlen(desiredWords)){
                    break;
                }
            }
            //add word, count of word, and proportion of total word appearences to string
            sprintf(str + strlen(str), "%s:   %d   %0.2f\n", word, numFoundWords[wordsProc], ((double)numFoundWords[wordsProc]/totalWords));
            wordsProc++;
            for(int j = 0; j < wordLoc; j++){
                word[j] = '\0';
            }
            //empty string
            wordLoc = 0;
    }
    //write results to file
    write(fd, str, strlen(str));
    close(fd);
    
    
    // locate(infoStructs[0]);
    // locate(infoStructs[1]); 
    // locate(infoStructs[2]);
    // locate(infoStructs[3]);

    //printf("start: %c, end: %c\n",charArr[periods[infoStructs[0]->start]+3], 
    //charArr[periods[infoStructs[0]->end]-1]);

    return 0;
}