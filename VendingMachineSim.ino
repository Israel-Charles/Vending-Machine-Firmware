/**************************************************************************************************
* Project Author:      Israel Charles
* Project Name:        Vending Machine Simulation
*
* Project Folder:      Project_VendingMachine
* Main File:           VendingMachineSim.ino
* Version:             1.0
* Last Modification:   04/21/2024
* 
* Project Description: Using a ESP32Wroom Development board, and the FreeRTOS as an Real-Time
*                      Operating System, this project implements the firmware of a vending machine
**************************************************************************************************/

//********************************* Program Libraries *******************************************//
#include <Arduino.h>


//************************** Pins and Messages Definition ***************************************//
#define BLUE_LED 2                              // LED will indicate how the program is running
#define END_INPUT '\n'                          // Character that indicate the end of user input
#define INVALID_INPUT "X"                       // String that indicate an invalid input

// Input to enter when choosing which soda to dispense
#define COLA_ID '1'
#define GRAPE_ID '2'
#define ORANGE_ID '3'
#define LIME_ID '4'

// Output message when user input invalid character(s)
#define INVALID_MESSAGE "*** Your input contains invalid character(s). ***\n"\
            "*** Please follow the given instruction. ***"

// Welcome meesage that output during the set up function
#define WELCOME "      *** Welcome to Izi Vending Machine ***\n\n"\
            "         -- Let's satisfy your thirst!! --\n                       ---\n"

// Instruction to insert coins in the machine
#define INSTRUCTION "____________________________________________________\n"\
            "----------------------------------------------------\n"\
            "INSTRUCTIONS\nTo enter coins, type the respective letter according" \
            "\nto the following list:\n"\
            "\n - Q -> $0.25 (Quarter)\n - D -> $0.10 (Dimes)\n - N -> $0.05 (Nickels)\n\n"\
            "--> Then press 'Enter' to finalise your input\n\n** Note **\n"\
            " - Stack inputs are accepted\n\ - Inputs are Case Incentives\n"\
            "(Ex: Type 'QdN' then press 'Enter' to insert $0.40)\n\n"\
            "After inserting the coin(s), type the ID of the Soda\nto dispense,"\
            " then press 'Enter'\nOr type 'C' to cancel purchase then press Enter.\n"\
            "Change is dispensed after each purchase, if\nnecessary.\n"\
            "____________________________________________________\n"\
            "----------------------------------------------------\n"

// Tabular output that has the soda menu forma
#define MENU_TITLE  " --------------------------------------------------"\
                  "\n|ID        Soda         Price($)          Available|"\
                  "\n|--------------------------------------------------|"

// To output information about cola soda in the right format
#define COLA_INFO   "|%c         Cola          $%.2f               %2d    |"

// To output information about grape soda in the right format
#define GRAPE_INFO  "|%c         Grape         $%.2f               %2d    |"

// To output information about orange soda in the right format
#define ORANGE_INFO "|%c         Orange        $%.2f               %2d    |"

// To output information about lime soda in the right format
#define LIME_INFO   "|%c         Lime          $%.2f               %2d    |\n"\
                    " --------------------------------------------------"

// Message that outputs the amount of each coin returned after each purchase
#define COINS_RETURNED "Quarter(s) returned => %d\nDime(s) returned    => %d\n"\
                      "Nickel(s) returned  => %d"

// Outputing the amount of coins available in the machine
#define COINS_AVAILABLE " --------------- \n"\
                        "|Coins Available|\n"\
                        " --------------- \n"\
                        "|Quarter(s)  %2d |\n"\
                        "|Dime(s)     %2d |\n"\
                        "|Nickel(s)   %2d |\n"\
                        " --------------- \n"

