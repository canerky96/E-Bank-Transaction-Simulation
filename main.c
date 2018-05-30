#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "definition.h"
#include "log.h"


void initalize();
int random_number(int , int );
void putOnBuffer(TransactionPtr,int );
TransactionPtr getOfBuffer(int);

Simulation simulation;
AccountPtr accountsPtr[MAX_ACCOUNT];
CustomerPtr customersPtr[MAX_CUSTOMER];
TellerPtr tellersPtr[MAX_TELLER];

pthread_t day_simulator;

void *day_setter(void){
  int a,b;

  for(a=0; a<simulation.simulatinDays; a++){
    sleep(10);

    for(b=0; b<simulation.numberOfTellers; b++){
      pthread_mutex_lock(&tellersPtr[b]->buffer.mutex);

    }

    simulation.currentDays += 1;

    for(b=0; b<simulation.numberOfTellers; b++){
      tellersPtr[b]->buffer.first = 0;
      tellersPtr[b]->buffer.last = 0;
      tellersPtr[b]->buffer.numInQ = 0;

    }
    for(b=0; b<simulation.numberOfAccount; b++){
      accountsPtr[b]->operationCount = 0;
      accountsPtr[b]->daySpend = accountsPtr[b]->daylimit;

    }

    for(b=0; b<simulation.numberOfTellers; b++){
      pthread_mutex_unlock(&tellersPtr[b]->buffer.mutex);

    }

  }
  pthread_exit((void*) 3);

}

void *customerThread(void *customer) {

    CustomerPtr _customer  = (CustomerPtr)customer;

    int i;
    while(simulation.currentDays != (simulation.simulatinDays + 1)){
        sleep(0.001);
        TransactionPtr _transaction = (Transaction*)malloc(sizeof(Transaction));

        int numb=random_number(0,_customer->numberofAccounts);
        int a_id= _customer->myaccountsId[numb];
        AccountPtr _sender=accountsPtr[a_id-1];



        int operation=random_number(0,4);
        int r_id;
        r_id=-1;
        if (operation==3){
            r_id= accountsPtr[random_number(0,simulation.numberOfAccount)]->account_id;
            if (r_id==a_id)
                continue;
        }


          if(_sender->operationCount >= OPERATION_LIMIT){
                continue;
            }

            int amount;

            if(_sender->amount < _sender->daySpend ){
              amount=random_number(0,_sender->amount);
            }else{
              amount=random_number(0,_sender->daySpend);
            }


        _transaction->customer_id=_customer->customer_id;
        _transaction->amount=amount;
        _transaction->sender_id=a_id;
        _transaction->receiver_id=r_id;
        _transaction->operation=operation;
        _transaction->currentDay=simulation.currentDays;

        //add new controller here :),don't put any control issues to teller,handle them in customeThread

        int buffer_index = random_number(0,simulation.numberOfTellers);


         putOnBuffer(_transaction,buffer_index);


    }
    pthread_exit((void*) 1);
}

