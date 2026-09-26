#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// represents one word in a word ladder
typedef struct WordNode_struct {
    char* myWord;
    struct WordNode_struct* next; 
} WordNode;

// represents one word ladder in list of ladders
typedef struct LadderNode_struct {
    WordNode* topWord;
    struct LadderNode_struct* next; 
} LadderNode;


// counts how many words in the file match the given word size
int countWordsOfLength(char* filename, int wordSize) { 
    
    FILE* file = fopen(filename, "r");
    
    //if failed return -1
    if (file == NULL){
        return -1;
    }

    char temp[100];
    int count = 0;

    // read words one by one
    while (fscanf(file, "%s", temp) == 1) {
        if (strlen(temp) == wordSize) {
            count++;                   
        }
    }

    fclose(file);
    return count; 
}

// builds array containing all words that match given word size
bool buildWordArray(char* filename, char** words, int numWords, int wordSize) { 
    FILE* file = fopen(filename, "r");
    
    // if failed
    if (file == NULL){
        return false;
    }

    char temp[100];
    int index = 0;

    // read words one by one
    while (fscanf(file, "%s", temp) == 1) {
        if (strlen(temp) == wordSize) {
            // add matching word to array
            strcpy(words[index], temp);
            index++;                          
        }
    }

    fclose(file);
    return (index == numWords);
}

// uses binary search to find a word in the array
int findWord(char** words, char* aWord, int loInd, int hiInd) {
    while (loInd <= hiInd) {
        // middle index
        int mid = (loInd + hiInd) / 2;

        // compare middle word to target word
        int cmp = strcmp(words[mid], aWord);

        // found
        if (cmp == 0) {
            return mid;
        // search right half
        } else if (cmp < 0) {
            loInd = mid + 1;
        // search left half
        } else {
            hiInd = mid - 1;
        }
    }
    // not found
    return -1;
}

// frees all memory allocated for the word array
void freeWords(char** words, int numWords) {
    // free each word
    for (int i = 0; i < numWords; i++) {
        free(words[i]);
    }

    // free the array of pointers
    free(words);
}

// counts how many characters differ between two strings
int strCmpCnt(char* str1, char* str2) {
    int count = 0;
    int i = 0;

    // compare characters at each position
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            count++;
        }
        i++;
    }

    // count any remaining characters
    count += strlen(str1 + i);
    count += strlen(str2 + i);

    return count;
}

// finds the index of the first character that differs between two strings
int strCmpInd(char* str1, char* str2) {
    int i = 0;

    // compare characters at each position
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            // index of first difference
            return i; 
        }
        i++;
    }

    // if one string ends earlier
    if (str1[i] != str2[i]) {
        return i;
    }

    // identical strings
    return -1;
}

// adds a new word to the front of a ladder
void insertWordAtFront(WordNode** ladder, char* newWord) {
    // new word node
    WordNode* newNode = (WordNode*)malloc(sizeof(WordNode));

    newNode->myWord = newWord;   
    
    // point to old head
    newNode->next = *ladder;

    // new node into new head
    *ladder = newNode;
}

// counts number of words in a ladder
int getLadderHeight(WordNode* ladder) {
    int count = 0;

    while (ladder != NULL) {
        count++;
        ladder = ladder->next;
    }

    return count;
}

// checks whether a word is valid for the next ladder step
bool checkForValidWord(char** words, int numWords, int wordSize, WordNode* ladder, char* aWord) {

    // user entered "DONE" - valid, top (#1) priority
    if (strcmp(aWord, "DONE") == 0) {
        printf("Stopping with an incomplete word ladder...\n");
        return true;
    }
    
    // user entered a word that is too long/short - invalid, priority #2
    if (strlen(aWord) != wordSize) {
        printf("Entered word does NOT have the correct length. Try again...\n");
        return false;
    }

    // user entered a word that is not found in the dictionary - invalid, priority #3
    if (findWord(words, aWord, 0, numWords - 1) == -1) {
        printf("Entered word NOT in dictionary. Try again...\n");
        return false;
    }
        
    // invalid if words do not differ by exactly one character
    if (strCmpCnt(ladder->myWord, aWord) != 1) {
        printf("Entered word is NOT a one-character change from the previous word. Try again...\n");
        return false;
    }

    // user entered a word a valid word - valid, priority #5 (default)
    printf("Entered word is valid and will be added to the word ladder.\n");
    return true;
}

