#include "markov_chain.h"
#include <stdio.h>
#include <string.h>

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}


/**
* Check if data_ptr is in database. If so, return the Node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the data to look for
 * @return Pointer to the Node wrapping given data, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr){
    // make sure the linked list is not empty
    if(markov_chain->database->first == NULL){
        return NULL;
    }
    Node* currentNode = markov_chain->database->first;
    while(currentNode != NULL && !strcmp(markov_chain->database->first->data->data, data_ptr)){
        currentNode = currentNode->next;
    }
    return currentNode;
}


/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the data to look for
 * @return Node wrapping given data_ptr in given chain's database,
 * returns NULL in case of memory allocation failure.
 */
Node* add_to_database(MarkovChain *markov_chain, char *data_ptr){
    // first let's start by checking if the newLinkedListNode exists in the chain already, if so return it.
    Node* newLinkedListNode = get_node_from_database(markov_chain, data_ptr);
    if(newLinkedListNode != NULL){
        return newLinkedListNode;
    }

    // create the new markov node with the data
    MarkovNode* newMarkovNode = (MarkovNode*) malloc(sizeof(MarkovNode));
    if(newMarkovNode == NULL){
        return NULL;
    }
    newMarkovNode->data = data_ptr; // TODO assuming we don't need to do strdupe

    // try to add a new node but detect if failed
    if(add(markov_chain->database, newMarkovNode) == 1){
        free(newMarkovNode); // since the node failed we now have to free the markov node
        return NULL;
    }

    // since the linked list adds at the end we can just return the last node
    return markov_chain->database->last;
}


/**
 * Add the second markov_node to the frequency list of the first markov_node.
 * If already in list, update it's occurrence frequency value.
 * @param first_node
 * @param second_node
 * @return success/failure: 0 if the process was successful, 1 if in
 * case of allocation error.
 */
int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node){
    // check if the frequency list is empty, if it is we just add the node directly
    if(first_node->frequency_list == NULL){
        first_node->frequency_list = (MarkovNodeFrequency*) malloc(sizeof (MarkovNodeFrequency));
        if(first_node->frequency_list == NULL){
            return 1;
        }
        first_node->frequency_list->markov_node = second_node;
        first_node->frequency_list->frequency = 1;
        first_node->frequency_list->next_frequency_node = NULL; // since this is the first node there is no next node
        first_node->last_frequency_node = first_node->frequency_list; // since the list was empty the first node is = to the last node
    }
    // now we are left with 2 options: either it exists and needs updating or does not exist and needs to be added
    else {
        // since the list is not empty lets see if the second node is already in the frequency list
        MarkovNodeFrequency *frequency_node = find_markov_node_frequency(first_node, second_node);


        // if the second node was not found in the frequency list we need to add it to the end of the list
        if (frequency_node == NULL) {
            // create the new node that needs to be added
            MarkovNodeFrequency *new_frequency_node = (MarkovNodeFrequency *) malloc(sizeof(MarkovNodeFrequency));
            if (new_frequency_node == NULL) {
                return 1;
            }

            // the last node needs to point to this new node
            first_node->last_frequency_node->next_frequency_node = new_frequency_node;
            // update the last node to be this new node
            first_node->last_frequency_node = new_frequency_node;

            // update this new node
            new_frequency_node->next_frequency_node = NULL;
            new_frequency_node->markov_node = second_node;
            new_frequency_node->frequency = 1;
        } else {
            frequency_node->frequency++;
        }
    }
    return 0;
}

// this is a helper function to see if a frequency node is in the frequency list of a markov node
// IMPORTANT: this assumes the list is not empty (added that check in the add_node_to_frequency_list function)
MarkovNodeFrequency* find_markov_node_frequency(MarkovNode *first_node, MarkovNode *second_node){
    MarkovNodeFrequency* frequency_node_iterator = first_node->frequency_list;

    while(frequency_node_iterator != NULL && frequency_node_iterator->markov_node != second_node) {
        frequency_node_iterator = frequency_node_iterator->next_frequency_node;
    }
    // we will return NULL if it is not found and the frequency node that matches if it is found
    return frequency_node_iterator;
}


