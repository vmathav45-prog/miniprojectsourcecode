#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================= STRUCTURES =================
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
    int pin;
};

struct transaction
{
    unsigned int acctNum;
    char type[20];
    double amount;
};

// ================= PROTOTYPES =================
unsigned int enterChoice(void);
void createAccount(FILE *fPtr);
void deposit(FILE *fPtr);
void withdraw(FILE *fPtr);
void transfer(FILE *fPtr);   // NEW
void checkBalance(FILE *fPtr);
void deleteAccount(FILE *fPtr);
void textFile(FILE *readPtr);
int verifyPIN(struct clientData client);
void recoverPIN(FILE *fPtr);
void displayAll(FILE *fPtr);

void addTransaction(unsigned int acc, char type[], double amount);
void miniStatement(unsigned int acc);

int isValidAccount(unsigned int acc);

// ================= MAIN =================
int main()
{
    FILE *cfPtr;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");
    }

    int choice;

    while ((choice = enterChoice()) != 11)
    {
        switch (choice)
        {
        case 1: createAccount(cfPtr); break;
        case 2: deposit(cfPtr); break;
        case 3: withdraw(cfPtr); break;
        case 4: transfer(cfPtr); break; // NEW
        case 5: checkBalance(cfPtr); break;
        case 6: deleteAccount(cfPtr); break;
        case 7: textFile(cfPtr); break;
        case 8: recoverPIN(cfPtr); break;
        case 9:
        {
            unsigned int acc;
            printf("Enter account number: ");
            scanf("%u", &acc);
            miniStatement(acc);
            break;
        }
        case 10: displayAll(cfPtr); break;

        default: printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);
    printf("Thank you for using Bank System!\n");
    return 0;
}

// ================= VALIDATION =================
int isValidAccount(unsigned int acc)
{
    if (acc < 1 || acc > 100)
    {
        printf("Invalid account number! (1-100 only)\n");
        return 0;
    }
    return 1;
}

// ================= VERIFY PIN =================
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

// ================= CREATE ACCOUNT =================
void createAccount(FILE *fPtr)
{
    struct clientData client = {0};
    unsigned int acc;

    printf("Enter account number (1-100): ");
    scanf("%u", &acc);

    if (!isValidAccount(acc)) return;

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

// ================= DEPOSIT =================
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

    printf("Enter amount: ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Invalid amount!\n");
        return;
    }

    client.balance += amount;

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    addTransaction(acc, "Deposit", amount);

    printf("Deposit successful!\n");
}

// ================= WITHDRAW =================
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

    printf("Enter amount: ");
    scanf("%lf", &amount);

    if (amount <= 0 || amount > client.balance)
    {
        printf("Invalid or insufficient balance!\n");
        return;
    }

    client.balance -= amount;

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    addTransaction(acc, "Withdraw", amount);

    printf("Withdraw successful!\n");
}