// Message for random error 1 
#define ERROR_1_MESSAGE "****************************************************\n"\
           "*** ERROR CODE:        1                         ***\n"\
           "*** ERROR DESCRIPTION: VENDING MACHINE HAS BEEN  ***\n"\
           "***                    REMOTELY DISABLED.        ***\n"\
           "*** ERROR RESOLUTION:  TYPE THE REACTIVATION CODE***\n"\
           "***                    AND PRESS 'ENTER'.        ***\n"\
           "****************************************************\n"\
           "* Reactivation code: 'IziUnlock' (Case Sensitive)  *\n"\
           "****************************************************\n"\
           "\nENTER CODE ==> "

// Unlock code that fixes error 1
#define UNLOCK_CODE "IziUnlock"

// Message for random error 2
#define ERROR_2_MESSAGE "****************************************************\n"\
           "*** ERROR CODE:        2                         ***\n"\
           "*** ERROR DESCRIPTION: MAINTNANCE REQUIRED - COIN***\n"\
           "***                    COUNTER JAMMED            ***\n"\
           "*** ERROR RESOLUTION:  UNCLOG COUNTER AND PRESS  ***\n"\
           "***                    'RESET' BUTTON ON MACHINE ***\n"\
           "****************************************************\n"

// Message for random error 3
#define ERROR_3_MESSAGE "****************************************************\n"\
           "*** ERROR CODE:        3                         ***\n"\
           "*** ERROR DESCRIPTION: POWER FLUCTUATION         ***\n"\
           "*** ERROR RESOLUTION:  MACHINE EXPERIENCED MAJOR ***\n"\
           "***                    POWER FLUCTUATION. PLEASE ***\n"\
           "***                    RESET MACHINE             ***\n"\
           "****************************************************\n"

// Message for random error 4
#define ERROR_4_MESSAGE "****************************************************\n"\
           "*** ERROR CODE:        4                         ***\n"\
           "*** ERROR DESCRIPTION: MAINTNANCE REQUIRED - SODA***\n"\
           "***                    DISPENSER CABLE BROKEN    ***\n"\
           "*** ERROR RESOLUTION:  REPLACE CABLE AND PRESS   ***\n"\
           "***                    'RESET' BUTTON ON MACHINE ***\n"\
           "****************************************************\n"


//************************* Global Variables and constants **************************************//
// Number of Coins
int numQuarters = 8;                            // Initial number of Quarters in the machine
int numDimes = 20;                              // Initial number of Dimes in the machine
int numNickels = 40;                            // initial number of Nickels in the machine

// Number of Sodas
int numColaSoda = 10;                           // Initial number of Cola Soda in the machine
int numOrangeSoda = 10;                         // Initial number of Orange Soda in the machine
int numGrapeSoda = 10;                          // Initial number of Grape Soda in the machine
int numLimeSoda = 10;                           // Initial number of Lime Soda in the machine

// Price of Sodas in cents to prevent rounding error if working with double
const int COLA_SODA_PRICE = 25;                 // Price of a single Cola Soda in cents
const int GRAPE_SODA_PRICE = 30;                // Price of a single Grape Soda in cents
const int ORANGE_SODA_PRICE = 45;               // Price of a single Orange Soda in cents
const int LIME_SODA_PRICE = 40;                 // Price of a single Lime Soda in cents

// Input characters definition
const char INPUT_NICKEL = 'N';                  // Input that represents a nickel or $0.05
const char INPUT_DIME = 'D';                    // Input that represents a dime or $0.10
const char INPUT_QUARTER = 'Q';                 // Input that represents a Quarter or $0.25
const char CANCEL_OPTION = 'C';                 // Input that cancels an ongoing purchase

// Input/Output related variables
String inputString;                             // Variable that will hold of the input
char inputChar;                                 // Variable that transmit each character of input
char outputString[200];                         // Variable that store characters to output

int inputCoinTotal = 0;                         // Stores the amount of money inserted in cents
int insertedQuarter = 0;                        // Store the number of Quarters inserted
int insertedDime = 0;                           // Store the number of Dimes inserted
int insertedNickel = 0;                         // Store the number of Nickels inserted


