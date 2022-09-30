#include "autocomplete.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

double power(double base, double exponent){
    int i;

    if(exponent == 0){
        return 1;
    };
    double result = base;
    for(i = 1; i < exponent; i++){
        result = result * base;
    };
    return result;
}

int number_of_elements(char *filename){
//this function counts the number of terms in a given file
//the goal is to assign an array with the correct size in the read_in_terms function
    FILE *fcount = fopen(filename, "r");
    char c[200];
    int counter = 0;

    while( fgets(c, sizeof c, fcount) != NULL){ //check if we are at the end of the file
        counter = counter + 1;
    }
    if(feof(fcount)){
        //fclose(fcount);
        //  =("count %d \n", counter);
        return counter - 1;
    }else{
        //fclose(fcount);
        return 1;
    }

}

double string_weight_to_double(char *weight_string){
//this function takes the weight as an ordered string and converts it to a double.
    double weight = 0;

    char reversed_weight[200];
    strcpy(reversed_weight, weight_string);
    strrev(reversed_weight); //reverse the order of numbers to add them
    int N = strlen(reversed_weight);
    int pow = 0; //power of 10 to add the digits in weight
    for(int i = 0; i < N; i++){

        weight = weight + ( (reversed_weight[i] - '0') * power(10, pow));
        pow++;
    };

    return weight;
}


void read_in_line(char *line, double *weight, char* aterm){
//This function takes a line in the file, a pointer to a double and a pointer to a string
//It stores the weight in the variable pointed by *weight and the term in the variable in the string term.



    int N = strlen(line); //define the size of the line

    int weight_index = 0; //define where the weight starts
    char weight_string[200];

    int term_index = 0; //define where the term starts;

    int i = 0;
    while(line[i] == *" "){
        i++;
    }
    weight_index = i;

    while(isdigit(line[weight_index]) != 0){ //save the weight in a string
        weight_string[weight_index - i] = line[weight_index];
        weight_index++;
    }
    weight_string[weight_index - i] = '\0';

    *weight = string_weight_to_double(weight_string); //convert the weight to double
    term_index = weight_index;

    while((term_index < N - 1)){ //save the term in a string
        aterm[term_index - weight_index - 1] = line[term_index];
        term_index++;
    }
    aterm[term_index - weight_index - 1] = '\0';

    return;
}

int comparator(const void *a, const void *b){
    struct term *t1 = (struct term *)a;
    struct term *t2 = (struct term *)b;
    //  =("\n %d \n ",strcmp(a->term, b->term));
    return strcmp(t1->term, t2->term);
}

void read_in_terms(struct term **terms, int *pnterms, char *filename){
    FILE *fp = fopen(filename, "r"); //open file
    
    *pnterms = number_of_elements(filename); //store the nb of terms

    char line[200];

    //get the number of items in the file

    fgets(line, sizeof(line), fp); //not read the first line

    *terms = (struct term*)malloc(sizeof(struct term) * (*pnterms));

    for(int i = 0; i < *pnterms; i++){
        fgets(line, sizeof(line), fp); //read in at most sizeof(line) characters
                                    //(including '\0') into line.
        
        double temp_weight = 0; //weight for line [i]
        char temp_term[200] = " "; //term for line [i]
        read_in_line(line, &temp_weight, temp_term);


        (*terms)[i].weight = temp_weight;
        strcpy((*terms)[i].term, temp_term);


    };

    qsort(*terms, *pnterms, sizeof(struct term), comparator);

}


int lowest_match(struct term *terms, int nterms, char *substr){
    int LHS = 0;
    int RHS = nterms - 1;

    int j = (RHS + LHS) / 2;
    while( RHS - LHS > 0){
        if((strncmp(substr, terms[j].term, strlen(substr)) > 0)){
            LHS = j + 1;
        }else if (strncmp(substr, terms[j].term, strlen(substr)) < 0){
            RHS = j - 1;
        }else{
            RHS = j;
        }
        j = (RHS + LHS) / 2;
    }
    
    if ((strncmp(substr, terms[LHS].term, strlen(substr)) == 0)){
        return LHS;
    }else{
        return -1;
    };
}


int highest_match(struct term *terms, int nterms, char *substr){
    int LHS = 0;
    int RHS = nterms - 1;

    int j = (RHS + LHS) / 2;

    //to avoid doing infinitely many loops, the function stores the best match and looks for better matches that are in the interval
    //after the current best match. if there are none, return the best match.
    int temp_bestmatch = 0;

    while( RHS - LHS > 0){
        if((strncmp(substr, terms[j].term, strlen(substr)) >= 0)){
            LHS = j + 1;
            temp_bestmatch = j;
        }else if (strncmp(substr, terms[j].term, strlen(substr)) < 0){
            RHS = j - 1;
        }
        j = (RHS + LHS) / 2;

    }
    
    if ((strncmp(substr, terms[LHS].term, strlen(substr)) == 0)){
        return LHS;
    }else if (strncmp(substr, terms[temp_bestmatch].term, strlen(substr)) == 0){
        return temp_bestmatch;
    }else{
        return -1; //make sure to create an error to debug in Part 3
    }
}

int compare_weight(const void *a, const void *b){
// sorts terms by highest weight to lowest weight
    struct term *t1 = (struct term *)a;
    struct term *t2 = (struct term *)b;

    if(t1->weight > t2->weight){
        return -1;
    }else if(t1->weight < t2->weight){
        return 1;
    }else{
        return 0;
    };
}

void autocomplete(struct term **answer, int *n_answer, struct term *terms, int nterms, char *substr){
    int lowest = lowest_match(terms, nterms,substr);
    int highest = highest_match(terms, nterms, substr);

    if((lowest == -1) || (highest == -1)){
        //avoid errors
        *n_answer = 0;

    }else{
        //if there are no errors, run the code
        *n_answer = highest - lowest + 1;

        *answer = (struct term*)malloc(sizeof(struct term) * (*n_answer));

        for(int i = 0; i < *n_answer; i++){
            (*answer)[i].weight = terms[lowest + i].weight;
            strcpy((*answer)[i].term, terms[lowest + i].term);
        }
        qsort(*answer, *n_answer, sizeof(struct term), compare_weight);
    }

}
