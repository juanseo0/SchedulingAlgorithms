#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdbool.h"
#include "schedulers.h"
#include "list.h"
#include "cpu.h"
#include "task.h"

#define QUANTUM 10

struct node *head = NULL;

bool comesBefore(Task *a, Task *b) {
    if (a->priority != b->priority) {
        return a->priority > b->priority;
    } else {
        return strcmp(a->name, b->name) < 0;
    }
}

void add(char *name, int priority, int burst) {
    Task *newTask = (Task *)malloc(sizeof(Task));
    if (newTask == NULL) {
        fprintf(stderr, "Failed to allocate memory!");
        exit(1);
    }
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;

    insert(&head, newTask);
}

void sortList() {
    struct node *sorted = NULL;
    struct node *current = head;

    while (current != NULL) {
        struct node *next = current->next;
        struct node **insertNode = &sorted;

        while (*insertNode != NULL && comesBefore((*insertNode)->task, current->task)) {
            insertNode = &(*insertNode)->next;
        }
        current->next = *insertNode;
        *insertNode = current;
        current = next;
    }
    head = sorted;
}

void schedule() {
    sortList();
    int time = 0;

    while (head != NULL) {
        bool multipleSamePriority = false;
        int currentPriority = head->task->priority;
        struct node *current = head;

        struct node *temp = head;
        while (temp != NULL && temp->task->priority == currentPriority) {
            if (temp->next != NULL && temp->next->task->priority == currentPriority) {
                multipleSamePriority = true;
                break;
            }
            temp = temp->next;
        }

        if (multipleSamePriority) { // round robin
            while (current != NULL) {
                Task *task = current->task;
                if (task->priority == currentPriority) {
                    int timeSlice = 0;
                    if (task->burst > QUANTUM) { 
                        timeSlice = QUANTUM;
                    } else {
                        timeSlice = task->burst;
                    }
                    run(task, timeSlice);
                    time += timeSlice;
                    printf("\tTime is now: %d\n", time);
                    task->burst -= timeSlice;

                    if (task->burst <= 0) {
                        struct node *temp = current;
                        current = current->next;
                        delete(&head, temp->task);
                    } else {
                        current = current->next;
                    }
                    if (current == NULL) {
                        current = head;
                    }
                } else {
                    current = current->next;
                }
            }
        } else { // only priority
            Task *task = head->task;
            run(task, task->burst);
            time += task->burst;
            printf("\tTime is now: %d\n", time);
            delete(&head, task);
        }
    }
}