//************************** Handles and Function prototypes ************************************//
// Handles Definition
TaskHandle_t User_Input;
TaskHandle_t Display_Menu;
TaskHandle_t Random_Errors;

// Function Prototypes for Tasks
void Task_Random_Errors(void * pvParameters);
void Task_Display_Menu(void * pvParameters);
void Task_User_Input(void * pvParameters);

// Helper Function Prototypes
String getSodaInput(int numChoices, char choiceArray[]);
String getCoinInput(int numChoices, char choiceArray[]);
bool isCharIn (char charToCheck, int arrayLength, char arrayOption[]);
char toUpper(char charToConvert);
void updateSodaStatus(char charIn);
void updateInsertStatus(char charIn);
void cancelPurchase();
bool processChange(double moneyToGive);
double priceInDollar(int cents);

//**************************** Initialization/Setup Function ************************************//
// Function that create the different tasks and set up Serial Communication
void setup() {
  // Setting the Pin for the Blue LED as output
  pinMode(BLUE_LED, OUTPUT);

  // Setting communication rate as 9600 Baud Rate 
  Serial.begin(9600);

  // Seting the seed for the random generator function
  pinMode(0, INPUT);
  randomSeed(analogRead(0));

  // Outputing the welcome message and instructions on the terminal
  Serial.println();
  Serial.println(INSTRUCTION);
  Serial.println(WELCOME);

  // Defining a data type that will receive the return value of when creating a task
  BaseType_t TaskCreationReturn;

  // Create a Random Error generator Task and return the status of the task (Created or Not)
  TaskCreationReturn = xTaskCreatePinnedToCore(&Task_Random_Errors,
                                              "Random Error Task",
                                              2000,
                                              NULL,
                                              3,
                                              &Random_Errors,
                                              1);

  // Checking if task was created successfully. If not, indicate error with Blue LED
  // BLUE LED blinking 3 times means Display Menu Task was not created successfully
  if (TaskCreationReturn != pdPASS){
    for (int i = 0; i < 3; i++){
      digitalWrite(BLUE_LED, HIGH);             // Turn LED ON
      vTaskDelay(250 / portTICK_RATE_MS);       // Wait 250 millisecond
      digitalWrite(BLUE_LED, LOW);              // Turn LED OFF
      vTaskDelay(250 / portTICK_RATE_MS);       // Wait 250 millisecond 
    }
  }

  // Create a Display Menu Task and return the status of the task (Created or Not)
  TaskCreationReturn = xTaskCreatePinnedToCore(&Task_Display_Menu,
                                              "Display Menu Task",
                                              10000,
                                              NULL,
                                              2,
                                              &Display_Menu,
                                              0);

  // Checking if task was created successfully. If not, indicate error with Blue LED
  // BLUE LED blinking 4 times means Display Menu Task was not created successfully
  if (TaskCreationReturn != pdPASS){
    for (int i = 0; i < 4; i++){
      digitalWrite(BLUE_LED, HIGH);             // Turn LED ON
      vTaskDelay(250 / portTICK_RATE_MS);       // Wait 250 millisecond
      digitalWrite(BLUE_LED, LOW);              // Turn LED OFF
      vTaskDelay(250 / portTICK_RATE_MS);       // Wait 250 millisecond 
    }
  }

  // Create User Taking Input Task and return the status of the task (Created or Not)
  TaskCreationReturn = xTaskCreatePinnedToCore(&Task_User_Input,
                                              "User Input Task",
                                              10000,
                                              NULL,
                                              1,
                                              &User_Input,
                                              0);

  // Checking if task was created successfully. If not, indicate error with Blue LED
  // BLUE LED blinking 5 times means Taking Input Task was not created successfully
  if (TaskCreationReturn != pdPASS){
    for (int i = 0; i < 5; i++){
      digitalWrite(BLUE_LED, HIGH);             // Turn LED ON
      vTaskDelay(250 / portTICK_RATE_MS);       // Wait 250 millisecond
      digitalWrite(BLUE_LED, LOW);              // Turn LED OFF
      vTaskDelay(250 / portTICK_RATE_MS);       // Wait 250 millisecond 
    }
  }

  // Turning the Blue LED ON to indicate that the program is running correctly
  digitalWrite(BLUE_LED, HIGH);

  // Delete the setup task since it's not needed
  vTaskDelete(NULL);
}


