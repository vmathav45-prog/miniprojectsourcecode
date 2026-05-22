// ...existing code...
/*
  Bank Management System (Single-file C program)
  - Features: Create account, Display all, Search, Deposit, Withdraw,
    Check balance, Update details, Delete account, SIP calculator,
    GST calculator, Transaction history, Mini statement, Admin login.
  - Uses: structures, functions, arrays, loops, conditionals, file handling
  - Files used: accounts.dat (binary), transactions.dat (binary)

  Compile: gcc trans.c -o bank
  Run: ./bank
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// ------------------------- Data Structures -------------------------
// Account structure holds customer details and balance
typedef struct {
    int accNo;
    char name[50];
    int age;
    char gender[10];
    char phone[20];
    char address[100];
    char accType[10]; // Savings or Current
    char password[20];
    double balance;
} Account;

// Transaction structure stores individual transaction records
typedef struct {
    int accNo;
    char type[10]; // Deposit or Withdraw
    double amount;
    double balanceAfter;
    char datetime[30];
} Transaction;

// ------------------------- File Names -------------------------
const char *ACCOUNTS_FILE = "accounts.dat";
const char *TRANSACTIONS_FILE = "transactions.dat";

// ------------------------- Utility Functions -------------------------
// Get current date-time as string
void getDateTime(char *buffer, int len) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(buffer, len, "%Y-%m-%d %H:%M:%S", &tm);
}

// Generate unique account number by scanning existing accounts
int generateAccountNumber() {
    FILE *fp = fopen(ACCOUNTS_FILE, "rb");
    int max = 999; // start from 1000
    if (fp == NULL) return 1000;
    Account a;
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        if (a.accNo > max) max = a.accNo;
    }
    fclose(fp);
    return max + 1;
}

// Read a line safely into buffer
void readLine(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t ln = strlen(buffer);
        if (ln > 0 && buffer[ln - 1] == '\n') buffer[ln - 1] = '\0';
    }
}

// Find account by account number. Returns 1 if found and fills acc, else 0.
int findAccount(int accNo, Account *acc, long *position) {
    FILE *fp = fopen(ACCOUNTS_FILE, "rb");
    if (!fp) return 0;
    Account a; long pos = 0;
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        if (a.accNo == accNo) {
            if (acc) *acc = a;
            if (position) *position = pos;
            fclose(fp);
            return 1;
        }
        pos++;
    }
    fclose(fp);
    return 0;
}

// Validate account number and password
int validateCredentials(int accNo, const char *password) {
    Account a; long pos;
    if (!findAccount(accNo, &a, &pos)) return 0;
    if (strcmp(a.password, password) == 0) return 1;
    return 0;
}

// Record a transaction in transactions.dat
void recordTransaction(int accNo, const char *type, double amount, double balanceAfter) {
    Transaction t;
    t.accNo = accNo;
    strncpy(t.type, type, sizeof(t.type)-1);
    t.type[sizeof(t.type)-1] = '\0';
    t.amount = amount;
    t.balanceAfter = balanceAfter;
    getDateTime(t.datetime, sizeof(t.datetime));
    FILE *fp = fopen(TRANSACTIONS_FILE, "ab");
    if (!fp) return;
    fwrite(&t, sizeof(Transaction), 1, fp);
    fclose(fp);
}

// ------------------------- Core Operations -------------------------

// 1. Create New Account
void createAccount() {
    Account a;
    printf("\n----- Create New Account -----\n");
    a.accNo = generateAccountNumber();
    printf("Assigned Account Number: %d\n", a.accNo);
    printf("Enter Customer Name: ");
    readLine(a.name, sizeof(a.name));
    printf("Enter Age: ");
    if (scanf("%d", &a.age) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    getchar();
    printf("Enter Gender: ");
    readLine(a.gender, sizeof(a.gender));
    printf("Enter Phone Number: ");
    readLine(a.phone, sizeof(a.phone));
    printf("Enter Address: ");
    readLine(a.address, sizeof(a.address));
    printf("Enter Account Type (Savings/Current): ");
    readLine(a.accType, sizeof(a.accType));
    printf("Set Password: ");
    readLine(a.password, sizeof(a.password));
    printf("Enter Initial Deposit Amount: ");
    if (scanf("%lf", &a.balance) != 1) { while (getchar() != '\n'); printf("Invalid amount.\n"); return; }
    getchar();

    FILE *fp = fopen(ACCOUNTS_FILE, "ab");
    if (!fp) {
        printf("Error opening accounts file.\n");
        return;
    }
    fwrite(&a, sizeof(Account), 1, fp);
    fclose(fp);
    recordTransaction(a.accNo, "Deposit", a.balance, a.balance);
    printf("Account created successfully!\n");
}

// 2. Display All Account Details
void displayAllAccounts() {
    printf("\n----- All Accounts -----\n");
    FILE *fp = fopen(ACCOUNTS_FILE, "rb");
    if (!fp) {
        printf("No accounts found.\n");
        return;
    }
    Account a;
    printf("%-8s %-20s %-4s %-8s %-12s %-10s %-8s\n", "AccNo", "Name", "Age", "Gender", "Phone", "Type", "Balance");
    printf("--------------------------------------------------------------------------------\n");
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        printf("%-8d %-20s %-4d %-8s %-12s %-10s %-8.2f\n", a.accNo, a.name, a.age, a.gender, a.phone, a.accType, a.balance);
    }
    fclose(fp);
}

// 3. Search Account using Account Number
void searchAccount() {
    printf("\n----- Search Account -----\n");
    int accNo; printf("Enter Account Number: "); if (scanf("%d", &accNo) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    Account a; long pos;
    if (findAccount(accNo, &a, &pos)) {
        printf("Account found:\n");
        printf("Account Number: %d\nName: %s\nAge: %d\nGender: %s\nPhone: %s\nAddress: %s\nType: %s\nBalance: %.2f\n", a.accNo, a.name, a.age, a.gender, a.phone, a.address, a.accType, a.balance);
    } else printf("Account not found.\n");
}

// 4. Deposit Money
void depositMoney() {
    printf("\n----- Deposit Money -----\n");
    int accNo; char pwd[20];
    printf("Enter Account Number: "); if (scanf("%d", &accNo) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    printf("Enter Password: "); readLine(pwd, sizeof(pwd));
    if (!validateCredentials(accNo, pwd)) { printf("Invalid account number or password.\n"); return; }
    double amount; printf("Enter amount to deposit: "); if (scanf("%lf", &amount) != 1) { while (getchar() != '\n'); printf("Invalid amount.\n"); return; } getchar();
    if (amount <= 0) { printf("Invalid amount.\n"); return; }
    // Update account in file
    FILE *fp = fopen(ACCOUNTS_FILE, "r+b");
    if (!fp) { printf("Accounts file error.\n"); return; }
    Account a; long pos = 0; int found = 0;
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        if (a.accNo == accNo) {
            a.balance += amount;
            fseek(fp, pos * sizeof(Account), SEEK_SET);
            fwrite(&a, sizeof(Account), 1, fp);
            found = 1; break;
        }
        pos++;
    }
    fclose(fp);
    if (found) {
        recordTransaction(accNo, "Deposit", amount, a.balance);
        printf("Deposit successful. Updated Balance: %.2f\n", a.balance);
    } else printf("Account not found for deposit.\n");
}

// 5. Withdraw Money
void withdrawMoney() {
    printf("\n----- Withdraw Money -----\n");
    int accNo; char pwd[20];
    printf("Enter Account Number: "); if (scanf("%d", &accNo) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    printf("Enter Password: "); readLine(pwd, sizeof(pwd));
    if (!validateCredentials(accNo, pwd)) { printf("Invalid account number or password.\n"); return; }
    double amount; printf("Enter amount to withdraw: "); if (scanf("%lf", &amount) != 1) { while (getchar() != '\n'); printf("Invalid amount.\n"); return; } getchar();
    if (amount <= 0) { printf("Invalid amount.\n"); return; }
    FILE *fp = fopen(ACCOUNTS_FILE, "r+b");
    if (!fp) { printf("Accounts file error.\n"); return; }
    Account a; long pos = 0; int found = 0;
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        if (a.accNo == accNo) {
            if (a.balance < amount) { printf("Insufficient balance. Current Balance: %.2f\n", a.balance); fclose(fp); return; }
            a.balance -= amount;
            fseek(fp, pos * sizeof(Account), SEEK_SET);
            fwrite(&a, sizeof(Account), 1, fp);
            found = 1; break;
        }
        pos++;
    }
    fclose(fp);
    if (found) {
        recordTransaction(accNo, "Withdraw", amount, a.balance);
        printf("Withdrawal successful. Updated Balance: %.2f\n", a.balance);
    } else printf("Account not found for withdrawal.\n");
}

// 6. Check Balance
void checkBalance() {
    printf("\n----- Check Balance -----\n");
    int accNo; char pwd[20];
    printf("Enter Account Number: "); if (scanf("%d", &accNo) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    printf("Enter Password: "); readLine(pwd, sizeof(pwd));
    if (!validateCredentials(accNo, pwd)) { printf("Invalid account number or password.\n"); return; }
    Account a; long pos;
    findAccount(accNo, &a, &pos);
    printf("Current Balance: %.2f\n", a.balance);
}

// 7. Update Account Details
void updateAccountDetails() {
    printf("\n----- Update Account Details -----\n");
    int accNo; char pwd[20];
    printf("Enter Account Number: "); if (scanf("%d", &accNo) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    printf("Enter Password: "); readLine(pwd, sizeof(pwd));
    if (!validateCredentials(accNo, pwd)) { printf("Invalid account number or password.\n"); return; }
    FILE *fp = fopen(ACCOUNTS_FILE, "r+b");
    if (!fp) { printf("Accounts file error.\n"); return; }
    Account a; long pos = 0; int found = 0;
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        if (a.accNo == accNo) {
            printf("Updating details for %s (leave blank to keep unchanged)\n", a.name);
            char input[120];
            printf("New Name: "); readLine(input, sizeof(input)); if (strlen(input)) strncpy(a.name, input, sizeof(a.name)-1);
            printf("New Phone: "); readLine(input, sizeof(input)); if (strlen(input)) strncpy(a.phone, input, sizeof(a.phone)-1);
            printf("New Address: "); readLine(input, sizeof(input)); if (strlen(input)) strncpy(a.address, input, sizeof(a.address)-1);
            printf("New Account Type: "); readLine(input, sizeof(input)); if (strlen(input)) strncpy(a.accType, input, sizeof(a.accType)-1);
            printf("New Password: "); readLine(input, sizeof(input)); if (strlen(input)) strncpy(a.password, input, sizeof(a.password)-1);
            fseek(fp, pos * sizeof(Account), SEEK_SET);
            fwrite(&a, sizeof(Account), 1, fp);
            found = 1; break;
        }
        pos++;
    }
    fclose(fp);
    if (found) printf("Account updated successfully.\n"); else printf("Account not found.\n");
}

// 8. Delete Account
void deleteAccount() {
    printf("\n----- Delete Account -----\n");
    int accNo; char confirm[4];
    printf("Enter Account Number to delete: "); if (scanf("%d", &accNo) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    printf("Are you sure? This action is permanent (yes/no): "); readLine(confirm, sizeof(confirm));
    if (strcmp(confirm, "yes") != 0) { printf("Deletion cancelled.\n"); return; }
    FILE *fp = fopen(ACCOUNTS_FILE, "rb");
    if (!fp) { printf("No accounts file found.\n"); return; }
    FILE *temp = fopen("temp.dat", "wb");
    Account a; int deleted = 0;
    while (fread(&a, sizeof(Account), 1, fp) == 1) {
        if (a.accNo == accNo) { deleted = 1; continue; }
        fwrite(&a, sizeof(Account), 1, temp);
    }
    fclose(fp); fclose(temp);
    remove(ACCOUNTS_FILE); rename("temp.dat", ACCOUNTS_FILE);
    if (deleted) printf("Account deleted successfully.\n"); else printf("Account not found.\n");
}

// 9. SIP Investment Calculator
void sipCalculator() {
    printf("\n----- SIP Investment Calculator -----\n");
    double monthly; double annualRate; int years;
    printf("Enter monthly investment amount: "); if (scanf("%lf", &monthly) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    printf("Enter annual interest rate (percent): "); if (scanf("%lf", &annualRate) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    printf("Enter investment duration (years): "); if (scanf("%d", &years) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    int months = years * 12;
    double r = annualRate / 100.0;
    double monthlyRate = r / 12.0;
    // Future value of series formula: P * [ ((1+i)^n -1)/i ] * (1+i)
    double factor = 0.0;
    if (monthlyRate != 0) factor = (pow(1 + monthlyRate, months) - 1) / monthlyRate * (1 + monthlyRate);
    else factor = months;
    double maturity = monthly * factor;
    double invested = monthly * months;
    double returns = maturity - invested;
    printf("Total Invested Amount: %.2f\n", invested);
    printf("Estimated Returns: %.2f\n", returns);
    printf("Maturity Amount: %.2f\n", maturity);
}

// 10. GST Calculator
void gstCalculator() {
    printf("\n----- GST Calculator -----\n");
    double amount; double gstPercent;
    printf("Enter product amount: "); if (scanf("%lf", &amount) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    printf("Enter GST percentage (e.g., 18): "); if (scanf("%lf", &gstPercent) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    double gst = amount * gstPercent / 100.0;
    double cgst = gst / 2.0;
    double sgst = gst / 2.0;
    double finalAmount = amount + gst;
    printf("CGST: %.2f\nSGST: %.2f\nFinal Amount (including GST): %.2f\n", cgst, sgst, finalAmount);
}

// 11. Transaction History (all transactions for account)
void transactionHistory() {
    printf("\n----- Transaction History -----\n");
    int accNo; if (scanf("%d", &accNo) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    FILE *fp = fopen(TRANSACTIONS_FILE, "rb");
    if (!fp) { printf("No transaction history found.\n"); return; }
    Transaction t; int found = 0;
    printf("Date/Time             Type      Amount     BalanceAfter\n");
    printf("-----------------------------------------------------------\n");
    while (fread(&t, sizeof(Transaction), 1, fp) == 1) {
        if (t.accNo == accNo) {
            printf("%-20s %-9s %-10.2f %-10.2f\n", t.datetime, t.type, t.amount, t.balanceAfter);
            found = 1;
        }
    }
    fclose(fp);
    if (!found) printf("No transactions found for this account.\n");
}

// 12. Mini Statement (last 5 transactions)
void miniStatement() {
    printf("\n----- Mini Statement (Last 5) -----\n");
    int accNo; if (scanf("%d", &accNo) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; } getchar();
    FILE *fp = fopen(TRANSACTIONS_FILE, "rb");
    if (!fp) { printf("No transaction history found.\n"); return; }
    // Read all matching transactions into an array (dynamic)
    Transaction *arr = NULL; size_t count = 0;
    Transaction t;
    while (fread(&t, sizeof(Transaction), 1, fp) == 1) {
        if (t.accNo == accNo) {
            Transaction *tmp = realloc(arr, (count+1) * sizeof(Transaction));
            if (!tmp) { free(arr); fclose(fp); printf("Memory error.\n"); return; }
            arr = tmp;
            arr[count++] = t;
        }
    }
    fclose(fp);
    if (count == 0) { printf("No transactions found for this account.\n"); free(arr); return; }
    size_t start = (count > 5) ? (count - 5) : 0;
    printf("Date/Time             Type      Amount     BalanceAfter\n");
    printf("-----------------------------------------------------------\n");
    for (size_t i = start; i < count; ++i) {
        printf("%-20s %-9s %-10.2f %-10.2f\n", arr[i].datetime, arr[i].type, arr[i].amount, arr[i].balanceAfter);
    }
    free(arr);
}

// 13. Admin Login System
int adminLogin() {
    const char *ADMIN_USER = "admin";
    const char *ADMIN_PASS = "admin123";
    char user[30], pass[30];
    printf("\n----- Admin Login -----\n");
    printf("Username: "); readLine(user, sizeof(user));
    printf("Password: "); readLine(pass, sizeof(pass));
    if (strcmp(user, ADMIN_USER) == 0 && strcmp(pass, ADMIN_PASS) == 0) {
        printf("Admin login successful.\n");
        return 1;
    }
    printf("Invalid admin credentials.\n");
    return 0;
}

void adminMenu() {
    if (!adminLogin()) return;
    int choice;
    do {
        printf("\n--- Admin Menu ---\n");
        printf("1. Display All Accounts\n2. Search Account\n3. Delete Account\n4. Back to Main Menu\nChoice: ");
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
        getchar();
        switch (choice) {
            case 1: displayAllAccounts(); break;
            case 2: searchAccount(); break;
            case 3: deleteAccount(); break;
            case 4: return;
            default: printf("Invalid option.\n");
        }
    } while (1);
}

// 14. Exit handled in main menu

// ------------------------- Main Menu -------------------------
void showMenu() {
    printf("\n===============================================\n");
    printf("       Welcome to Mini Bank Management System\n");
    printf("===============================================\n");
    printf("1. Create New Account\n");
    printf("2. Display All Account Details\n");
    printf("3. Search Account by Account Number\n");
    printf("4. Deposit Money\n");
    printf("5. Withdraw Money\n");
    printf("6. Check Balance\n");
    printf("7. Update Account Details\n");
    printf("8. Delete Account\n");
    printf("9. SIP Investment Calculator\n");
    printf("10. GST Calculator\n");
    printf("11. Transaction History\n");
    printf("12. Mini Statement\n");
    printf("13. Admin Login System\n");
    printf("14. Exit\n");
    printf("===============================================\n");
}

int main() {
    int choice;
    do {
        showMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) { // clear invalid input
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        getchar();
        switch (choice) {
            case 1: createAccount(); break;
            case 2: displayAllAccounts(); break;
            case 3: searchAccount(); break;
            case 4: depositMoney(); break;
            case 5: withdrawMoney(); break;
            case 6: checkBalance(); break;
            case 7: updateAccountDetails(); break;
            case 8: deleteAccount(); break;
            case 9: sipCalculator(); break;
            case 10: gstCalculator(); break;
            case 11: transactionHistory(); break;
            case 12: miniStatement(); break;
            case 13: adminMenu(); break;
            case 14: printf("Exiting. Thank you for using Mini Bank System.\n"); break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 14);
    return 0;
}
// ...existing code...