// checks if ladder is complete
bool isLadderComplete(WordNode* ladder, char* finalWord) {
    // empty ladder not complete
    if (ladder == NULL) {
        return false;
    }

    // top word must match final word
    return (strcmp(ladder->myWord, finalWord) == 0);
}

// creates a copy of a word ladder
WordNode* copyLadder(WordNode* ladder) {
    if (ladder == NULL) return NULL;

    WordNode* newHead = NULL;
    WordNode* tail = NULL;

    while (ladder != NULL) {
        // make new node
        WordNode* newNode = (WordNode*)malloc(sizeof(WordNode));
        // just point to same word
        newNode->myWord = ladder->myWord; 
        newNode->next = NULL;

        // first node
        if (newHead == NULL) {
            newHead = newNode;
            tail = newNode;
        } else {
            // attach to end
            tail->next = newNode;
            tail = newNode;
        }

        ladder = ladder->next;
    }

    return newHead;
}

// free all nodes in word ladder
void freeLadder(WordNode* ladder) {
    WordNode* temp;

    while (ladder != NULL) {
        temp = ladder;
        ladder = ladder->next;
        free(temp);
    }
}

// displays an incomplete word ladder
void displayIncompleteLadder(WordNode* ladder) {
    // top dots
    printf("  ...\n");
    printf("  ...\n");
    printf("  ...\n");

    // print ladder top to bottom
    WordNode* curr = ladder;
    while (curr != NULL) {
        printf("  %s\n", curr->myWord);
        curr = curr->next;
    }
}

// displays a completed word ladder and marks the changed character between each word
void displayCompleteLadder(WordNode* ladder) {
    WordNode* curr = ladder;

    while (curr != NULL) {
        // print word
        printf("  %s\n", curr->myWord);

        // print ^ line between this word and next word
        if (curr->next != NULL) {
            int ind = strCmpInd(curr->myWord, curr->next->myWord);
            int len = strlen(curr->myWord);

            // left indent
            printf("  "); 

            // spaces before ^
            for (int i = 0; i < ind; i++) {
                printf(" ");
            }

            printf("^");

            // spaces after ^
            for (int i = ind + 1; i < len; i++) {
                printf(" ");
            }

            printf("\n");
        }

        curr = curr->next;
    }
}

// adds a word ladder to the end of the ladder list
void insertLadderAtBack(LadderNode** list, WordNode* newLadder) {
    LadderNode* newNode = (LadderNode*)malloc(sizeof(LadderNode));

    newNode->topWord = newLadder;
    newNode->next = NULL;

    // empty list
    if (*list == NULL) {
        *list = newNode;
        return;
    }

    // go to last node
    LadderNode* curr = *list;
    while (curr->next != NULL) {
        curr = curr->next;
    }

    // attach at end
    curr->next = newNode;
}

// removes and returns the first ladder from the ladder list
WordNode* popLadderFromFront(LadderNode** list) {
    // empty list
    if (*list == NULL) {
        return NULL;
    }

    // save front node
    LadderNode* temp = *list;
    WordNode* frontLadder = temp->topWord;

    // move head forward
    *list = temp->next;

    // free ladder node only
    free(temp);

    // return popped ladder
    return frontLadder;
}

// frees all ladders and nodes in the ladder list
void freeLadderList(LadderNode* myList) {
    LadderNode* temp;

    while (myList != NULL) {
        temp = myList;               
        myList = myList->next;        

        freeLadder(temp->topWord);    
        free(temp);                   
    }
}