//****************************** Random Error Generator Function ********************************//
// Function that randomly generate an error for the program
void Task_Random_Errors(void * pvParameters){
  for(;;){
    vTaskDelay(1000 / portTICK_RATE_MS);       // Wait 1000 millisecond
  } 
}


//*********************************** Display Menu Task Function ********************************//
// Function that displays the Sodas for sale, their quatity, and their price
void Task_Display_Menu(void * pvParameters){
  // Loop to stop the task from returning
  for(;;){
    // Displaying the table like menu title
    Serial.println(MENU_TITLE);

    // Displaying info about Cola soda
    sprintf(outputString, COLA_INFO, COLA_ID, priceInDollar(COLA_SODA_PRICE), numColaSoda);
    Serial.println(outputString);

    // Displaying info about Grape soda
    sprintf(outputString, GRAPE_INFO, GRAPE_ID, priceInDollar(GRAPE_SODA_PRICE), numGrapeSoda);
    Serial.println(outputString);

    // Displaying info about Orange soda
    sprintf(outputString, ORANGE_INFO, ORANGE_ID, priceInDollar(ORANGE_SODA_PRICE), numOrangeSoda);
    Serial.println(outputString);

    // Displaying info about Lime soda
    sprintf(outputString, LIME_INFO, LIME_ID, priceInDollar(LIME_SODA_PRICE), numLimeSoda);
    Serial.println(outputString);
    Serial.println();

    // Displaying info abou the amount of coins available in the machine
    sprintf(outputString, COINS_AVAILABLE, numQuarters, numDimes, numNickels);
    Serial.println(outputString);
    Serial.println();

    // Suspending the display task so the program can take inputs
    vTaskSuspend(NULL);
  }
}


//*********************************** User Input Task Function **********************************//
// Function that takes in user coins input and user soda selection and dispense soda
void Task_User_Input(void * pvParameters){
  // Infinite Loop for task to not return
  for (;;){
    inputString = "";                           // Reseting the input string after each sale
    inputCoinTotal = 0;                         // Reseting money inserted after each operation
    insertedNickel = 0;
    insertedDime = 0;
    insertedQuarter = 0;

    Serial.print("Please Insert Coins ==> ");

    // Setting the selection of acceptable input to take from user
    char acceptableCoins[] = {INPUT_NICKEL, INPUT_DIME, INPUT_QUARTER};

    // Get user input through helper function given number of acceptable options and option array
    inputString = getCoinInput(3, acceptableCoins);

    // Checking if user had input something invalid
    if (inputString == INVALID_INPUT){
      Serial.println(INVALID_MESSAGE);
    }

    // Proceeding with selecting soda to purchase if input is valid
    else{
      inputString = "";                           // Reseting the input string to get soda
      Serial.print("Total Coin(s) Inserted => $");
      Serial.println(priceInDollar(inputCoinTotal));
      Serial.println("----");
      Serial.print("Please select a soda ==> ");

      // Setting the selection of acceptable input to take from user for the sodas
      char acceptableInput[] = {COLA_ID, GRAPE_ID, ORANGE_ID, LIME_ID, CANCEL_OPTION};

      // Get user input through helper function given number of acceptable options and option array
      inputString = getSodaInput(5, acceptableInput);

      // Checking if user had input something invalid
      if (inputString == INVALID_INPUT){
        Serial.println(INVALID_MESSAGE);
      }
      else{
        updateSodaStatus(inputString.charAt(0));
      }
    }
    Serial.println("----------------------------------------------------");
    Serial.println("____________________________________________________\n");

    // Resuming the Display menu task and execute since it has higher priority than current task
    vTaskResume(Display_Menu);
    vTaskDelay(250 / portTICK_RATE_MS);         // Delay to not flag the watchdog timer
  }
}


