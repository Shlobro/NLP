#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "markov_chain.h"

// error messages
#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define MAX_TWEET_LEN 20

#define DELIMITERS " \n\t\r"



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

    // get the filePath and make sure it's valid
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

    // let's get those words from the file and fill the database
    MarkovChain * markovChain = (MarkovChain*) malloc(sizeof(MarkovChain));
    if (markovChain == NULL) {;
        return EXIT_FAILURE;
    }
    markovChain->database = (LinkedList*) malloc(sizeof(LinkedList));
    if (markovChain->database == NULL) {
        free(markovChain);
        return EXIT_FAILURE;
    }
    markovChain->database->first = NULL;
    markovChain->database->last = NULL;
    markovChain->database->size = 0;


    if(!fill_database(file, num_of_words_to_read, markovChain)){
        // TODO free the memory we had a failure somewhere
        return EXIT_FAILURE; // TODO ask teacher if this is what im supposed to return
    }

    // Print out the tweets
    for (int i = 0; i < num_of_tweets; ++i) {
        MarkovNode * first_markov_node_in_new_tweet = get_first_random_node(markovChain);
        generate_tweet(first_markov_node_in_new_tweet, MAX_TWEET_LEN);
    }

    // Hopefully this deals with all the allocated memory all in one go
    free_database(&markovChain);

    // close the file
    fclose(file);

    return EXIT_SUCCESS;
}




int fill_database(FILE *fp, int words_to_read, MarkovChain* markovChain){
    int words_read = 0;
    // buffer to store each line (the size is 1000 since we assume the line will not be more than that)
    char buffer[1000];


    // line reading loop
    while(fgets(buffer, sizeof(buffer), fp) != NULL && words_read <= words_to_read){
        // we don't need to add the first word to any frequency list
        int skip_frequency_list_stage = 1;

        Node* current_node;
        Node* next_node;

        // loop will exit as soon as the line runs out
        char* token = strtok(buffer, DELIMITERS);
        while(token != NULL && words_read < words_to_read){
            next_node = add_to_database(markovChain, token);
            // check if adding the new node was successful (allocation success check) (success also means it existed)
            if(next_node == NULL){
                return 1;
            }
            if(skip_frequency_list_stage)
                skip_frequency_list_stage = 0;
            // TODO really this should be just else without the condition but clion was worried. need to decide if I leave this
            else if(current_node != NULL)
                add_node_to_frequency_list(current_node->data, next_node->data);
            current_node = next_node;
            if(token[strlen(token) - 1] != '.'){
                skip_frequency_list_stage = 1;
            }
            words_read++;
            token = strtok(NULL, DELIMITERS);
        }
    }
    return 0;
}


