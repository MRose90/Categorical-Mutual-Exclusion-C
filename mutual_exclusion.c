//Michael Rose - Matthew Zakrevsky
//Oregon State University - CS 444 - Concurrency Assignment 3
//Categorical Mutual Exclusion
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
pthread_mutex_t insert_mutex;
pthread_mutex_t pause_searches;
pthread_mutex_t searches;
int MAX_SEARCHES = 10;
int MAX_INSERT = 5;
int MAX_DELETE = 2;
int max_value = 20;

struct node{
	int value;
	struct node* next;
};
int num_searches = 0;
int pause = 0;
struct node * head;


void * insert(void *arg){
	while(1){
		struct node * temp;
		struct node * curNode;
		pthread_mutex_lock(&insert_mutex);
		curNode = head;
		if(curNode == NULL){
			curNode = (struct node*)malloc( sizeof(struct node) );
			curNode->value = rand()%max_value;
			head = curNode;
			printf("INSERT: The value %d has been added as the head.\n",curNode->value);
		}else{
			while(curNode->next != NULL){
				curNode = curNode->next;
			}
			temp = (struct node*)malloc(sizeof(struct node));
			temp->value = rand()%max_value;
			temp->next = NULL;
			curNode->next = temp;
			printf("INSERT: The value %d has been added.\n",curNode->next->value);
		}
		
		pthread_mutex_unlock(&insert_mutex);
		sleep(rand()%MAX_INSERT);
	}
}



void * delete(void *arg){
	int value;
	struct node * curNode;
	int search_count;
	struct node * temp;
	while(1){
		temp = NULL;
		pthread_mutex_lock(&insert_mutex);
		pthread_mutex_lock(&pause_searches);
		pause = 1;
		pthread_mutex_unlock(&pause_searches);
		pthread_mutex_lock(&searches);
		search_count = num_searches;
		pthread_mutex_unlock(&searches);
		while(search_count != 0){
			pthread_mutex_lock(&searches);
			search_count = num_searches;
			pthread_mutex_unlock(&searches);
		}
		value = rand()%max_value;
		if(head != NULL){
			curNode = head;
			if(curNode->value == value){
				head = curNode->next;
				printf("DELETE: The value %d has been terminated!\n",value);
				free(curNode);
			} else{
				while(curNode != NULL && curNode->next != NULL && curNode->next->value != value){
					curNode = curNode->next;
				}
				if(curNode != NULL  && curNode->next != NULL && curNode->next->value == value){
					temp = curNode->next;
					curNode->next = curNode->next->next;
					printf("DELETE: The value %d has been terminated!\n",value);
					free(temp);
				}
			}
		}
		pthread_mutex_lock(&pause_searches);
		pause = 0;
		pthread_mutex_unlock(&pause_searches);
		pthread_mutex_unlock(&insert_mutex);
		sleep(rand()%MAX_DELETE);
		
	}
}

void * search(void *arg){
	int value;
	struct node * curNode;
	int found = 0;
	int p;
	while(1){
		pthread_mutex_lock(&pause_searches);
		p = pause;
		pthread_mutex_unlock(&pause_searches);
		found = 0;
		if(p == 0){
			pthread_mutex_lock(&searches);
			++num_searches;
			pthread_mutex_unlock(&searches);
			value = rand()%max_value;
			curNode = head;
			while(found == 0 && curNode != NULL){
				if(curNode->value == value){
					printf("SEARCH: The value %d exists!\n",value);
					found = 1;
				} else{
					curNode = curNode->next;
				}
			}
			pthread_mutex_lock(&searches);
			num_searches--;
			pthread_mutex_unlock(&searches);
		}
	sleep(rand()%MAX_SEARCHES);
	}
}

int main(int argc, char **argv)
{
	int t;
	pthread_t th[MAX_SEARCHES+MAX_INSERT+MAX_DELETE];
	for(t = 0; t<MAX_INSERT;++t){
		if(pthread_create(&th[t], NULL, insert, NULL)){
			printf("Error creating insert %d.\n",t);
			return 1;
		}
	}
	for(t = MAX_INSERT; t<MAX_SEARCHES+MAX_INSERT;++t){
		if(pthread_create(&th[t], NULL, search, NULL)){
			printf("Error creating searcher %d.\n",t);
			return 1;
		}
	}
	for(t = MAX_SEARCHES+MAX_INSERT; t<MAX_SEARCHES+MAX_INSERT+MAX_DELETE;++t){
		if(pthread_create(&th[t], NULL, delete, NULL)){
			printf("Error creating deleter %d.\n",t);
			return 1;
		}
	}
	for(t = 0; t < MAX_SEARCHES+MAX_INSERT+MAX_DELETE; t++)
   {
       pthread_join(th[t], NULL);
   }
	return 0;
}