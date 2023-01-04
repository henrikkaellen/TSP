#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "location.h"

typedef struct city {
    char name[4];
    location dims;
    int order;
} cities;

//functions for algorithms
void do_given(FILE* location_file, cities *stops, int city_num);
void do_farthest(FILE* location_file, cities *stops, int city_num);
void do_exchange_adjacent(FILE* location_file, cities *stops, int city_num);
void do_exchange_any(FILE* location_file, cities *stops, int city_num);

//function for calculating the sum of the distance
double sum_distances(cities *stops, int city_num);

//function for handling commmand line argument errors
int handle_errors(FILE* location_file, int argc, char *argv[]);

//function for running different algorithms
void run_algorithm(FILE* location_file, int argc, char* argv[], cities *stops, int city_num);

int main(int argc, char *argv[])
{
    FILE* location_file;
    location_file = fopen(argv[1], "r");

    if (handle_errors(location_file, argc, argv) == 1)
    {
        exit(1);
    }

    //character for number of cities
    int num;

    fscanf(location_file, "%d", &num);

    //declare city_num as number of cities
    int city_num = num;

    if (city_num < 2)
    {
        fprintf(stderr, "TSP: too few cities\n");
        exit(1);
    }

    //allocate spots for number of cities in array
    cities *stops = malloc(city_num * sizeof(cities));

    //add strings scanned from file to the cities struct
    for (size_t i = 0; i < city_num; i++)
    {
        fscanf(location_file, "%s", stops[i].name);
        stops[i].order = i;
    }

    //add the longitude and latitude to the cities struct
    for (int j = 0; j < city_num; j++)
    {
        fscanf(location_file, "%lf %lf", &stops[j].dims.lat, &stops[j].dims.lon);  
    }

    run_algorithm(location_file, argc, argv, stops, city_num);

    fclose(location_file);

    free(stops);
}

//function that handles command line errors
int handle_errors(FILE* location_file, int argc, char *argv[])
{
    //checks if file is present
    if (argv[1] == NULL)
    {
        fprintf(stderr, "TSP: missing filename\n");
        exit(1);
    }

    //checks if file opens
    else if (!location_file)
    {
        fprintf(stderr, "TSP: could not open %s\n", argv[1]);
        exit(1);
    }

    //checks if algorithms are valid
    for (size_t i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "-given") == 0 )
        {

        }

        else if (strcmp(argv[i], "-farthest") == 0)
        {

        }

        else if (strcmp(argv[i], "-exchange") == 0)
        {
            if (argv[i+1] == NULL)
            {
                fprintf(stderr, "TSP: invalid algorithm arguments\n");
                exit(1);
            }
            
            else if (strcmp(argv[i+1], "any") == 0)
            {
                i++;
            }

            else if (strcmp(argv[i+1], "adjacent") == 0)
            {
                i++;
            }

            else
            {
                fprintf(stderr, "TSP: invalid algorithm arguments\n");
                exit(1);
            }
        }

        else
        {
            fprintf(stderr, "TSP: invalid algorithm arguments\n");
            exit(1);
        }
    }
    return 0;
}

//function that goes through the command line and determines accordingly which function should execute
void run_algorithm(FILE* location_file, int argc, char* argv[], cities *stops, int city_num)
{
    for (size_t i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "-given") == 0)
        {
            //run given function
            do_given(location_file, stops, city_num);
        }

        else if (strcmp(argv[i], "-farthest") == 0)
        {
            //run farthest function
            do_farthest(location_file, stops, city_num);
        }

        else if (strcmp(argv[i], "-exchange") == 0)
        {    
            if (strcmp(argv[i+1], "any") == 0)
            {
                cities *exchange_any_stops = malloc(city_num * sizeof(cities));
                for (int j = 0; j < city_num; j++)
                {
                    exchange_any_stops[j] = stops[j];
                }
                i++;
                //run any function
                do_exchange_any(location_file, exchange_any_stops, city_num);
                free(exchange_any_stops);
            }

            else if (strcmp(argv[i+1], "adjacent") == 0)
            {
                cities *exchange_adj_stops = malloc(city_num * sizeof(cities));
                for (int j = 0; j < city_num; j++)
                {
                    exchange_adj_stops[j] = stops[j];
                }
                i++;
                //run adj function
                do_exchange_adjacent(location_file, exchange_adj_stops, city_num);
                free(exchange_adj_stops);
            }
        }
    }
}