// finds the shortest word ladder from the start word to the final word
WordNode* findShortestWordLadder(   char** words, 
                                    bool* usedWord, 
                                    int numWords, 
                                    int wordSize, 
                                    char* startWord, 
                                    char* finalWord ) {

    LadderNode* myList = NULL;
    WordNode* firstLadder = NULL;

    // start ladder with startWord
    int startInd = findWord(words, startWord, 0, numWords - 1);
    insertWordAtFront(&firstLadder, words[startInd]);
    insertLadderAtBack(&myList, firstLadder);
    // mark used
    usedWord[startInd] = true;

    // keep going while ladders exist
    while (myList != NULL) {
        WordNode* currLadder = popLadderFromFront(&myList);
        char* currWord = currLadder->myWord;

        // try every one-letter change
        for (int i = 0; i < wordSize; i++) {
            char tempWord[30];
            strcpy(tempWord, currWord);

            for (char ch = 'a'; ch <= 'z'; ch++) {
                tempWord[i] = ch;

                // skip same word
                if (strcmp(tempWord, currWord) == 0) {
                    continue;
                }

                int wordInd = findWord(words, tempWord, 0, numWords - 1);

                // valid unused dictionary word
                if (wordInd != -1 && !usedWord[wordInd]) {
                    // mark used
                    usedWord[wordInd] = true;

                    // if final word found
                    if (strcmp(words[wordInd], finalWord) == 0) {
                        insertWordAtFront(&currLadder, words[wordInd]);
                        // free remaining ladders
                        freeLadderList(myList);   
                        // shortest ladder found
                        return currLadder;        
                    }

                    // make new ladder and add to back
                    WordNode* nextLadder = copyLadder(currLadder);
                    insertWordAtFront(&nextLadder, words[wordInd]);
                    insertLadderAtBack(&myList, nextLadder);
                }
            }
        }

        // done with this ladder
        freeLadder(currLadder);
    }

    // no ladder found
    return NULL;
}

// randomly set a word from the dictionary word array
void setWordRand(char** words, int numWords, int wordSize, char* aWord) {
    printf("  Picking a random word for you...\n");
    strcpy(aWord,words[rand()%numWords]);
    printf("  Your word is: %s\n",aWord);
}

// Provided by the course instructor.
// interactive user-input to set a word;
//  ensures the word is in the dictionary word array
void setWord(char** words, int numWords, int wordSize, char* aWord) {
    bool valid = false;
    if (strcmp(aWord,"RAND") != 0) printf("  Enter a %d-letter word (enter RAND for a random word): ", wordSize);
    int count = 0;
    while (!valid) {
        if (strcmp(aWord,"RAND") != 0) scanf("%s",aWord);
        count++;
        valid = (strlen(aWord) == wordSize);
        if (valid) {
            int wordInd = findWord(words, aWord, 0, numWords-1);
            if (wordInd < 0) {
                valid = false;
                printf("    Entered word %s is not in the dictionary.\n",aWord);
                printf("  Enter a %d-letter word (enter RAND for a random word): ", wordSize);
            }
        } else if (strcmp(aWord,"RAND") != 0) {
            printf("    Entered word %s is not a valid %d-letter word.\n",aWord,wordSize);
            printf("  Enter a %d-letter word (enter RAND for a random word): ", wordSize);
        }
        if (!valid && (count >= 5 || strcmp(aWord,"RAND") == 0)) { //too many tries, picking random word
            setWordRand(words, numWords, wordSize, aWord);
            valid = true;
        }
    }
}

// Provided by the course instructor.
// helpful debugging function to print a single Ladder
void printLadder(WordNode* ladder) {
    WordNode* currNode = ladder;
    while (currNode != NULL) {
        printf("\t\t\t%s\n",currNode->myWord);
        currNode = currNode->next;
    }
}

