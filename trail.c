#include <stdio.h>
#include <stdlib.h>

// structure
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
    int pin;
};

// function prototypes
unsigned int enterChoice(void);
void createAccount(FILE *fPtr);
void deposit(FILE *fPtr);
void withdraw(FILE *fPtr);
void checkBalance(FILE *fPtr);
void deleteAccount(FILE *fPtr);
void textFile(FILE *readPtr);
int verifyPIN(struct clientData client);

int main()
{
    FILE *cfPtr;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("File could not be opened.\n");
        exit(1);
    }

    int choice;

    while ((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        case 1:
            createAccount(cfPtr);
            break;
        case 2:
            deposit(cfPtr);
            break;
        case 3:
            withdraw(cfPtr);
            break;
        case 4:
            checkBalance(cfPtr);
            break;
        case 5:
            deleteAccount(cfPtr);
            break;
        case 6:
            textFile(cfPtr);
            break;
        default:
            printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);
    return 0;
}


// verify PIN
int verifyPIN(struct clientData client)
{
    int pin;
    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (pin == client.pin)
        return 1;

    printf("Incorrect PIN!\n");
    return 0;
}

// create account
void createAccount(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0, 0};
    unsigned int acc;

    printf("Enter account number (1-100): ");
    scanf("%u", &acc);

    if (acc < 1 || acc > 100)
    {
        printf("Invalid account number\n");
        return;
    }

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists!\n");
    }
    else
    {
        printf("Enter LastName FirstName Balance PIN:\n");
        scanf("%s %s %lf %d",
              client.lastName,
              client.firstName,
              &client.balance,
              &client.pin);

        client.acctNum = acc;

        fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);

        printf("Account created successfully!\n");
    }
}

// deposit
void deposit(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;
    double amount;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    if (!verifyPIN(client)) return;

    printf("Enter deposit amount: ");
    scanf("%lf", &amount);

    client.balance += amount;

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Deposit successful! New Balance: %.2f\n", client.balance);
}

// withdraw
void withdraw(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;
    double amount;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    if (!verifyPIN(client)) return;

    printf("Enter withdraw amount: ");
    scanf("%lf", &amount);

    if (amount > client.balance)
    {
        printf("Insufficient balance!\n");
        return;
    }

    client.balance -= amount;

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Withdraw successful! New Balance: %.2f\n", client.balance);
}

// check balance
void checkBalance(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }

    if (!verifyPIN(client)) return;

    printf("\nAccount Details:\n");
    printf("Name: %s %s\n", client.firstName, client.lastName);
    printf("Balance: %.2f\n", client.balance);
}

// delete account
void deleteAccount(FILE *fPtr)
{
    struct clientData client, blank = {0, "", "", 0.0, 0};
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist!\n");
        return;
    }

    if (!verifyPIN(client)) return;

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blank, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted successfully!\n");
}

// generate report
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client = {0, "", "", 0.0, 0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    }
    else
    {
        rewind(readPtr);

        fprintf(writePtr, "%-6s %-16s %-11s %10s\n",
                "Acct", "Last Name", "First Name", "Balance");

        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
        {
            if (client.acctNum != 0)
            {
                fprintf(writePtr, "%-6d %-16s %-11s %10.2f\n",
                        client.acctNum,
                        client.lastName,
                        client.firstName,
                        client.balance);
            }
        }

        fclose(writePtr);
        printf("Report generated successfully (accounts.txt)\n");
    }
}
// menu
unsigned int enterChoice(void)
{
    printf("\n==============================\n");
    printf("   BANK MANAGEMENT SYSTEM\n");
    printf("==============================\n");
    printf("1. Create Account\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Check Balance\n");
    printf("5. Delete Account\n");
    printf("6. Generate Report\n");
    printf("7. Exit\n");
    printf("==============================\n");
    printf("Enter choice: ");

    int choice;
    scanf("%d", &choice);
    return choice;
}
