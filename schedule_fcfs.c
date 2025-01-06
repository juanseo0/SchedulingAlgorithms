#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdbool.h"
#include "schedulers.h"
#include "list.h"
#include "cpu.h"
#include "task.h"

struct node *head = NULL;

bool comesBefore(char *a, char *b) {
    return strcmp(a, b) < 0;
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

Task *pickNextTask() {
    if (!head) {
		return NULL;
	}
    struct node *temp = head;
    Task *current = temp->task;

    while (temp != NULL) {
        if (comesBefore(temp->task->name, current->name)) {
            current = temp->task;
        }
        temp = temp->next;
    }
    return current;
}

void schedule() {
    int time = 0;
    Task *nextTask;
    while (head != NULL) {
        nextTask = pickNextTask();
        if (nextTask == NULL) {
            break;
        }
        run(nextTask, nextTask->burst);
        time += nextTask->burst;
        printf("\tTime is now: %d\n", time);
        delete(&head, nextTask);
    }
}