// Provided by the course instructor.
// helpful debugging function to print the entire list of Ladders
void printList(LadderNode* list) {
    printf("\n");
    printf("Printing the full list of ladders:\n");
    LadderNode* currList = list;
    while (currList != NULL) {
        printf("  Printing a ladder:\n");
        printLadder(currList->topWord);
        currList = currList->next;
    }
    printf("\n");
}

// Provided by course instructor.
int main(int argc, char* argv[]) {

    printf("\n");
    printf("--------------------------------------------\n");
    printf("Welcome to the CS 211 Word Ladder Generator!\n");
    printf("--------------------------------------------\n\n");
    

    //-------------- \/\/\/ TOP OF PROGRAM SETTINGS \/\/\/ --------------
    //--- COMMAND-LINE ARGUMENTS AND/OR INTERACTIVE USER-INPUT \/\/\/ ---

    
    // default values for program parameters that may be set with
    //  command-line arguments
    int wordSize = -2114430;
    char dict[100] = "notAfile";
    char startWord[30] = "notAword";
    char finalWord[30] = "notValid";
    bool playMode = false;
    
    printf("\nProcessing command-line arguments...\n");

    //-------------------------------------------------------------------
    // command-line arguments:
    //  [-n wordLen] = sets word length for word ladder;
    //                 if wordLen is not a valid input
    //                 (cannot be less than 2 or greater than 20),
    //                 or missing from command-line arguments,
    //                 then let user set it using interactive user input
    // [-d dictFile] = sets dictionary file;
    //                 if dictFile is invalid (file not found) or
    //                 missing from command-line arguments, then let
    //                 user set it using interactive user input
    // [-s startWord] = sets the starting word;
    //                  if startWord is invalid
    //                  (not in dictionary or incorrect length) or
    //                  missing from command-line arguments, then let
    //                  user set it using interactive user input
    // [-f finalWord] = sets the final word;
    //                  if finalWord is invalid
    //                  (not in dictionary or incorrect length) or
    //                  missing from command-line arguments, then let
    //                  user set it using interactive user input
    // [-p playModeSwitch] = turns playMode ON if playModeSwitch is "ON"
    //                       or leaves playMode OFF if playModeSwitch is
    //                       anything else, including "OFF"
    //-------------------------------------------------------------------

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i],"-n") == 0 && argc > i+1) {
            wordSize = atoi(argv[i+1]);
            ++i;
        } else if (strcmp(argv[i],"-d") == 0 && argc > i+1) {
            strcpy(dict, argv[i+1]);
            ++i;
        } else if (strcmp(argv[i],"-s") == 0 && argc > i+1) {
            strcpy(startWord, argv[i+1]);
            ++i;
        } else if (strcmp(argv[i],"-f") == 0 && argc > i+1) {
            strcpy(finalWord, argv[i+1]);
            ++i;
        } else if (strcmp(argv[i],"-p") == 0 && argc > i+1) {
            playMode = (strcmp(argv[i+1],"ON") == 0);
            ++i;
        }
    }
    
    srand((int)time(0));
    
    // set word length using interactive user-input
    //  if wordSize == -2114430, it was NOT set with command-line args
    while (wordSize < 2 || wordSize > 20) {
        if (wordSize != -2114430) printf("Invalid word size for the ladder: %d\n", wordSize);
        printf("Enter the word size for the ladder: ");
        scanf("%d",&wordSize);
        printf("\n");
    }

    printf("This program is a word ladder building game and a solver that\n");
    printf("finds the shortest possible ");
    printf("word ladder between two %d-letter words.\n\n",wordSize);
    
    // interactive user-input to set the dictionary file;
    //  check that file exists; if not, user enters another file
    //  if file exists, count #words of desired length [wordSize];
    //  if dict == "notAfile", it was NOT set with command-line args
    int numWords = countWordsOfLength(dict,wordSize);
    while (numWords < 0) {
        if (strcmp(dict, "notAfile") != 0) {
            printf("  Dictionary %s not found...\n",dict);
        }
        printf("Enter filename for dictionary: ");
        scanf("%s", dict);
        printf("\n");
        numWords = countWordsOfLength(dict,wordSize);
    }
    
    // end program if file does not have at least two words of desired length
    if (numWords < 2) {
        printf("  Dictionary %s contains insufficient %d-letter words...\n",dict,wordSize);
        printf("Terminating program...\n");
        return -1;
    }
    
    // allocate heap memory for the word array; only words with desired length
    char** words = (char**)malloc(numWords*sizeof(char*));
    for (int i = 0; i < numWords; ++i) {
        words[i] = (char*)malloc((wordSize+1)*sizeof(char));
    }
    
    // [usedWord] bool array has same size as word array [words];
    //  all elements initialized to [false];
    //  later, usedWord[i] will be set to [true] whenever
    //      words[i] is added to ANY partial word ladder;
    //      before adding words[i] to another word ladder,
    //      check for previous usage with usedWord[i]
    bool* usedWord = (bool*)malloc(numWords*sizeof(bool));
    for (int i = 0; i < numWords; ++i) {
        usedWord[i] = false;
    }
    
    // build word array (only words with desired length) from dictionary file
    printf("Building array of %d-letter words... ", wordSize);
    bool status = buildWordArray(dict,words,numWords,wordSize);
    if (!status) {
        printf("  ERROR in building word array.\n");
        printf("  File not found or incorrect number of %d-letter words.\n",wordSize);
        printf("Terminating program...\n");
        return -1;
    }
    printf("Done!\n\n");

    // set the two ends of the word ladder using interactive user-input
    //  make sure start and final words are in the word array,
    //  have the correct length (implicit by checking word array), AND
    //  that the two words are not the same
    // start/final words may have already been set using command-line arguments
    // the start/final word can also be set to "RAND" resulting in a random
    //  assignment from any element of the words array
    if (strcmp(startWord,"RAND")==0) {
        printf("Setting the start word randomly...\n");
        setWordRand(words, numWords, wordSize, startWord);
    } else if (findWord(words, startWord,0, numWords-1) < 0 || strlen(startWord) != wordSize) {
        if (strcmp(startWord,"notAword")==0) {
            printf("Setting the start %d-letter word... \n", wordSize);
        } else {
            printf("Invalid start word %s. Resetting the start %d-letter word... \n", startWord, wordSize);
        }
        setWord(words, numWords, wordSize, startWord);
    }
    printf("\n");
    
    if (strcmp(finalWord,"RAND")==0) {
        printf("Setting the final word randomly...\n");
        setWordRand(words, numWords, wordSize, finalWord);
    } else if (findWord(words, finalWord,0, numWords-1) < 0 || strlen(finalWord) != wordSize) {
        if (strcmp(finalWord,"notValid")==0) {
            printf("Setting the final %d-letter word... \n", wordSize);
        } else {
            printf("Invalid final word %s. Resetting the final %d-letter word... \n", finalWord, wordSize);
        }
        setWord(words, numWords, wordSize, finalWord);
    }
    while (strcmp(finalWord,startWord) == 0) {
        printf("  The final word cannot be the same as the start word (%s).\n",startWord);
        printf("Setting the final %d-letter word... \n", wordSize);
        setWord(words, numWords, wordSize, finalWord);
    }
    printf("\n");
    
    //----------------- ^^^ END OF PROGRAM SETTINGS ^^^ -----------------
    
    
    //-------------- \/\/\/ TOP OF GAME PLAY SECTION \/\/\/ --------------
    
    if (!playMode) {
        printf("\n");
        printf("---------------------------------------------\n");
        printf("No Word Ladder Builder Game; Play Mode is OFF\n");
        printf("---------------------------------------------\n");
        printf("\n");
    } else {
        printf("\n");
        printf("-----------------------------------------------\n");
        printf("Welcome to the CS 211 Word Ladder Builder Game!\n");
        printf("-----------------------------------------------\n");
        printf("\n");

        printf("Your goal is to make a word ladder between two ");
        printf("%d-letter words: \n  %s -> %s\n\n",wordSize, startWord,finalWord);
        
        WordNode* userLadder = NULL;
        int ladderHeight = 0; // initially, the ladder is empty
        int startInd = findWord(words, startWord, 0, numWords-1);
        insertWordAtFront(&userLadder, words[startInd]);
        ladderHeight++; // Now, the ladder has a start word
            
        char aWord[30] = "XYZ";
        printf("\n");
        

//------------------------------ student written Code ---------------------------------
        // continue until the user stops or completes the ladder
        while (strcmp(aWord, "DONE") != 0 && !isLadderComplete(userLadder, finalWord)) { 
            printf("The goal is to reach the final word: %s\n",finalWord);
            printf("The ladder is currently: \n");
            displayIncompleteLadder(userLadder);
            printf("Current ladder height: %d\n",ladderHeight);
            printf("Enter the next word (or DONE to stop): ");
            scanf("%s",aWord);
            printf("\n");
            
            // keep asking until a valid next word is entered
            while (!checkForValidWord(words, numWords, wordSize, userLadder, aWord)) {
                printf("Enter another word (or DONE to stop): ");
                scanf("%s",aWord);
                printf("\n");
            }

            // add valid word to the ladder unless the user stops
            if (strcmp(aWord,"DONE") != 0) {
                int currInd = findWord(words, aWord, 0, numWords-1);
                insertWordAtFront(&userLadder, words[currInd]);
                ladderHeight++;
            }
            printf("\n");
        }

        // display the completed or incomplete ladder
        if (isLadderComplete(userLadder, finalWord)) {
            printf("Word Ladder complete!\n\n");
            displayCompleteLadder(userLadder);
            printf("\nWord Ladder height = %d\n\n", ladderHeight);
            printf("Can you find a shorter Word Ladder next time??? \n");
        } else {
            printf("The final Word Ladder is incomplete:\n");
            displayIncompleteLadder(userLadder);
            printf("Word Ladder height = %d\n\n", ladderHeight);
            printf("Can you complete the Word Ladder next time??? \n");
        }
        freeLadder(userLadder);
    }
    