//********************************* Helper Functions called by Tasks ****************************//

// Input Control and Parse User Input function via serial input for inserting Coins
String getCoinInput(int numChoices, char choiceArray[]){
  bool isInputValid = true;                     // Flag that sets if there was an invalid input
  
  // Loop that process each character entered through Serial Input
  do{
    // Checking if there is a character to process from Serial input
    if (Serial.available() >  0) {
      inputChar = Serial.read();                // Save the character in the char input variable

      // Converting all alphabetic characters to Upper Case for uniformity
      inputChar = toUpper(inputChar);
      inputString += inputChar;                 // Add character to input String variable

      // Checking if the character is the END OF LINE (newline character)
      if (inputChar != END_INPUT) {

        // Processing non-space characters
        if (inputChar != ' '){
          // Checking if a character is a valid character. Flag it if it's not
          if (!isCharIn(inputChar, numChoices, choiceArray)){
            isInputValid = false;
          }
          else{
            updateInsertStatus(inputChar);      // Update coins inserted
          }
        }
      }
      // Finishing processing if character is a new line 
      else {
        inputString.trim();
        Serial.println(inputString);

        // If all characters were valid return the input string
        if (isInputValid){
          return inputString;
        }

        // If a character was invalid, canceling all input and return invalid flag
        else{
          inputCoinTotal = 0;
          return INVALID_INPUT;
        }
      }
    }
    vTaskDelay(250 / portTICK_RATE_MS);         // Delay to not raise watch dog timer flag
  } while (inputChar != END_INPUT);             // looping until a character is a new line
}

// Input Control and Parse User Input function via serial input for inserting Soda
String getSodaInput(int numChoices, char choiceArray[]){
  bool isInputValid = true;                     // Flag that sets if there was an invalid input
  
  // Loop that process each character entered through Serial Input
  do{
    // Checking if there is a character to process from Serial input
    if (Serial.available() >  0) {
      inputChar = Serial.read();                // Save the character in the char input variable

      // Converting all alphabetic characters to Upper Case for uniformity
      inputChar = toUpper(inputChar);
      inputString += inputChar;                 // Add character to input String variable

      // Checking if the character is the END OF LINE (newline character)
      if (inputChar == END_INPUT) {
        // Process and output the received message
        inputString.trim();
        Serial.println(inputString);

        // Checking if the input string is among the options
        if (inputString.length() == 1 && isCharIn(inputString.charAt(0), numChoices, choiceArray)){
          return inputString;
        }

        // If input is invalid return invalid flag
        else{
          return INVALID_INPUT;
        }
      } 
    }
    vTaskDelay(250 / portTICK_RATE_MS);         // Delay to not raise watch dog timer flag
  } while (inputChar != END_INPUT);             // looping until a character is a new line
}

// Function that checks if a character is in an array
bool isCharIn (char charToCheck, int arrayLength, char arrayOption[]){
  // Looping through the array to see if the input is one of the option
  for (int i = 0; i < arrayLength; i++){
    if (charToCheck == arrayOption[i]){
      return true;
    }
  }
  return false;
}

// Function that convert a lower case character to upper case character
char toUpper(char charToConvert) {
  // Converting only lowercase characters
    if (charToConvert >= 'a' &&  charToConvert <= 'z') {
        return charToConvert - ('a' - 'A');
    }
    return charToConvert;
}

// Functiont that counts money inserted and update coin counts
void updateInsertStatus (char charIn){
  if (charIn == INPUT_NICKEL){
    inputCoinTotal += 5;
    insertedNickel++;
  }
  else if (charIn == INPUT_DIME){
    inputCoinTotal += 10;
    insertedDime++;
  }
  else if (charIn == INPUT_QUARTER){
    inputCoinTotal += 25;
    insertedQuarter++;
  }
}