// ================= TRANSFER =================
void transfer(FILE *fPtr)
{
    struct clientData sender, receiver;
    unsigned int from, to;
    double amount;

    printf("Enter sender account: ");
    scanf("%u", &from);

    printf("Enter receiver account: ");
    scanf("%u", &to);

    if (!isValidAccount(from) || !isValidAccount(to)) return;

    // Read sender
    fseek(fPtr, (from - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&sender, sizeof(struct clientData), 1, fPtr);

    // Read receiver
    fseek(fPtr, (to - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&receiver, sizeof(struct clientData), 1, fPtr);

    if (sender.acctNum == 0 || receiver.acctNum == 0)
    {
        printf("One of the accounts not found!\n");
        return;
    }

    if (!verifyPIN(sender)) return;

    printf("Enter amount: ");
    scanf("%lf", &amount);

    if (amount <= 0 || amount > sender.balance)
    {
        printf("Invalid or insufficient balance!\n");
        return;
    }

    // Update balances
    sender.balance -= amount;
    receiver.balance += amount;

    // Write sender
    fseek(fPtr, (from - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&sender, sizeof(struct clientData), 1, fPtr);

    // Write receiver
    fseek(fPtr, (to - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&receiver, sizeof(struct clientData), 1, fPtr);

    addTransaction(from, "Transfer Out", amount);
    addTransaction(to, "Transfer In", amount);

    printf("Transfer successful!\n");
}

// ================= DISPLAY ALL =================
void displayAll(FILE *fPtr)
{
    struct clientData client;
    rewind(fPtr);

    printf("\nAcc   First      Last       Balance\n");
    printf("-----------------------------------\n");

    while (fread(&client, sizeof(struct clientData), 1, fPtr))
    {
        if (client.acctNum != 0)
        {
            printf("%-5d %-10s %-10s %-10.2f\n",
                   client.acctNum,
                   client.firstName,
                   client.lastName,
                   client.balance);
        }
    }
}

// ================= MINI STATEMENT =================
void miniStatement(unsigned int acc)
{
    FILE *tPtr = fopen("transactions.dat", "rb");

    if (!tPtr)
    {
        printf("No transactions found!\n");
        return;
    }

    struct transaction t;
    while (fread(&t, sizeof(struct transaction), 1, tPtr))
    {
        if (t.acctNum == acc)
        {
            printf("%s : %.2f\n", t.type, t.amount);
        }
    }

    fclose(tPtr);
}

// ================= TRANSACTION =================
void addTransaction(unsigned int acc, char type[], double amount)
{
    FILE *tPtr = fopen("transactions.dat", "ab");

    struct transaction t;
    t.acctNum = acc;
    strcpy(t.type, type);
    t.amount = amount;

    fwrite(&t, sizeof(struct transaction), 1, tPtr);
    fclose(tPtr);
}

// ================= CHECK BALANCE =================
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

    printf("Name: %s %s\nBalance: %.2f\n",
           client.firstName,
           client.lastName,
           client.balance);
}

// ================= DELETE ACCOUNT =================
void deleteAccount(FILE *fPtr)
{
    struct clientData client = {0};
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

    client.acctNum = 0;

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted successfully!\n");
}

// ================= FORMAL REPORT =================
void textFile(FILE *readPtr)
{
    FILE *writePtr = fopen("accounts.txt", "w");

    if (writePtr == NULL)
    {
        printf("Error creating report file!\n");
        return;
    }

    struct clientData client;

    rewind(readPtr);

    // ===== HEADER =====
    fprintf(writePtr, "=============================================\n");
    fprintf(writePtr, "           BANK ACCOUNT REPORT\n");
    fprintf(writePtr, "=============================================\n\n");

    fprintf(writePtr, "%-10s %-15s %-15s %-10s\n",
            "Acc No", "First Name", "Last Name", "Balance");

    fprintf(writePtr, "-------------------------------------------------------------\n");

    // ===== DATA =====
    while (fread(&client, sizeof(struct clientData), 1, readPtr))
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-10d %-15s %-15s %-10.2f\n",
                    client.acctNum,
                    client.firstName,
                    client.lastName,
                    client.balance);
        }
    }

    // ===== FOOTER =====
    fprintf(writePtr, "\n=============================================\n");
    fprintf(writePtr, "        END OF REPORT\n");
    fprintf(writePtr, "=============================================\n");

    fclose(writePtr);

    printf("Formal report generated successfully!\n");
}

// ================= RECOVER PIN =================
void recoverPIN(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;
    char fname[10];

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found!\n");
        return;
    }
    printf("Enter First Name: ");
    scanf("%s", fname);

    if (strcmp(fname, client.firstName) == 0)
        printf("Your PIN is: %d\n", client.pin);
    else
        printf("Verification failed!\n");
}
// ================= MENU =================
unsigned int enterChoice(void)
{
    printf("\n===================================\n");
    printf("          BANK SYSTEM             ");
    printf("\n===================================\n");
    printf("1. Create Account\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Transfer Money\n");
    printf("5. Check Balance\n");
    printf("6. Delete Account\n");
    printf("7. Generate Report\n");
    printf("8. Recover PIN\n");
    printf("9. Mini Statement\n");
    printf("10. Display All Accounts\n");
    printf("11. Exit\n");
    printf("==================================\n");
    printf("Enter choice: ");
    int choice;
    scanf("%d", &choice);
    return choice;
}