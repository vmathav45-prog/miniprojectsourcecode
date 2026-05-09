#include <stdio.h>
#include <stdlib.h>

struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

void searchRecord(FILE *fPtr);
void displayAll(FILE *fPtr);
void deposit(FILE *fPtr);
void withdraw(FILE *fPtr);
void totalAccounts(FILE *fPtr);
void richestCustomer(FILE *fPtr);

int main()
{
    FILE *cfPtr;
    int choice;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("File could not be opened.\n");
        return 1;
    }

    do
    {
        printf("\n====== BANK MANAGEMENT SYSTEM ======\n");
        printf("1. Search Account\n");
        printf("2. Display All Accounts\n");
        printf("3. Deposit Money\n");
        printf("4. Withdraw Money\n");
        printf("5. Total Accounts\n");
        printf("6. Richest Customer\n");
        printf("7. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            searchRecord(cfPtr);
            break;

        case 2:
            displayAll(cfPtr);
            break;

        case 3:
            deposit(cfPtr);
            break;

        case 4:
            withdraw(cfPtr);
            break;

        case 5:
            totalAccounts(cfPtr);
            break;

        case 6:
            richestCustomer(cfPtr);
            break;

        case 7:
            printf("Program Ended\n");
            break;

        default:
            printf("Invalid Choice\n");
        }

    } while (choice != 7);

    fclose(cfPtr);

    return 0;
}

/* SEARCH ACCOUNT */
void searchRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int account;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr,
          (long)(account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\nAccount Found\n");
        printf("Account Number : %u\n", client.acctNum);
        printf("First Name     : %s\n", client.firstName);
        printf("Last Name      : %s\n", client.lastName);
        printf("Balance        : %.2lf\n", client.balance);
    }
}

/* DISPLAY ALL ACCOUNTS */
void displayAll(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    rewind(fPtr);

    printf("\n===== ACCOUNT DETAILS =====\n");

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%u  %s  %s  %.2lf\n",
                   client.acctNum,
                   client.firstName,
                   client.lastName,
                   client.balance);
        }
    }
}

/* DEPOSIT MONEY */
void deposit(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    unsigned int account;
    double amount;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr,
          (long)(account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
    }
    else
    {
        printf("Enter amount to deposit: ");
        scanf("%lf", &amount);

        client.balance += amount;

        fseek(fPtr,
              -(long)sizeof(struct clientData),
              SEEK_CUR);

        fwrite(&client,
               sizeof(struct clientData),
               1,
               fPtr);

        printf("Deposit Successful\n");
        printf("Updated Balance = %.2lf\n",
               client.balance);
    }
}

/* WITHDRAW MONEY */
void withdraw(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    unsigned int account;
    double amount;

    printf("Enter account number: ");
    scanf("%u", &account);

    fseek(fPtr,
          (long)(account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
    }
    else
    {
        printf("Enter amount to withdraw: ");
        scanf("%lf", &amount);

        if (amount > client.balance)
        {
            printf("Insufficient Balance\n");
        }
        else
        {
            client.balance -= amount;

            fseek(fPtr,
                  -(long)sizeof(struct clientData),
                  SEEK_CUR);

            fwrite(&client,
                   sizeof(struct clientData),
                   1,
                   fPtr);

            printf("Withdrawal Successful\n");
            printf("Remaining Balance = %.2lf\n",
                   client.balance);
        }
    }
}

/* TOTAL ACCOUNTS */
void totalAccounts(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    int count = 0;

    rewind(fPtr);

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            count++;
        }
    }

    printf("Total Active Accounts = %d\n", count);
}

/* RICHEST CUSTOMER */
void richestCustomer(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    struct clientData richClient = {0, "", "", 0.0};

    rewind(fPtr);

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        if (client.balance > richClient.balance)
        {
            richClient = client;
        }
    }

    if (richClient.acctNum != 0)
    {
        printf("\n===== RICHEST CUSTOMER =====\n");

        printf("Account Number : %u\n",
               richClient.acctNum);

        printf("Customer Name  : %s %s\n",
               richClient.firstName,
               richClient.lastName);

        printf("Balance        : %.2lf\n",
               richClient.balance);
    }
    else
    {
        printf("No accounts found.\n");
    }
}