// Function that keep track of soda being sold
void updateSodaStatus(char charIn){
  // Reseting money inserted and coins inserted coins if cancel option was chosen
  if (charIn == CANCEL_OPTION){
    Serial.println("Option selected ==> Cancel Purchase...");
    Serial.println("Preocessing...\n----");
    cancelPurchase();
  }

  // Selling that Soda if that option was chosen
  else if (charIn == COLA_ID){
    Serial.println("Option selected ==> Cola");
    Serial.println("Processing...\n----");
    //Checking if that soda is available
    if (numColaSoda > 0){
      // Checking if the money inserted is enough for the soda
      if (inputCoinTotal >= COLA_SODA_PRICE){
        // Checking if it's possible to give exact change back
        if(processChange(inputCoinTotal - COLA_SODA_PRICE)){
          numColaSoda--;
          inputCoinTotal -= COLA_SODA_PRICE;
          Serial.println("1 Cola Soda sold");
          Serial.print("Total Change Returned => $");
          Serial.println(priceInDollar(inputCoinTotal));
          Serial.println();
          Serial.println(outputString);
          Serial.println("----");
        }
        else{
          Serial.println("Exact Change is required for this purchase.");
          cancelPurchase();
        }
      }
      else{
        Serial.println("Coin(s) inserted is insufficient for this purchase");
        cancelPurchase();
      }
    }
    else{
      Serial.println("Machine is out of the selected item");
      cancelPurchase();
    }
  }

  // Selling that Soda if that option was chosen
  else if (charIn == GRAPE_ID){
    Serial.println("Option selected ==> Grape");
    Serial.println("Processing...\n----");
    //Checking if that soda is available
    if (numGrapeSoda > 0){
      // Checking if the money inserted is enough for the soda
      if (inputCoinTotal >= GRAPE_SODA_PRICE){
        // Checking if it's possible to give exact change back
        if(processChange(inputCoinTotal - GRAPE_SODA_PRICE)){
          numGrapeSoda--;
          inputCoinTotal -= GRAPE_SODA_PRICE;
          Serial.println("1 Grape Soda sold");
          Serial.print("Total Change Returned => $");
          Serial.println(priceInDollar(inputCoinTotal));
          Serial.println();
          Serial.println(outputString);
          Serial.println("----");
        }
        else{
          Serial.println("Exact Change is required for this purchase.");
          cancelPurchase();
        }
      }
      else{
        Serial.println("Coin(s) inserted is insufficient for this purchase");
        cancelPurchase();
      }
    }
    else{
      Serial.println("Machine is out of the selected item");
      cancelPurchase();
    }
  }

  // Selling that Soda if that option was chosen
  else if (charIn == LIME_ID){
    Serial.println("Option selected ==> Lime");
    Serial.println("Processing...\n----");
    //Checking if that soda is available
    if (numLimeSoda > 0){
      // Checking if the money inserted is enough for the soda
      if (inputCoinTotal >= LIME_SODA_PRICE){
        // Checking if it's possible to give exact change back
        if(processChange(inputCoinTotal - LIME_SODA_PRICE)){
          numLimeSoda--;
          inputCoinTotal -= LIME_SODA_PRICE;
          Serial.println("1 Lime Soda sold");
          Serial.print("Total Change Returned => $");
          Serial.println(priceInDollar(inputCoinTotal));
          Serial.println();
          Serial.println(outputString);
          Serial.println("----");
        }
        else{
          Serial.println("Exact Change is required for this purchase.");
          cancelPurchase();
        }
      }
      else{
        Serial.println("Coin(s) inserted is insufficient for this purchase");
        cancelPurchase();
      }
    }
    else{
      Serial.println("Machine is out of the selected item");
      cancelPurchase();
    }
  }

  // Selling that Soda if that option was chosen
  else if (charIn == ORANGE_ID){
    Serial.println("Option selected ==> Orange");
    Serial.println("Processing...\n----");
    //Checking if that soda is available
    if (numOrangeSoda > 0){
      // Checking if the money inserted is enough for the soda
      if (inputCoinTotal >= ORANGE_SODA_PRICE){
        // Checking if it's possible to give exact change back
        if(processChange(inputCoinTotal - ORANGE_SODA_PRICE)){
          numOrangeSoda--;
          inputCoinTotal -= ORANGE_SODA_PRICE;
          Serial.println("1 Orange Soda sold");
          Serial.print("Total Change Returned => $");
          Serial.println(priceInDollar(inputCoinTotal));
          Serial.println();
          Serial.println(outputString);
          Serial.println("----");
        }
        else{
          Serial.println("Exact Change is required for this purchase.");
          cancelPurchase();
        }
      }
      else{
        Serial.println("Coin(s) inserted is insufficient for this purchase");
        cancelPurchase();
      }
    }
    else{
      Serial.println("Machine is out of the selected item");
      cancelPurchase();
    }
  }
}

