#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CHAIRS 5

sem_t chairs;          
sem_t ta_sleeping;     
pthread_mutex_t mutex; 

int waiting_students = 0;

void *student(void *id) {
    int student_id = *(int *)id;
    free(id);

    while (1) {
      
        printf("Student %d arrived\n", student_id);

    
        pthread_mutex_lock(&mutex);

        if (waiting_students < MAX_CHAIRS) {
            waiting_students++;
            printf("Student %d is waiting. Waiting students: %d\n", student_id, waiting_students);

            pthread_mutex_unlock(&mutex);

           
            sem_wait(&chairs);

            
            sem_post(&ta_sleeping);

         
            printf("Student %d getting help from TA\n", student_id);
            sleep(1); 

            
            sem_post(&chairs);

            pthread_mutex_lock(&mutex);
            waiting_students--;
            pthread_mutex_unlock(&mutex);

            printf("Student %d leaving after help\n", student_id);
            break; 
        } else {
           
            pthread_mutex_unlock(&mutex);
            printf("Student %d found no empty chair and will come back later\n", student_id);
            sleep(rand() % 3 + 1); 
        }
    }
    return NULL;
}

void *ta(void *arg) {
    while (1) {
        
        printf("TA is sleeping\n");
        sem_wait(&ta_sleeping);

        printf("TA is helping a student\n");
        sleep(1); 

      
        pthread_mutex_lock(&mutex);
        if (waiting_students == 0) {
            printf("No students waiting, TA will sleep\n");
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t ta_thread;
    pthread_t students[20];

    sem_init(&chairs, 0, MAX_CHAIRS);
    sem_init(&ta_sleeping, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&ta_thread, NULL, ta, NULL);

    for (int i = 0; i < 20; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&students[i], NULL, student, id);
        sleep(rand() % 2); 
    }

    for (int i = 0; i < 20; i++) {
        pthread_join(students[i], NULL);
    }

    
    return 0;
}
