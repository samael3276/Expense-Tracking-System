#ifndef EXPENSE_TRACKER_H
#define EXPENSE_TRACKER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <windows.h>
#include <conio.h>      // _getch() for masked password input
using namespace std;

// ============================================================
//  STRUCT
// ============================================================

struct Expense
{
    int id;
    string date;
    string category;
    string item;
    double amount;
    string paymentMethod;
    string priority;
    string notes;
};

// ============================================================
//  FILE CONSTANTS
// ============================================================

// ── Shared file (one for whole app) ─────────────────────────
const string FILE_USERS = "users.txt";

// ── Per-user file names (built at runtime using currentUser) ─
extern string currentUser;

inline string FILE_ALL()       { return currentUser + "_all_expenses.txt"; }
inline string FILE_FOOD()      { return currentUser + "_food.txt"; }
inline string FILE_TRANSPORT() { return currentUser + "_transport.txt"; }
inline string FILE_HEALTH()    { return currentUser + "_health.txt"; }
inline string FILE_EDUCATION() { return currentUser + "_education.txt"; }
inline string FILE_SHOPPING()  { return currentUser + "_shopping.txt"; }
inline string FILE_OTHER()     { return currentUser + "_other.txt"; }
inline string FILE_REPORT()    { return currentUser + "_summary_report.txt"; }
inline string FILE_BUDGET()    { return currentUser + "_budget.txt"; }

// ============================================================
//  COLOR CONSTANTS
// ============================================================

#define CLR_RESET   7   // White (default)
#define CLR_TITLE   11  // Bright Cyan
#define CLR_HEADER  14  // Bright Yellow
#define CLR_MENU    10  // Bright Green
#define CLR_ERROR   4   // Red
#define CLR_SUCCESS 2   // Green
#define CLR_BORDER  9   // Blue
#define CLR_DATA    3   // Cyan
#define CLR_LABEL   13  // Magenta
#define CLR_WARNING 6   // Yellow

// ============================================================
//  FUNCTION PROTOTYPES
// ============================================================

// Console GUI helpers
void setColor(int color);
void resetColor();
void clearScreen();
void drawTopBorder(int width);
void drawBottomBorder(int width);
void drawMidBorder(int width);
void drawRow(string content, int width);
void showBanner();

// Input helpers
int    getIntInput(string prompt, int lo, int hi);
double getDoubleInput(string prompt);
string getStringInput(string prompt);

// Display helpers
void printLine();
void printHeader(string title);
void showMenu();
void printExpenses(vector<Expense> list);

// Category helpers
void   showCategories();
string getCategory(int choice);
string getCategoryFile(string category);

// File helpers
void            initFiles();
string          toCSV(Expense e);
Expense         fromCSV(string line);
void            saveExpense(Expense e);
vector<Expense> loadExpenses();
vector<Expense> loadByCategory(string category);
void            rewriteFile(vector<Expense> list);
int             getNextID();

// Budget helpers
double loadBudget();
void   saveBudget(double budget);
void   setBudget();
void   checkBudgetWarning(double newExpenseAmount);

// Login / Auth helpers
string  getPasswordInput(string prompt);
int     simpleHash(string s);
string  hashToString(int h);
bool    userExists(string username);
bool    registerUser(string username, string password);
bool    validateLogin(string username, string password);
void    showLoginBanner();
bool    showLoginPage();
bool    showRegisterPage();
bool    runAuthScreen();      // entry point: returns true when authenticated

// Core functions
void addExpense();
void viewExpenses();
void viewByCategory();
void deleteExpense();
void searchByID();
void showSummary();
void exportReport();

#endif // EXPENSE_TRACKER_H
