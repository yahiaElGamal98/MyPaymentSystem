#include"paymentSystem.h"
#include<string.h>

#pragma warning (disable :4305)     //disabling truncation warning of line 8,9 and 10 (assigning double to float)

ST_transaction_t gastr_transactionData[TRANS_SIZE];
uint8_t gu8_numTransactions = 0;
ST_accountBalance_t gastr_serverData[ACC_SIZE] = { {100.00 ,"123456789" },{6000.00,"234567891"},{3250.25,"567891234"},
                                             {1500.12,"456789123"},{500.00,"258649173"},{2100.00,"654823719"},
                                             {0.00,"971362485"},{1.00,"793148625"},{10.12,"123123456"},{0.55,"456789321"} };

//The function below changes each date token of CARD ("MM"/"YY") and transaction ("DD"/"MM"/YY"YY") 
//into seaprate strings as shown by the double quotes above
//it takes a 2D array to store the tokens, the number of tokens (2) tokens if card expiry and (3) tokens if transaction date
//and it takes the string entered by user as a source
void extractDateTokens(uint8_t destination[][NUM_ELEMENTS], uint8_t numTokens, uint8_t* source)
{
   uint8_t u8_index;    //the index used to go through source array
   uint8_t u8_finalcursor;    //used to shift the value of cursor from index to extract the final "YY" from "YYYY"
                              //used only for transaction date extraction
   for (u8_index = 0; source[u8_index] != MY_NULL; u8_index++)
   {
      if (u8_index == 6)   //if the index is 6 DD/MM/->'Y'YYY
      {
         u8_index++;    //shift one more element DD/MM/Y->'Y'YY and continue
      }
      else if (u8_index > 6)     //after shifting in the line above we are now in DD/MM/YY->'Y'Y 
      {                                   //cusor now needs to be at string start of destination string
         u8_finalcursor = u8_index - 2;  //shifting the cursor so as to have no gaps in the destination year string
         destination[u8_finalcursor / NUM_ELEMENTS][u8_finalcursor % NUM_ELEMENTS] = source[u8_index];    //assigning the data

      }
      else if (source[u8_index] != '/')      //in any other case than the one shown above, if numbers are encountered not '/'
      {
         destination[u8_index / NUM_ELEMENTS][u8_index % NUM_ELEMENTS] = source[u8_index];   // we assign them to the corresponding string
      }
      else
      {
         destination[u8_index / NUM_ELEMENTS][u8_index % NUM_ELEMENTS] = MY_NULL;   //else we replace the '/' with NULL to end the string
      }

      if (numTokens == CARD_TOKENS)    //if we are extracting card date
      {
         destination[YEAR_INDEX_CARD][2] = MY_NULL;      //Termination with NULL in the last array place 
      }
      else
      {
         destination[YEAR_INDEX_TRANS][2] = MY_NULL;     //same for transaction date
      }
   }
}
uint8_t ifExpired(ST_cardData_t card, ST_terminalData_t terminal)  //function to check expiry date of card with current date
{
   uint8_t terminalDateTokens[3][3];      //the terminal token arrays used as destination as shown in the function above
   uint8_t cardDateTokens[2][3];       //the card token arrays
   extractDateTokens(cardDateTokens,CARD_TOKENS, card.cardExpirationDate);    //use the extraction function for card tokens
   extractDateTokens(terminalDateTokens, TERMINAL_TOKENS, terminal.transactionDate);   //use the extraction fun for terminal tokens
   if (!(OLDER == strcmp(terminalDateTokens[YEAR_INDEX_TRANS], cardDateTokens[YEAR_INDEX_CARD])))    // if the year string of terminal isn't older
   {
      if (!(OLDER == strcmp(terminalDateTokens[MONTH_INDEX_TRANS], cardDateTokens[MONTH_INDEX_CARD])))      //check for the month
      {
         return VALID;        //card is valid
      }
      else
      {
         return INVALID;      //card is expired (older month)
      }
   }
   else
   {
      return INVALID;      //card is expired (older year)
   }
}

sint16_t ifExists(ST_cardData_t card,ST_accountBalance_t* serverData)    //function to check if PAN exits in server data
{
   sint16_t s16_clientIndex;
   for (s16_clientIndex = 0; s16_clientIndex < ACC_SIZE; s16_clientIndex++)      //normal linear search
   {
      if (0==strcmp(serverData[s16_clientIndex].primaryAccountNumber,card.primaryAccountNumber))
      {
         return s16_clientIndex;    //return client index
      }
   }
   return DOES_NOT_EXIST;   //not found, return NULL
}