//---------------------- end of student written code ----------------------------------    
    
// Provided by instructor

    printf("\n\n");
    printf("-----------------------------------------\n");
    printf("Welcome to the CS 211 Word Ladder Solver!\n");
    printf("-----------------------------------------\n");
    printf("\n");
    

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // OPTIONAL EXTENTION TO FIND LONGEST WORD LADDER:
    //  program must end with finding the shortest word ladder
    //  (& the associated print statements); if you choose to
    //  extend your program to find the longest word ladder,
    //  put the long word ladder algorithm (& the associated
    //  print statements) BEFORE the short word ladder algorithm
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    
    // run the algorithm to find the shortest word ladder
    WordNode* myLadder = findShortestWordLadder(words, usedWord, numWords, wordSize, startWord, finalWord);

    // display word ladder and its height if one was found
    if (myLadder == NULL) {
        printf("There is no possible word ladder from %s to %s\n",startWord,finalWord);
    } else {
        printf("Shortest Word Ladder found!\n\n");
        displayCompleteLadder(myLadder);
        //printLadder(myLadder);
    }
    printf("\nWord Ladder height = %d\n\n",getLadderHeight(myLadder));    
    
//-----------------------Student written code -------------------------------------
    
    // free the dynamically allocated memory
    freeLadder(myLadder);
    freeWords(words,numWords);
    free(usedWord);
    
//-------------- end of student written code -------------------------------

    
    return 0;
}
