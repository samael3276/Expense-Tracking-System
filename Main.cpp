#include "ExpenseTracker.h"

// ============================================================
//  MAIN
// ============================================================

int main()
{
    system("mode con: cols=105 lines=40");
    SetConsoleTitle(TEXT("Daily Expense Tracking System"));

    // ── Authentication gate (initFiles called AFTER login) ───
    if (!runAuthScreen()) return 0;

    // Now currentUser is set — create this user's files if needed
    initFiles();

    int choice;
    do
    {
        showMenu();
        choice = getIntInput("Enter your choice: ", 0, 8);

        if      (choice == 1) addExpense();
        else if (choice == 2) viewExpenses();
        else if (choice == 3) viewByCategory();
        else if (choice == 4) searchByID();
        else if (choice == 5) deleteExpense();
        else if (choice == 6) showSummary();
        else if (choice == 7) exportReport();
        else if (choice == 8) setBudget();
        else if (choice == 0)
        {
            showBanner();
            setColor(CLR_SUCCESS);
            cout << "  Thank you, " << currentUser << "! Goodbye!\n\n";
            resetColor();
        }

        if (choice != 0)
        {
            setColor(CLR_WARNING);
            cout << "\n  Press Enter to return to menu...";
            resetColor();
            cin.get();
        }

    } while (choice != 0);

    return 0;
}
