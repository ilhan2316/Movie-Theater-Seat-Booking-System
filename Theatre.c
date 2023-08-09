#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "MovieTheaterLib.h"
#include "QueueLib.h"
#include "BSTLib.h"
#include "ListLib.h"
#include "StackLib.h"
void PrintReceipts(SNODE **StackTop)
{
    if (*StackTop == NULL)
    {
        printf("No receipts.\n");
        return;
    }
 
    printf("Today's receipts:\n\n");

    SNODE *TempPtr = *StackTop;
    char ticket[10];

    while (TempPtr != NULL)
    {
        printf("\n\nReceipt #%d\n\n", TempPtr->ReceiptNumber);
        printf("%s\n\n", TempPtr->MovieTheaterName);
       

        LNODE *TicketListHead = TempPtr->TicketList;

        while (TicketListHead != NULL)
        {
            ReturnAndFreeLinkedListNode(&TicketListHead, ticket);
            printf("%s  ", ticket);
        }

        pop(StackTop);
        TempPtr = *StackTop;
    }

}

BNODE *PickAndDisplayTheater(BNODE *BSTRoot, char MovieTheaterMap[][MAXCOLS], int *MapRow, int *MapCol)
{
   
    BNODE *MyTheater = NULL;
    char zip[6] = {};
    char MyDims[6] = {};
   
    printf("\n\nPick a theater by entering the zipcode\n\n");
   
   
    InOrder(BSTRoot);
   
   
    printf("\nEnter the zip code of the theater: ");
    scanf("%s", zip);
   
    
    MyTheater = SearchForBNODE(BSTRoot, zip);
   
   
    if (MyTheater == NULL)
    {
        printf("\nNo theater found with the given zip code.\n");
    }
    else
    {
        
        strcpy(MyDims, MyTheater->Dimensions);

        *MapRow = atoi(strtok(MyDims, "x"));

        *MapCol = atoi(strtok(NULL, "x"));
       
        if (ReadMovieTheaterFile(MovieTheaterMap, MyTheater->FileName, *MapRow, *MapCol) == 0)
        {
            printf("\nUnable to print that seat map at this time. Check back later.\n");
        }
        else
        {
         
            PrintSeatMap(MovieTheaterMap, *MapRow, *MapCol);
        }
    }
   
    return MyTheater;
   
}

void ReadFileIntoQueue(FILE *QueueFile, QNODE **QH, QNODE **QT)
{

    char buffer[100];

    while (fgets(buffer, sizeof(buffer), QueueFile) != NULL)
    {
        int len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[ -- len] = '\0';
        }
        enQueue(buffer, QH, QT);
    }

}

void ReadFileIntoBST(FILE *BSTFile, BNODE **BSTnode)
{
    char buffer[100];
    char *MTN, *ZC, *FN, *DIM;
    char MTN_copy[50], ZC_copy[50], FN_copy[20],DIM_copy[20];

    while (fgets(buffer, sizeof(buffer), BSTFile) != NULL)
    {
       
       
        MTN= strtok(buffer, "|");
        ZC = strtok(NULL, "|");
        FN = strtok(NULL, "|");
        DIM= strtok(NULL, "|");
       
        strcpy(MTN_copy, MTN);
        strcpy(ZC_copy, ZC);
        strcpy(FN_copy, FN);
        strcpy(DIM_copy, DIM);
       

        AddBSTNode(BSTnode, MTN_copy, ZC_copy, FN_copy, DIM_copy);
       
    }
}


void get_command_line_parameter(char *argv[], char ParamName[], char ParamValue[])
{
    int i = 0;
   
    while (argv[++i] != NULL)
    {
        if (!strncmp(argv[i], ParamName, strlen(ParamName)))
        {
            strcpy(ParamValue, strchr(argv[i], '=') + 1);
        }
    }
}

int PrintMenu()
{
    int choice = 0;
   
    printf("\n\n1.  Sell tickets to next customer\n\n");
    printf("2.  See who's in line\n\n");
    printf("3.  See the seat map for a given theater\n\n");
    printf("4.  Print today's receipts\n\n");
    printf("Choice : ");
    scanf("%d", &choice);
   
    while (choice < 1 || choice > 4)
    {
        printf("Invalid choice.  Please reenter. ");
        scanf("%d", &choice);
    }
   
    return choice;
}