//function to calculate the distance of the array of structs
double sum_distances(cities *stops, int city_num)
{
    double sum = 0;

    for (int k = 0; k < city_num; k++)
    {
            sum += location_distance(&stops[k].dims, &stops[(k+1) % city_num].dims);  
    }

    return sum;
}

//function for given algorithm
void do_given(FILE* location_file, cities *stops, int city_num)
{
    double sum = sum_distances(stops, city_num);

    printf("-given            :");
    printf("%13.2f ", sum);
    for (int l = 0; l < city_num; l++)
    {
        printf("%s ", stops[l].name);    
    }
    printf("%s", stops[0].name);
    printf("\n");
}

//function for farthest algorithm
void do_farthest(FILE* location_file, cities *stops, int city_num)
{
    //allocate spots for number of cities in array
    int *check_arr = calloc(city_num, sizeof(int));
    cities *farthest_city = malloc(city_num * sizeof(cities));

    cities curr_struct;
    double temp;
    int ind = 0;
    double max = 0;
    int ord_num = 0;

    curr_struct = stops[0];
    for (int i = 0; i < city_num; i++)
    {
        max = 0;
        curr_struct = stops[ind];
        check_arr[ind] = 1;
        if ((i % 2) == 0)
        {
            farthest_city[ord_num] = curr_struct;
            ord_num++;
        }
        else
        {
            farthest_city[city_num - ord_num] = curr_struct;
        }
        for (int l = 0; l < city_num; l++)
        {
            if (check_arr[l] != 1)
            {
                temp = location_distance(&curr_struct.dims, &stops[l].dims);
                if (temp > max)
                {
                    max = temp;
                    ind = l;
                }    
            }    
        }
    }

    double sum = sum_distances(farthest_city, city_num);

    printf("-farthest         :");
    printf("%13.2f ", sum);
    for (int i = 0; i < city_num; i++)
    {
        printf("%s ", farthest_city[i].name);
    }
    printf("%s", farthest_city[0].name);
    printf("\n");

    free(farthest_city);
    free(check_arr);
}

//function for the exchange adjacent algorithm
void do_exchange_adjacent(FILE* location_file, cities *stops, int city_num)
{
    //temporaray struct for swapping
    cities temp_struct;
    //sum of the distance that updates when the total distance is decrease
    double sum = sum_distances(stops, city_num);
    //variables for indices (ints) which were swapped
    int swap1 = 0;
    int swap2 = 0;
    //int that checks whether any new swaps have ben made
    int change = 1;
    //finds first city of initial input through order in struct
    int start;
    double half_sum = sum;

    //create hash table of all distances
    double hash_arr[city_num][city_num];
    for (int i = 0; i < city_num; i++)
    {
        for (int j = 0; j < city_num; j++)
        {
            hash_arr[i][j] = location_distance(&stops[i].dims, &stops[j].dims);
        }
    }

    while (change != 0)
    {
        change = 0;
        //swap adjacent city structs in array using a temporary vraibale
        temp_struct = stops[swap1];
        stops[swap1] = stops[swap2];
        stops[swap2] = temp_struct;

        //update sum
        sum = half_sum;
        for (int k = 0; k < city_num; k++)
        {
            double subtract = hash_arr[stops[(((k - 1) % city_num) + city_num) % city_num].order][stops[k].order] + hash_arr[stops[(k + 1) % city_num].order][stops[(k+2) % city_num].order];
            temp_struct = stops[k];
            stops[k] = stops[(k+1) % city_num];
            stops[(k+1) % city_num] = temp_struct;

            double temp_sum = (sum - subtract) + hash_arr[stops[(((k - 1) % city_num) + city_num) % city_num].order][stops[k].order] + hash_arr[stops[(k + 1) % city_num].order][stops[(k+2) % city_num].order];

            if (temp_sum < half_sum) 
            {
                half_sum = temp_sum;
                swap1 = k;
                swap2 = ((k+1) % city_num);
                change++;
            }
            temp_struct = stops[k];
            stops[k] = stops[(k+1) % city_num];
            stops[(k+1) % city_num] = temp_struct;
        }
    }

    //gets the final sum of the distance
    sum = sum_distances(stops, city_num);

    //prints 1)algorithm 2)distance 3)order of cities
    printf("-exchange adjacent:");
    printf("%13.2f ", sum);
    for (int i = 0; i < city_num; i++)
    {
        if (stops[i].order == 0)
        {
            start = i;
        }
    }

    if (stops[(((start - 1) % city_num) + city_num) % city_num].order > stops[(start + 1) % city_num].order) 
    {
        for (int i = start; i < city_num; i++)
        {
            printf("%s ", stops[i].name);
        }
        for (int i = 0; i < start; i++)
        {
            printf("%s ", stops[i].name);
        }  
        printf("%s", stops[start].name);  
    }
    else
    {
        for (int i = start; i >= 0; i--)
        {
            printf("%s ", stops[i].name);
        }
        for (int i = (city_num - 1); i > start; i--)
        {
            printf("%s ", stops[i].name);
        } 
        printf("%s", stops[start].name);  
    }
    printf("\n");
}

