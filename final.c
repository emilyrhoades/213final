#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
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

//struct containing information to be passed to threads
typedef struct threadInfo{
    char* charArr;
    //will be the indices within the periods array
    int start;
    int end;
    //contains indices of overy sentence ending punctuation in the text
    int *perLoc;
    //word everything will be compared to
    char* compWord;
    //desired words
    char* desiredWords;
} threadInfo_t;



/**
 * Determines if a word appears within a given piece of text
 *
 * \param word  The word that is being searched for in the text
 * \param charArr The complete input text to be analysed
 * \param start The index within the complete text of the start of the sentence to be searched
 * \param end The index within the complete text that marks the end of the sentence to be searched
 * \return Returns 0 if the word is not in the sentence and 1 if it is
 */
int wordAppears(char* word, char* charArr, int start, int end){
    size_t wordLength = strlen(word);
    //1 if word is found, 0 if not
    int contains = 0;
    //keep track of location within sentence after start location
    int sentLoc = 0;
    //iterate through sentence until it could not contain word
    for(int i=start; i<(end-1); i++){
        //if the word has been found, exit
        if(contains != 0){
            break;
        }
        //if a letter doesn't match, begin looking again
        //continue until all letters match for length of word
        for(int j=0; j<wordLength; j++){
            if(word[j] != charArr[i+sentLoc]){
                sentLoc = 0;
                break;
            }
            //if word is found, make sure that it is the end of a word
            //it iis considered the end of a word if it is followed by anything other than a letter
            int nextChar = charArr[i + sentLoc + 1]; 
            if((sentLoc == (wordLength-1)) && ((nextChar < 65) || (nextChar > 122))){
                contains = 1;
                break;
            }
            sentLoc++;
        }
    }
    return contains;
}

/**
 * Thread function: runs simultaneously to find counts of co-oucurring main word and desired words
 *
 * \param infoStruct Struct containing a char* of the conplete desired text, 
 *                   start and end indices for the period array which denote which section a thread will analyze,
 *                   the array containing period locations (or other sentence ending punctuation), the main word,
 *                   and the words to compare it to
 */

