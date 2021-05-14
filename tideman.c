#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count = 0;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
int graph(int prim, int location);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    //itarate through candidates to check to see if the name provided is valid
    for (int i = 0; i < candidate_count; i++)
    {
        //compaire the 2 strings
        if (strcmp(candidates[i], name) == 0)
        {
            //if it's valid then update the ranks array
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    //itarate throug a grid and implemnt how many people perfer that candidate fill in the grid
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]] += 1;
        }
        
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    //iterate through candidates
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            //if the element in row i calume j is more then the element in row j calumn i then the candidate in row i wins
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                //i added this diff to keep track by how much they won
                pair_count++;
            }
            //if the element in row i calumn j is less then the candidate in row j calumn i then candidate in row j wins
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                //this diff was added to keep track by how much they won
                pair_count++;
            }
          
        }


        
        
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int greatest = 0;
    //iterate through the array
    for (int i = 0; i < pair_count - 1; i++)
    {
        //set a varubale to store the largest number
        int large = i;
        
        //itarate through the array taking the next number and comparing it to the largest number
        for (int j = i + 1; j < pair_count; j++)
        {
            //if largest is not more perfered on the preferance grid then j then assighn j to largest
            if (preferences[pairs[large].winner][pairs[large].loser] < preferences[pairs[j].winner][pairs[j].loser])
            {
                large = j;
            }
            

        }
        //swap the  elements
        if (large > i)
        {
            //put elements from the right into a temp varuble
            int win = pairs[large].winner;
            int los = pairs[large].loser;
                
            //put elements from the left into the elemnts from the right
            pairs[large].winner = pairs[i].winner;
            pairs[large].loser = pairs[i].loser;
                
            //put the temp varubles into the elements from the left
            pairs[i].winner = win;
            pairs[i].loser = los;
        }
       
       
    }

    return;
}


//set a varuble to keep track to my return values in graph
int counterl = 0;
// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    
    
    //itarate through the pairs sorted array
    for (int i = 0; i < pair_count; i++)
    {
        //set the globle varuble to zero befor calling the function
        counterl = 0;
        
        //call the function
        graph(pairs[i].winner, pairs[i].loser);
         
        //if the counter varuble is still 0 that means there are no cycals then change the value to true
        if (counterl == 0)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
        
       
 
    }
    
    return;
}


//created a recursive function to keep see if there is a cycal 
int graph(int prim, int location)
{
    //itarate through the row of the loser to see if he is pointing at anyting   
    for (int i = 0; i < candidate_count; i++)
    {
        //if the loser or the candidate the loser is pointing to is true then add one to our counter
        if (locked[location][prim] == true)
        {
            return 1;
        }
        //if the loser is pointing to someone then call the function again to make sure the persone he is pointing to is not pointing to the winner
        if (locked[location][i] == true)
        {
            //update the counter every time the function retuns 
            counterl += graph(prim, i);
        }
    }
    return 0;
           
}
       
       

// Print the winner of the election
void print_winner(void)
{
    //iterate through the rows in the locked 2 dimentinal array
    for (int i = 0; i < candidate_count; i++)
    {
        //this counter function will keep track of the amount of trues in the row 
        int counter = 0;
        //iterate through the calumns in the locked array
        for (int j = 0; j < candidate_count; j++)
        {
            //if all the cells in calumn is set to false this means no one is pointing to it 
            if (locked[j][i] == false)
            {
                counter++;
                if (counter == candidate_count)
                {
                    printf("%s\n", candidates[i]);
                    return ;
                }
            }
        }
    }
    return;
}