// Function that cancels a purchase and reset money and coins inserted
void cancelPurchase(){
    inputCoinTotal = 0;
    insertedNickel = 0;
    insertedDime = 0;
    insertedQuarter = 0;
    Serial.println("Purchase has been cancelled.\nCoin(s) have been returned.");
}

// Function that gives change back to user. Returns false if unable to
bool processChange(double moneyToGive){
  double leftOver = moneyToGive;                // Variable used to parse the change to give

  // Store the amount of each coins to give based on calculation and availability
  int quarterToGive = 0;
  int dimeToGive = 0;
  int nickelToGive = 0;

  // Checking how many quarters to give if change is greater than a quarter or equal to it
  if (leftOver >= 25){
    // Checking if we have enough available quarters to give for change
    if (((int) (leftOver / 25)) <= numQuarters){
      quarterToGive = (int) (leftOver / 25);
    }
    // if we don't have enough quarters, give whatever is left
    else{
      quarterToGive = numQuarters;
    }
    // update the amount we have to find change for
    leftOver -= (quarterToGive * 25);
  }

  // Checking how many dimes to give if change is greater than a quarter or equal to it
  if (leftOver >= 10){
    // Checking if we have enough available dimes to give for change
    if (((int) (leftOver / 10)) <= numDimes){
      dimeToGive = (int) (leftOver / 10);
    }
    // if we don't have enough dimes, give whatever is left
    else{
      dimeToGive = numDimes;
    }
    // update the amount we have to find change for
    leftOver -= (dimeToGive * 10);
  }

  // Checking how many nickels to give if change is greater than a quarter or equal to it
  if (leftOver >= 5){
    // Checking if we have enough available nickels to give for change
    if (((int) (leftOver / 5)) <= numNickels){
      nickelToGive = (int) (leftOver / 5);
    }
    // if we don't have enough nickels, give whatever is left
    else{
      nickelToGive = numNickels;
    }
    // update the amount we have to find change for
    leftOver -= (nickelToGive * 5);
  }

  // If we are able to give the correct amount of change with the available coins return true
  if (leftOver == 0){
    // Saving coin output details in the output string
    sprintf(outputString, COINS_RETURNED, quarterToGive, dimeToGive, nickelToGive);

    // Updating the number of available coins in the machine
    numQuarters += insertedQuarter - quarterToGive;
    numDimes += insertedDime - dimeToGive;
    numNickels += insertedNickel - nickelToGive;

    return true;
  }

  // If unable to give correct amount of change return false
  else{
    return false;
  }
}

// Function that takes in money in cents and return its value in dollar
double priceInDollar(int cents){
  return (double)(cents / ((double) 100));
}


//******************************** Arduino Loop Function ****************************************//

// Empty Loop Function since it's not needed
void loop() {}