void* locate(void* infoStruct){
    threadInfo_t* info = (threadInfo_t*) infoStruct;
    char* words = info->desiredWords;
    //to hold counts of words
    char word[MAXWORD];
    char* wordStr = malloc(sizeof(char)*MAXWORD);
    if(wordStr== NULL){
        printf("Memory allocation error\n");
        exit(1);
    }    
    //keep track of location within individual words and the array
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
            //check that the word is not the same as the main word
            //if word appears increment association count and count of total found associations
            if(wordAppears(word, info->charArr, sentStart, sentEnd) == 1){
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
    free(wordStr);
    return NULL;
}

int main(int argc, char** argv){
    //many things must be declared ass arrays for the purpose of adding to thm dynamically and then 
    //converted into pointers so they can be passed into a struct
    //initialize lock  
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    //to hold text file to be analyzed
    FILE *ptr;
    //read in desired file
    char filename[FILENAMESIZE];
    //store chars
    char *charArr = NULL;
    charArr = malloc(ARRSIZE);
    if(charArr == NULL){
        printf("Memory allocation error\n");
        return 1;
    }
    //hold chars as they are read in
    char ch;
    //array to keep track of of how full arrays are
    //index 0 - chars from file
    //index 1 - periods
    int arrFull[2] = {0,0};
    //array to keep track of total in arrays, same indices as above
    //start periods at 1 becasue first sentence will begin at 0, not loc of first period
    int totalNum[2] = {0,1};
    //keep track of period (or other sentence ending punctuation) locations
    int *periods = NULL;
    periods = malloc(LOCARRSIZE*sizeof(int));
    if(periods == NULL){
        printf("Memory allocation error\n");
        return 1;
    }    
    //set start location to 0
    periods[0] = 0;
    //the word to be compares to others
    char* compWord = malloc(MAXWORD);
    if(compWord== NULL){
        printf("Memory allocation error\n");
        return 1;
    }   
    //the words to compare to the main word
    char desiredWordsArray[MAXWORD*MAXWORDCOMP];
    //convert so it can be used in the struct
    char* desiredWords = malloc(MAXWORD*MAXWORDCOMP);
    if(desiredWords == NULL){
        printf("Memory allocation error\n");
        return 1;
    }   
    //store number of threads, adjustable uncase number of sentences less then threads
    int numThreads = MAXTHREAD;
    //get file name
    printf("Please enter the text file you would like to analyze in .txt format:\n");
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
            charArr = realloc(charArr, (totalNum[0] + ARRSIZE));
            if(charArr == NULL){
                printf("Memory allocation error\n");
                return 1;
             }           
            arrFull[0] = 0;
        }
        //set first element to 0 so it starts at beginning of sentence
        if((ch == '.') || (ch == '!') || (ch == '?')|| (ch == ';')){
            //check if arry is full
            if(arrFull[1] == (LOCARRSIZE - 1)){
                periods = realloc(periods, ((LOCARRSIZE + totalNum[1])*sizeof(int)));
                if(periods == NULL){
                    printf("Memory allocation error\n");
                    return 1;
                }       
                arrFull[1] = 0;
            }
            //set location in periods array to the currentt location within the file
            periods[totalNum[1]] = totalNum[0];
            totalNum[1]++;
            arrFull[1]++;
        }
        //add char to array and increment, convert to lower so csae doen't matter
        charArr[totalNum[0]] = tolower(ch);
        arrFull[0]++;
        totalNum[0]++;
    }
    if(fclose(ptr) == EOF){
        printf("Error closing file\n");
        return 1;
    }
    printf("\n");

    //check is number of sentences is less then threads
    if((totalNum[1]-1)< numThreads){
        numThreads = (totalNum[1] - 1);
    }

    //read in list of desired words
    printf("Please enter the a file with words you would like to compare in .txt format\n");
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
        desiredWordsArray[desWordArrLoc] = tolower(ch);
        desWordArrLoc++;
    }
    //add null terminator to create string
    desiredWordsArray[desWordArrLoc] = '\0';
    //copy to struct usable format
    strcpy(desiredWords, desiredWordsArray);
    if(fclose(dWords)==EOF){
        printf("Error closing file\n");
        return 1;
    }

    printf("\n");

    //read in comp words
    char tempCompWord[MAXWORD];
    printf("Enter the word you would like to compare to others\n");
    printf("It can only be %d characters long and must contain only characters\n", MAXWORD);
    printf("If multiple words are entered, only the first will be considered\n");
    scanf("%s", tempCompWord);
    //convert to lowercase
    for(int i = 0; i< MAXWORD; i++){
         tempCompWord[i] = tolower(tempCompWord[i]);
    }
    //copy to actual storage for struct
    strcpy(compWord, tempCompWord);

    
    //total number of sections divided by max threads, will truncate
    //subtract one becasue totalNum is incremented after last addition
    int section = (int)((totalNum[1] - 1)/numThreads);

    //declare info structs for the threads
    threadInfo_t* infoStructs[numThreads];
    for(int i = 0; i < numThreads; i++){
        infoStructs[i] = (threadInfo_t*)malloc(sizeof(threadInfo_t));
        if(infoStructs[i] == NULL){
            printf("Memory allocation error\n");
            return 1;
        }       
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
     infoStructs[numThreads - 1]->end = (totalNum[1]-2);

     pthread_t threads[numThreads]; 

    //create threads
    for(int i = 0; i < numThreads; i++){
        if(pthread_create(&threads[i], NULL, locate, (void*)infoStructs[i])){
            perror("pthread_create failed\n");
            exit(EXIT_FAILURE);
        } 
    }

    //join threads
    for(int j = 0; j < numThreads; j++){
        if(pthread_join(threads[j], NULL)){
            perror("pthread_join failed");
            exit(EXIT_FAILURE);
        }
        free(infoStructs[j]);
    }
    pthread_mutex_destroy(&lock);

    free(charArr);
    free(periods);

    //create file with owner read/write permissions and group read permissions
    //code taken from files demo
    int fd = open("results.txt", O_RDWR | O_CREAT | O_APPEND, 0640);
    if(fd == -1){
        printf("Error opening file\n");
        return 1;
    }

    //account for size words, spaces, new lines, and values
    char str[(MAXWORD + 12)*MAXWORDCOMP];
    sprintf(str, "The selected file is '%s'\n", filename);
    sprintf(str + strlen(str), "The selected word is '%s'\n", compWord);
    free(compWord);

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
            //calculate proportion
            double prop = 0;
            if(numFoundWords[wordsProc] != 0){
                prop = (double)numFoundWords[wordsProc]/totalWords;
            }
            //add word, count of word, and proportion of total word appearences to string
            sprintf(str + strlen(str), "%s:   %d   %0.2f\n", word, numFoundWords[wordsProc], prop);
            wordsProc++;
            for(int j = 0; j < wordLoc; j++){
                word[j] = '\0';
            }
            //empty string
            wordLoc = 0;
    }
    free(desiredWords);
    sprintf(str + strlen(str), "\n\n");
    //write results to file
    write(fd, str, strlen(str));
    if(close(fd) == -1){
        printf("Error closing file\n");
        return 1;
    }
    printf("\nResults have been stored in results.txt\n");
    return 0;
}