//server function to handle data sent by terminal
void serverHandler(ST_cardData_t myCard,ST_terminalData_t myTerminal,ST_accountBalance_t* serverData)    
{
   sint16_t clientIndex = ifExists(myCard, serverData);   //does the PAN Exist?
   printf("Verifying Data from server....\n");
   if (DOES_NOT_EXIST!=clientIndex)  //if CLIENT EXISTS
   {
      if (myTerminal.transAmount <= serverData[clientIndex].balance)    //if he has enough balance
      {
         serverData[clientIndex].balance -= myTerminal.transAmount;     //deduct that balance from his account
         printf("The Transaction is APPROVED.\n");    //approve the transaction
         if (gu8_numTransactions<TRANS_SIZE)
         {
            gastr_transactionData[gu8_numTransactions].cardHolderData=myCard;    //save the transaction data
            gastr_transactionData[gu8_numTransactions].termData = myTerminal;
            gastr_transactionData[gu8_numTransactions].transStat = APPROVED;
            gu8_numTransactions++;        //increment number of transactions
         }
         else
         {
            printf("MEMORY FULL!!!\n");
         }
      }
      else        //if he doesn't have enough balance
      {
         printf("The Transaction is DECLINED.\n");    //decline transaction
         if (gu8_numTransactions < TRANS_SIZE)  
         {
            gastr_transactionData[gu8_numTransactions].cardHolderData = myCard;     //save transaction data
            gastr_transactionData[gu8_numTransactions].termData = myTerminal;
            gastr_transactionData[gu8_numTransactions].transStat = DECLINED;
            gu8_numTransactions++;     //increment number of transactions
         }
         else
         {
            printf("MEMORY FULL!!!\n");
         }
      }
   }
   else        //if the PAN doesn't exist
   {
      printf("The Transaction is DECLINED.\n");       //decline transaction
      if (gu8_numTransactions < TRANS_SIZE)     //save transaction data if memory isn't full
      {
         gastr_transactionData[gu8_numTransactions].cardHolderData = myCard;
         gastr_transactionData[gu8_numTransactions].termData = myTerminal;
         gastr_transactionData[gu8_numTransactions].transStat = DECLINED;
         gu8_numTransactions++;
      }
      else
      {
         printf("MEMORY FULL!!!\n");
      }
   }
}

//terminal handling function
void terminalHandler(ST_cardData_t myCard, ST_terminalData_t myTerminal, ST_accountBalance_t* serverData)
{
   uint8_t u8_verify;      //verify card expiry
   myTerminal.maxTransAmount = MAX_AMOUNT;      //set MAX transaction value
   u8_verify = ifExpired(myCard, myTerminal);      //check expiry date
   if (u8_verify == VALID)    //if valid
   {
      serverHandler(myCard, myTerminal, serverData);     //send data to server for further handling
   }
   else
   {
      printf("The Transaction is DECLINED.\n");       //else decline transaction
   }
}
void swap(ST_transaction_t* left, ST_transaction_t* right)  //sorting helper function
{
   ST_transaction_t u8_temp;       // temp variable for swapping
   u8_temp = *right;      // temp variable equal right
   *right = *left;         // right handside variable eqal left var
   *left = u8_temp;        //left variable equal temp
}
void sortAndCheck(ST_transaction_t* arr, uint8_t size)      //used to sort the transaction data
{
   uint8_t u8_traverseIndex   // used for moving in loop
      , u8_iterationIndex   //used for iterations 
      , u8_sortFlag = 0;   //used to check if array is sorted on first iteration
   for (u8_traverseIndex = 0; u8_traverseIndex < size - 1; u8_traverseIndex++)
   {
      if (1==strcmp(arr[u8_traverseIndex + 1].cardHolderData.primaryAccountNumber, arr[u8_traverseIndex].cardHolderData.primaryAccountNumber))   //Iterate once to check if array is sorted
      {
         u8_sortFlag = 0;   //it doesn't need sorting all elements are in ascending order
         continue;
      }
      else
      {
         u8_sortFlag = 1;     //an element is out of order, it needs sorting
         break;
      }
   }
   if (u8_sortFlag) //sorting operation is done if it needs sorting
   {
      for (u8_iterationIndex = 0; u8_iterationIndex < size - 1; u8_iterationIndex++)
      {
         for (u8_traverseIndex = u8_iterationIndex + 1; u8_traverseIndex < size; u8_traverseIndex++)
         {
            if ((-1)==strcmp(arr[u8_traverseIndex].cardHolderData.primaryAccountNumber, arr[u8_iterationIndex].cardHolderData.primaryAccountNumber))
            {
               swap(arr + u8_traverseIndex, arr + u8_iterationIndex);
            }
            else
            {
               continue;
            }
         }
      }
   }
   else  //it doesn't need sorting
   {
      printf("Transactions are already sorted.\n");
   }
}
const char* getTransStatus(EN_transStat_t status)  //switch enums to string to be printed
{
   switch (status)
   {
   case APPROVED: return "APRROVED";
   case DECLINED: return "DECLINED";
   }
   return MY_NULL; //it will never reach here.
}
void printTrans(void)      // print function to print all sorted transactions based on PAN with terminal exit
{
   uint8_t u8_transIndex;
   sortAndCheck(gastr_transactionData, gu8_numTransactions);
   printf("Transactions that were sent to server are (SORTED BASED ON PAN):\n");
   printf("PAN            NAME            TRANSACTION DATE     TRANS AMOUNT      TRANSACTION STATUS\n");
   for (u8_transIndex = 0; u8_transIndex < gu8_numTransactions; u8_transIndex++)
   {
      printf("%s      %s        %s       %f       %s\n",gastr_transactionData[u8_transIndex].cardHolderData.primaryAccountNumber,
                                                         gastr_transactionData[u8_transIndex].cardHolderData.cardHolderName,
                                                         gastr_transactionData[u8_transIndex].termData.transactionDate,
                                                         gastr_transactionData[u8_transIndex].termData.transAmount,
                                                         getTransStatus(gastr_transactionData[u8_transIndex].transStat));
   }
}




