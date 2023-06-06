#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TOBACCO 0
#define PAPER 1
#define MATCH 2

// Declaration of semaphores
sem_t sem_smoker[3];
sem_t sem_agent;
int ingredients[2];
char *ingredient_names[] = {
    "tobacco", "paper", "match"
};

void *smoker(void *arg)
{
    int smoker_id = *(int *)arg; // Get the smoker's ID
    for (;;)
    { // Infinite loop
        sem_wait(&sem_smoker[smoker_id]); // Wait for the semaphore to be available
        printf("Smoker %d has made a cigarette\n", smoker_id + 1);
        sem_post(&sem_agent); // Signal the agent semaphore
    }
    return NULL;
}

void *agent(void *arg)
{
    for (;;)
    {
        sem_wait(&sem_agent); // Wait for the agent semaphore to be available
        // Generate random ingredients
        ingredients[0] = rand() % 3;
        ingredients[1] = (ingredients[0] + (rand() % 2 + 1)) % 3;
        printf("Agent has placed %s and %s on the table\n",
               ingredient_names[ingredients[0]], ingredient_names[ingredients[1]]);
        // Post the semaphore of the smoker who has the missing ingredient
        if ((ingredients[0] == TOBACCO && ingredients[1] == PAPER) || (ingredients[0] == PAPER && ingredients[1] == TOBACCO))
        {
            sem_post(&sem_smoker[MATCH]);
        }
        else if ((ingredients[0] == TOBACCO && ingredients[1] == MATCH) || (ingredients[0] == MATCH && ingredients[1] == TOBACCO))
        {
            sem_post(&sem_smoker[PAPER]);
        }
        else if ((ingredients[0] == PAPER && ingredients[1] == MATCH) || (ingredients[0] == MATCH && ingredients[1] == PAPER))
        {
            sem_post(&sem_smoker[TOBACCO]);
        }
        sleep(1); // Sleep for 1 second for readability in the terminal
    }
    return NULL;
}

int main()
{
    srand(time(NULL)); // Seed the random number generator
    pthread_t smoker_thread[3], agent_thread; // Declare threads

    // Initialize agent and smoker semaphores
    sem_init(&sem_agent, 0, 1);
    for (int i = 0; i < 3; i++)
        sem_init(&sem_smoker[i], 0, 0);

    // Create smoker and agent threads
    int smoker_ids[] = {TOBACCO, PAPER, MATCH};
    for (int i = 0; i < 3; i++)
        pthread_create(&smoker_thread[i], NULL, smoker, &smoker_ids[i]);
    pthread_create(&agent_thread, NULL, agent, NULL);

    // Wait for all threads to finish
    for (int i = 0; i < 3; i++)
        pthread_join(smoker_thread[i], NULL);
    pthread_join(agent_thread, NULL);

    return 0;
}
