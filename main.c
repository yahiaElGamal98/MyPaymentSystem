#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"paymentSystem.h"

#pragma warning (disable :4996)     //diabling visual studio warning of using scanf

extern ST_transaction_t gastr_transactionData[TRANS_SIZE];     //Fetch the global variables from external file, all transaction data
extern uint8_t gu8_numTransactions;      //Number of transaction that reached the server
extern ST_accountBalance_t gastr_serverData[ACC_SIZE];      //The server data of accounts and balances


int main()
{
   uint8_t u8_quitFlag     //Flag to quit on user input (n/N)
      ,u8_choice     //variable to store user choice (yY/nN)
      ,u8_choiceValidFlag;  //Variable to check validity of input
   ST_cardData_t str_myCard;    //the card data needed from the user
   ST_terminalData_t str_myTerminal;   //the terminal data needed for the transaction
   do
   { /********DATA INPUT SEQUENCE**************************************/
      printf("Please Enter Card Data:\n");
      printf("Please Enter Card Holder Name:\n");
      scanf("%[^\n]", &str_myCard.cardHolderName);
      printf("Please Enter the Primary Account Number:\n");
      scanf("%s", &str_myCard.primaryAccountNumber);
      printf("Please Enter Card Expiry Date:\n");
      scanf("%s", &str_myCard.cardExpirationDate);
      printf("Please Enter Terminal Data:\n");
      printf("Please Enter transaction Amount:\n");
      scanf("%f", &str_myTerminal.transAmount);
      /*CHECKING IF TRANSACTION AMOUT EXCEEDS MAX TRANSACTION VALUE OF THE TERMINAL**************/
      if (MAX_AMOUNT < str_myTerminal.transAmount)
      {
         printf("The Transaction is DECLINED.\n");    //if it does, decline locally, no handling required
      }
      else
      {
         printf("Please Enter transaction Date:\n");     //else enter the transaction date 
         scanf("%s", &str_myTerminal.transactionDate);
         terminalHandler(str_myCard, str_myTerminal, gastr_serverData);    //send all data to the terminal to handle
      }
      printf("Do you want to continue? (y/n):\n");    //Check if user wants to continue or exit
      scanf(" %c", &u8_choice);
      getchar();    //eating enter character
      u8_choiceValidFlag = INVALID;    //Lower the flags to check inside the loop
      u8_quitFlag = INVALID;     
      while (!u8_choiceValidFlag)
      {
         if (('y' == u8_choice) || ('Y' == u8_choice))
         {
            u8_choiceValidFlag = VALID;       //user entered a valid choice, raise the flag
         }
         else if (('n' == u8_choice) || ('N' == u8_choice))
         {
            printTrans();     // user wants to exit, print all transaction history (sorted) and proceed to exit
            u8_quitFlag = VALID;       //raise quit flag
            u8_choiceValidFlag = VALID;    //user entered a valid choice
         }
         else
         {
            printf("incorrect choice, please try again.\n");
            u8_choiceValidFlag = INVALID;    //input wasn't Valid
         }
      }
   } while (!u8_quitFlag);
   system("PAUSE");
   return 0;
}