void *tellerThread(void *tId) {

    int id=(int)tId;

    while (simulation.currentDays != (simulation.simulatinDays + 1)) {

        TransactionPtr _transaction=getOfBuffer(id);//wait until get one item


        if (_transaction==NULL || (_transaction->currentDay  == simulation.simulatinDays + 1))
        {
            break;
        }

        AccountPtr _sender  =accountsPtr[_transaction->sender_id-1];
        AccountPtr _receiver=accountsPtr[_transaction->receiver_id-1];

        //Transfer fit control
        if(_sender->operationCount >= OPERATION_LIMIT){
              continue;
          }
        if(_transaction->operation==3 || _transaction->operation==2){
              if((_transaction->amount > _sender->amount) ||(_transaction->amount > _sender->daySpend))
                continue;
          }
        if(_transaction->operation==3)//Transfer
        {

        while (1){
          if (pthread_mutex_trylock(&_sender->mutual_mutex) == 0){
            if(pthread_mutex_trylock(&_receiver->mutual_mutex) == 0){

              _sender->amount -=_transaction->amount;
              _receiver->amount+=_transaction->amount;

              _sender->operationCount++;
              _sender->daySpend -=_transaction->amount;

              pthread_mutex_unlock (&_receiver->mutual_mutex);
              pthread_mutex_unlock (&_sender->mutual_mutex);
              printf("A_ID : %d C_ID : %d T_ID : %d RECV_ID : %d  OPERATION : %d AMOUNT : %d SIMULATION DAY : %d\n",
              _transaction->sender_id, _transaction->customer_id, id,_transaction->receiver_id,_transaction->operation, _transaction->amount, _transaction->currentDay);
              break;

            }else{
              pthread_mutex_unlock (&_sender->mutual_mutex);
              sleep((_transaction->sender_id * 0.001));
              continue;
            }
          }else{
            sleep((_transaction->sender_id * 0.001));
          }
        }

        }
        else if (_transaction->operation==2 || _transaction->operation==1 ) //withdraw or deposit
        {
            pthread_mutex_lock (&_sender->mutual_mutex);

            if(_transaction->operation-2==0){
                _sender->amount-=_transaction->amount;

                _sender->daySpend -=_transaction->amount;
            }
            else{
                _sender->amount+=_transaction->amount;


            }
            _sender->operationCount++;


            pthread_mutex_unlock (&_sender->mutual_mutex);
            printf("A_ID : %d C_ID : %d T_ID : %d RECV_ID : %d  OPERATION : %d AMOUNT : %d SIMULATION DAY : %d\n",
            _transaction->sender_id, _transaction->customer_id, id,_transaction->receiver_id,_transaction->operation, _transaction->amount, simulation.currentDays);
        }
        else{
            pthread_mutex_lock (&_sender->mutual_mutex);
              int temp_amount = _sender->amount;

              _sender->operationCount++;

            pthread_mutex_unlock (&_sender->mutual_mutex);
            printf("A_ID : %d C_ID : %d T_ID : %d RECV_ID : %d  OPERATION : %d AMOUNT : %d SIMULATION DAY : %d\n",
            _transaction->sender_id, _transaction->customer_id, id,_transaction->receiver_id,_transaction->operation, temp_amount, simulation.currentDays);
        }


    }
    pthread_exit((void*) 2);
}

int main(int argc, const char * argv[]){
    initalize();


    pthread_t customer_thread[simulation.numberOfCustomers];
    pthread_t teller_thread[simulation.numberOfTellers];

    pthread_attr_t attr;
    int t;
    void *status;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&day_simulator, &attr, day_setter, NULL);

    for(t=0; t<simulation.numberOfCustomers; t++) {
        pthread_create(&customer_thread[t], &attr, customerThread, (void *)customersPtr[t]);
    }

    //create teller thread
    for(t=0; t<simulation.numberOfTellers; t++) {
        pthread_create(&teller_thread[t], &attr, tellerThread, (void *)t);
    }

    pthread_attr_destroy(&attr);

    pthread_join(day_simulator,&status);
    printf("Main completed  Day Simulator join thread %ld having a status of %ld\n",t,(long)status);

    for(t=0; t<simulation.numberOfCustomers; t++) {
        pthread_join(customer_thread[t], &status);
        printf("Main completed  customer join thread %ld having a status of %ld\n",t,(long)status);

    }
    simulation.numberOfCustomers=0;
    for(t=0; t<simulation.numberOfTellers; t++) {
        pthread_join(teller_thread[t], &status);
        printf("Main completed  Teller join thread %ld having a status of %ld\n",t,(long)status);

    }


      printf("Main: program completed. Exiting.\n");

      for(t=0; t<simulation.numberOfAccount; t++){
        printf("Customer : %d Account : %d Amount : %d\n",accountsPtr[t]-> customer_id,accountsPtr[t]-> account_id,accountsPtr[t]-> amount);
      }
}

