//
//  definition.h
//  cse4034_project1
//
//  Created by Fatih on 07/04/2018.
//  Copyright © 2018 Fatih. All rights reserved.
//
#include<pthread.h>

#define MAX_ACCOUNT 100
#define MAX_CUSTOMER 100
#define MAX_TELLER 100
#define MAX_PENDING 150
#define ACCOUNT_LIMIT 3
#define OPERATION_LIMIT 3
#define DEBUG 0


#ifndef definition_h
#define definition_h

struct Account{
    int account_id;
    int customer_id;
    int amount;
    int daylimit;
    int operationCount;
    int daySpend;
    pthread_mutex_t mutual_mutex;
    pthread_cond_t condVar;
};

struct Customer{
    int customer_id;
    int numberofAccounts;
    int myaccountsId[ACCOUNT_LIMIT];

};
struct Simulation{
    int simulatinDays;
    int currentDays;
    int numberOfCustomers;
    int numberOfAccount;
    int numberOfTellers;

};
struct Transaction{
    int customer_id;
    int sender_id;
    int operation;
    int receiver_id;
    int amount;
    int currentDay
};


typedef struct Account Account;
typedef struct Account* AccountPtr;
typedef struct Customer Customer;
typedef struct Customer* CustomerPtr;
typedef struct Simulation Simulation;
typedef struct Transaction Transaction;
typedef struct Transaction* TransactionPtr;
typedef struct Teller Teller;
typedef struct Teller* TellerPtr;

struct Queue{
    TransactionPtr pending[MAX_PENDING];                   // queue to hold produced TRANSACTİONS
    int    first;                    // index into q of next free space
    int    last;                    // index into q of next available number
    int    numInQ;
    pthread_mutex_t mutex;          // mutual exclusion lock
};

typedef struct Queue Queue;
typedef struct Queue* QueuePtr;

struct Teller{
  int teller_id;
  Queue buffer;
};

#endif /* definition_h */