/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_database(MarkovChain ** ptr_chain){
    // first let's grab hold of the linked list
    LinkedList* linkedList = (*ptr_chain)->database;
    // next we grab the first node
    Node* currentNode = linkedList->first;
    while(currentNode != NULL){
        // free the markov node
        free_markov_node(currentNode->data);
        // save this node for removal
        Node* node_to_remove = currentNode;
        // point to the next node
        currentNode = currentNode->next;
        // remove the current node (the one we saved)
        free(node_to_remove);
    }
    // free the linked list
    free(linkedList);
    // free the markov chain
    free(*ptr_chain);
}

// helper function for the free database
void free_markov_node(MarkovNode* markov_node){
    // free the frequency list first
    free_frequency_list(markov_node->frequency_list);
    // free the data
    free(markov_node->data);
    // free the node itself (in theory at this point we freed all the pointer the markov node has).
    free(markov_node);
}

// helper function for the free database (called by free_markov_node function)
void free_frequency_list(MarkovNodeFrequency* frequency_list){
    // point to the first frequency node
    MarkovNodeFrequency* frequency_list_iterator = frequency_list;
    while (frequency_list_iterator != NULL){
        // put aside the current one to remove
        MarkovNodeFrequency* frequency_to_remove = frequency_list_iterator;
        // make sure im pointing to the next item
        frequency_list_iterator = frequency_list_iterator->next_frequency_node;
        // remove the current one (the one we saved)
        free(frequency_to_remove);
    }
}


/**
 * Get one random MarkovNode from the given markov_chain's database.
 * @param markov_chain
 * @return the random MarkovNode
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain){
    // grab a random number between 1 and linked list size (including)
    int element_number = get_random_number(markov_chain->database->size) + 1;
    // point at the first node
    Node* current_node = markov_chain->database->first;
    // traverse random number of nodes
    for (int i = 1; i <= element_number; ++i, current_node = current_node->next);
    // return the markovNode inside the markov_chain
    return current_node->data;
}

/**
 * Choose randomly the next MarkovNode, depend on it's occurrence frequency.
 * @param cur_markov_node current MarkovNode
 * @return the next random MarkovNode
 */
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node){
    MarkovNodeFrequency* current_frequency = cur_markov_node->frequency_list;

    // TODO not sure I need this I think I land out at NULL anyway in which case remove the comment in the bottom
    if(current_frequency == NULL)
        return NULL;

    int accumulated_frequency = 0;

    // let's find out the range of numbers from which to choose the random number
    while(current_frequency != NULL){
        accumulated_frequency += current_frequency->frequency;
        current_frequency = current_frequency->next_frequency_node;
    }

    // let's grab a random number from the range we made;
    int random_number = get_random_number(accumulated_frequency);

    // grab and return the node with the accumulation of the random number
    accumulated_frequency = 0;
    while(current_frequency != NULL){
        accumulated_frequency += current_frequency->frequency;
        if(random_number < accumulated_frequency)
            return current_frequency->markov_node;
        current_frequency = current_frequency->next_frequency_node;
    }

    // CODE SHOULD NEVER REACH THIS POINT!!!
    return NULL;
}

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence must have at least 2 words in it.
 * @param first_node markov_node to start with
 * @param  max_length maximum length of chain to generate
 */
 // TODO what does it mean that the sentence must have at least 2 words in it?
 //  does that mean that I return an error if I was given a node that ends with a dot?
 //  michael in the name of Nadav says that I deal with this when I call the function
void generate_tweet(MarkovNode *first_node, int max_length){
     MarkovNode* current_node = first_node;
     for (int i = 0; i < max_length; ++i, current_node = get_next_random_node(current_node)) {
         if(current_node->data[strlen(current_node->data)-1] == '.'){
             printf("%s", current_node->data);
             break;
         }
         else{
             printf("%s", current_node->data);
             printf(" ");
         }
     }
}

