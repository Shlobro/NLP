#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "markov_chain.h"

// error messages
#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"

/**
 * this function is just cause I'm lazy and I didn't want to type printf every time
 *
 * @param error_message
 * @return
 */
int error(char error_message[]);

/**
 * fills the database (the markovChain) with words from a given file
 *
 * @param fp A pointer to the FILE that contains the words
 * @param words_to_read
 * @param markovChain
 * @return
 */
int fill_database(FILE *fp, int words_to_read, MarkovChain* markovChain);


int main(int argc, char *argv[]){
    if(argc < 4 || argc > 5){
        return error(NUM_ARGS_ERROR);
    }

    // convert the seed into a number. no need to check if valid (assumed)
    char* endptr;
    unsigned int seed = (unsigned int) strtol(argv[1], &endptr, 10);

    // convert the number of strings into a number. no need to check if valid (assumed)
    int num_of_tweets = (int) strtol(argv[2], &endptr, 10);

    // TODO need to check the file path is valid
    // get the filePath
    char* file_path = argv[3];
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        return error(FILE_PATH_ERROR);
    }

    // default will be the max unless argument says otherwise
    int num_of_words_to_read = INT_MAX;
    if(argc == 5) {
        // convert the number of strings into a number. no need to check if valid (assumed)
        num_of_words_to_read = (int) strtol(argv[4], &endptr, 10);
    }

    MarkovChain * markovChain = (MarkovChain*) malloc(sizeof(MarkovChain));
    if(!fill_database(file, num_of_words_to_read, markovChain)){
        // TODO free the memory we had a failure somewhere
        return 1;
    }





    // close the file
    fclose(file);

    return EXIT_SUCCESS;
}


int error(char error_message[]){
    printf("%s", error_message);
    return EXIT_FAILURE;
}

int fill_database(FILE *fp, int words_to_read, MarkovChain* markovChain){
    // buffer to store each line (the size is 1000 since we assume the line will not be more than that)
    char buffer[1000];

    //TODO make sure that we can assume that every single line has at least 2 words in it!!!! (Nadav is dumb)

    // line reading loop
    while(fgets(buffer, sizeof(buffer), fp) != NULL){
        // we don't need to add the first word to any frequency list
        int skip_frequency_list_stage = 1;

        Node* current_node;
        Node* next_node;

        // loop will exit as soon as the line runs out
        char* token = strtok(buffer, DELIMITERS);
        while(token != NULL){
            next_node = add_to_database(markovChain, token);
            // check if adding the new node was successful (success also means it existed)
            if(next_node == NULL){
                //TODO failed to allocate. PANIC!!!!
            }
            if(skip_frequency_list_stage)
                skip_frequency_list_stage = 0;
            else
                add_node_to_frequency_list(current_node->data, next_node->data);
            current_node = next_node;
            if(token[strlen(token) - 1 != '.']){
                skip_frequency_list_stage = 1;
            }
            token = strtok(NULL, DELIMITERS);
        }
    }
}


