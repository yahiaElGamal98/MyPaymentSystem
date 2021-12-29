#ifndef _PAYMENTSYSTEM_H_
#define _PAYMENTSYSTEM_H_

/*INCLUSIONS***************************************/
#include<stdio.h>
#include<stdlib.h>
#include"STD_types.h"

/*MACROS******************************************/
#define MAX_AMOUNT 5000
#define TRANS_SIZE 10
#define ACC_SIZE 10
#define FOUND 1
#define NOT_FOUND 0
#define NUM_ELEMENTS 3
#define CARD_TOKENS 2
#define TERMINAL_TOKENS 3
#define OLDER 1
#define MONTH_INDEX_TRANS 1
#define YEAR_INDEX_TRANS 2
#define MONTH_INDEX_CARD 0
#define YEAR_INDEX_CARD 1
#define VALID 1
#define INVALID 0
#define MY_NULL '\0'
#define DOES_NOT_EXIST -1




/*TYPE DEFINITIONS (STRUCTS)****************************************/
typedef struct ST_cardData_t
{
   uint8_t cardHolderName[25];
   uint8_t primaryAccountNumber[20];
   uint8_t cardExpirationDate[6];
} ST_cardData_t;

typedef struct ST_terminalData_t
{
   float transAmount;
   float maxTransAmount;
   uint8_t transactionDate[11];
} ST_terminalData_t;

typedef enum EN_transStat_t
{
   DECLINED,
   APPROVED
}EN_transStat_t;

typedef struct ST_transaction_t
{
   ST_cardData_t cardHolderData;
   ST_terminalData_t termData;
   EN_transStat_t transStat;
} ST_transaction_t;

typedef struct ST_accountBalance_t
{
   float balance;
   uint8_t primaryAccountNumber[20];
}ST_accountBalance_t;

/**FUNCTION PROTOTYPES*********************************/

void extractDateTokens(uint8_t destination[][NUM_ELEMENTS], uint8_t numTokens, uint8_t* source );
uint8_t ifExpired(ST_cardData_t, ST_terminalData_t);
sint16_t ifExists(ST_cardData_t card,ST_accountBalance_t* server);
void terminalHandler(ST_cardData_t,ST_terminalData_t, ST_accountBalance_t*);
void serverHandler(ST_cardData_t, ST_terminalData_t, ST_accountBalance_t*);
void swap(ST_transaction_t* left, ST_transaction_t* right);
void sortAndCheck(ST_transaction_t* arr, uint8_t size);
void printTrans(void);
const char* getTransStatus(EN_transStat_t status);

#endif