void initalize(){
   srand(time(NULL));

    int counter = 0;
    int customerCount=0;

    FILE *input_file = fopen("/Users/fatih/Desktop/cse4034_project1/cse4034_project1/input.txt", "r");//in mac osx path
    if(input_file==NULL)
        input_file=fopen("input.txt", "r");

    simulation.currentDays=1;
    fscanf(input_file," %d %d %d ", &simulation.numberOfCustomers,&simulation.numberOfTellers,&simulation.simulatinDays);

    char buffer[255];
    char *token;

    while(fgets(buffer, 255, (FILE*) input_file)){
        AccountPtr temp_account = (Account*)malloc(sizeof(Account));

        token = strtok (buffer," ,.-");
        temp_account->account_id=atoi(token);
        temp_account->customer_id= atoi(strtok (NULL, " ,.-"));
        temp_account->amount= atoi(strtok (NULL, " ,.-"));
        temp_account->daylimit= atoi(strtok (NULL, " ,.-"));
        temp_account->operationCount=0;
        temp_account->daySpend=temp_account->daylimit;

        accountsPtr[counter] = temp_account;
        counter++;

        int index=0;
        CustomerPtr temp=customersPtr[index];
        while (temp!=NULL) {
            if (temp->customer_id==temp_account->customer_id) //same customer
            {
                temp->myaccountsId[temp->numberofAccounts]=temp_account->account_id;
                temp->numberofAccounts++;
                break;
            }
            index++;
            temp=customersPtr[index];
        }

        //not found same customer,create  customer and add array
        if(temp!=NULL)
            continue;
        CustomerPtr temp_customer = (Customer*)malloc(sizeof(Customer));

        temp_customer->myaccountsId[0]=temp_account->account_id;
        temp_customer->customer_id=temp_account->customer_id;
        temp_customer->numberofAccounts=1;

        customersPtr[customerCount]=temp_customer;
        customerCount++;


    }
     simulation.numberOfAccount=counter;

    int i;
    for(i = 0; i < simulation.numberOfTellers; i++){
      TellerPtr temp_teller = (Teller*)malloc(sizeof(Teller));
      temp_teller->teller_id = i;
      temp_teller->buffer.first=0;
      temp_teller->buffer.last=0;
      temp_teller->buffer.numInQ=0;
      int ret;
      ret=pthread_mutex_init(&temp_teller->buffer.mutex, NULL);

      tellersPtr[i] = temp_teller;

    }

    for(i = 0; i < simulation.numberOfTellers; i++){

      printf("%d\n", tellersPtr[i]->teller_id);
    }

    fclose(input_file);

}

int random_number(int min_num, int max_num){
    int result = 0;

    //srand(time(NULL));
    result = (rand() % (max_num - min_num)) + min_num;
    return result;
}

void putOnBuffer(TransactionPtr ptr, int index) {

    TellerPtr _temp = tellersPtr[index];
    pthread_mutex_lock(&_temp->buffer.mutex);
    _temp->buffer.pending[_temp->buffer.first] = ptr;
    _temp->buffer.first = (_temp->buffer.first + 1) % MAX_PENDING;
    _temp->buffer.numInQ++;

    pthread_mutex_unlock(&_temp->buffer.mutex);
    //printf("CId: %d CreatedTransc= SendId: %d : RecvId: %d : Oper: %d : Amount: %d Buffer Index : %d \n",ptr->customer_id,ptr->sender_id,ptr->receiver_id,ptr->operation,ptr->amount, index);
}

TransactionPtr getOfBuffer(int index) {
    TransactionPtr _ptritem;
    TellerPtr _temp = tellersPtr[index];

    while (1){
        pthread_mutex_lock(&_temp->buffer.mutex);
        if(simulation.simulatinDays==6 && _temp->buffer.numInQ == 0) // or day is finish
        {
            pthread_mutex_unlock(&_temp->buffer.mutex);
            return NULL;
        }
        else if(_temp->buffer.numInQ == 0){
            pthread_mutex_unlock(&_temp->buffer.mutex);
            sleep(0.001);
            continue;
        }
        else{
            _ptritem = _temp->buffer.pending[_temp->buffer.last];
            _temp->buffer.last = (_temp->buffer.last+1) % MAX_PENDING;
            _temp->buffer.numInQ--;
            pthread_mutex_unlock(&_temp->buffer.mutex);
            return _ptritem;
        }
    }

}