//function for the exchange any algorithm
void do_exchange_any(FILE* location_file, cities *stops, int city_num)
{
    //temporaray struct for swapping
    cities temp_struct;

    //sum of the distance that updates when the total distance is decrease
    double sum = sum_distances(stops, city_num);

    //variables for indices (ints) which were swapped
    int swap1 = 0;
    int swap2 = 0;

    //int that checks whether any new swaps have ben made
    int change = 1;

    //finds first city of initial input in order of struct
    int start;
    double half_sum = sum;

    //create hash table of all distances
    double hash_arr[city_num][city_num];
    for (int i = 0; i < city_num; i++)
    {
        for (int j = 0; j < city_num; j++)
        {
            hash_arr[i][j] = location_distance(&stops[i].dims, &stops[j].dims);
        }
    }

    while (change != 0)
    {
        change = 0;
        //swap city structs in array using a temporary variable
        temp_struct = stops[swap1];
        stops[swap1] = stops[swap2];
        stops[swap2] = temp_struct;

        sum = half_sum;
        for (int k = 0; k < city_num; k++)
        {
            double subtract_k = hash_arr[stops[(((k - 1) % city_num) + city_num) % city_num].order][stops[k].order] +  hash_arr[stops[(((k + 1) % city_num) + city_num) % city_num].order][stops[k].order];
            for (int l = 0; l < city_num; l++)
            {
                double subtract_l = hash_arr[stops[(((l - 1) % city_num) + city_num) % city_num].order][stops[l].order] +  hash_arr[stops[(((l + 1) % city_num) + city_num) % city_num].order][stops[l].order];
                temp_struct = stops[k];
                stops[k] = stops[l];
                stops[l] = temp_struct;

                double add1 = hash_arr[stops[(((k - 1) % city_num) + city_num) % city_num].order][stops[k].order] +  hash_arr[stops[(((k + 1) % city_num) + city_num) % city_num].order][stops[k].order];
                double add2 = hash_arr[stops[(((l - 1) % city_num) + city_num) % city_num].order][stops[l].order] +  hash_arr[stops[(((l + 1) % city_num) + city_num) % city_num].order][stops[l].order];
                double temp_sum = sum + add1 + add2 - subtract_k - subtract_l;

                if (temp_sum < half_sum) 
                {
                    half_sum = temp_sum;
                    swap1 = k;
                    swap2 = l;
                    change++;
                }
                temp_struct = stops[k];
                stops[k] = stops[l];
                stops[l] = temp_struct;
            }
        }
    }

    printf("-exchange any     :");
    printf("%13.2f ", sum);
    for (int i = 0; i < city_num; i++)
    {
        if (stops[i].order == 0)
        {
            start = i;
        }
    }
    //checks which adjacent struct to the first city has the lowest order
    if (stops[(((start - 1) % city_num) + city_num) % city_num].order > stops[(start + 1) % city_num].order) 
    {
        //prints all outputs from start to end
        for (int i = start; i < city_num; i++)
        {
            printf("%s ", stops[i].name);
        }
        //then from 0 to start
        for (int i = 0; i < start; i++)
        {
            printf("%s ", stops[i].name);
        }  
        printf("%s", stops[start].name);  
    }
    else
    {
        //does the reverse of the for loops above
        for (int i = start; i >= 0; i--)
        {
            printf("%s ", stops[i].name);
        }
        for (int i = (city_num - 1); i > start; i--)
        {
            printf("%s ", stops[i].name);
        } 
        printf("%s", stops[start].name);  
    }
    printf("\n");
}