int main(int argc, char *argv[])
{
    int i, j, k;
    FILE *queueFile = NULL;
    FILE *zipFile = NULL;
    char arg_value[20];
    char queuefilename[20];
    char zipfilename[20];
    int ReceiptNumber = 0;
    int choice = 0;
    int SeatNumber;
    char Row;
    char Ticket[5];
    int ArrayRow, ArrayCol;
    int MapRow, MapCol;
    char MovieTheaterMap[MAXROWS][MAXCOLS] = {};
    LNODE *TicketLinkedListHead = NULL;
    QNODE *QueueHead = NULL;
    QNODE *QueueTail = NULL;
    BNODE *BSTRoot = NULL;
    BNODE *MyTheater = NULL;
    SNODE *StackTop = NULL;
    int NumberOfTickets = 0;
   
    if (argc != 4)
    {
        printf("%s QUEUE=xxxxxx ZIPFILE=xxxxx RECEIPTNUMBER=xxxxx\n", argv[0]);
       
        exit(0);


    }

    get_command_line_parameter(argv, "QUEUE=", queuefilename);
    get_command_line_parameter(argv, "ZIPFILE=", zipfilename);
    get_command_line_parameter(argv, "RECEIPTNUMBER=", arg_value);
    ReceiptNumber = atoi(arg_value);
           
    queueFile = fopen(queuefilename, "r");
    if (!queueFile)
    {
        printf("Error: could not open %s\n", queuefilename);
        exit(0);
    }

    zipFile = fopen(zipfilename, "r");
    if (!zipFile)
    {
        printf("Error: could not open %s\n", zipfilename);
        exit(0);
    }

    ReadFileIntoQueue(queueFile, &QueueHead, &QueueTail);
   
    ReadFileIntoBST(zipFile, &BSTRoot);
   
    while (QueueHead != NULL)
    {
        choice = PrintMenu();
   
        switch (choice)
        {
            case 1 :
                printf("\n\nHello %s\n", QueueHead->name);
                MyTheater = PickAndDisplayTheater(BSTRoot, MovieTheaterMap, &MapRow, &MapCol);
                if (MyTheater != NULL)
                {
                    printf("\n\nHow many movie tickets do you want to buy? ");
                    scanf("%d", &NumberOfTickets);
                    for (i = 0; i < NumberOfTickets; i++)
                    {
                        do
                        {
                            printf("\nPick a seat (Row Seat) ");
                            scanf(" %c%d", &Row, &SeatNumber);
                            Row = toupper(Row);
                            ArrayRow = (int)Row - 65;
                            ArrayCol = SeatNumber - 1;
                       
                            if ((ArrayRow < 0 || ArrayRow >= MapRow) ||
                                (ArrayCol < 0 || ArrayCol >= MapCol))
                            {
                                printf("\nThat is not a valid seat.  Please choose again\n\n");
                            }
                        }
                        while ((ArrayRow < 0 || ArrayRow >= MapRow) ||
                               (ArrayCol < 0 || ArrayCol >= MapCol));
                       
                        if (MovieTheaterMap[ArrayRow][ArrayCol] == 'O')
                        {
                            MovieTheaterMap[ArrayRow][ArrayCol] = 'X';
                            sprintf(Ticket, "%c%d", Row, SeatNumber);
                            InsertNode(&TicketLinkedListHead, Ticket);
                        }
                        else
                        {
                            printf("\nSeat %c%d is not available.\n\n", Row, SeatNumber);
                            i--;
                        }
                        PrintSeatMap(MovieTheaterMap, MapRow, MapCol);
                    }
                   
                    WriteSeatMap(MyTheater, MovieTheaterMap, MapRow, MapCol);
                    push(&StackTop, TicketLinkedListHead, ReceiptNumber, MyTheater->MovieTheaterName);
                    TicketLinkedListHead = NULL;
                    ReceiptNumber++;
                    printf("\nThank you %s - enjoy your movie!\n", QueueHead->name);
                    deQueue(&QueueHead);
                }
                break;
            case 2 :
                printf("\n\nCustomer Queue\n\n");
                DisplayQueue(QueueHead);
                printf("\n\n");
                break;
            case 3 :
                PickAndDisplayTheater(BSTRoot, MovieTheaterMap, &MapRow, &MapCol);
                break;
            case 4 :
                PrintReceipts(&StackTop);
                break;
            default :
                printf("Bad menu choice");
        }
    }
   
    printf("Good job - you sold tickets to all of the customers in line.\n");
    PrintReceipts(&StackTop);
   
    return 0;
}

