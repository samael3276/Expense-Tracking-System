#include "ExpenseTracker.h"

// ============================================================
//  CONSOLE GUI HELPERS
// ============================================================

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetColor()
{
    setColor(CLR_RESET);
}

void clearScreen()
{
    system("cls");
}

void drawTopBorder(int width)
{
    setColor(CLR_BORDER);
    cout << "  " << (char)201;
    for (int i = 0; i < width - 2; i++) cout << (char)205;
    cout << (char)187 << "\n";
    resetColor();
}

void drawBottomBorder(int width)
{
    setColor(CLR_BORDER);
    cout << "  " << (char)200;
    for (int i = 0; i < width - 2; i++) cout << (char)205;
    cout << (char)188 << "\n";
    resetColor();
}

void drawMidBorder(int width)
{
    setColor(CLR_BORDER);
    cout << "  " << (char)204;
    for (int i = 0; i < width - 2; i++) cout << (char)205;
    cout << (char)185 << "\n";
    resetColor();
}

void drawRow(string content, int width)
{
    setColor(CLR_BORDER);
    cout << "  " << (char)186 << " ";
    resetColor();
    cout << left << setw(width - 4) << content;
    setColor(CLR_BORDER);
    cout << (char)186 << "\n";
    resetColor();
}

// ============================================================
//  BANNER
// ============================================================

void showBanner()
{
    clearScreen();
    cout << "\n";
    drawTopBorder(62);

    setColor(CLR_BORDER);  cout << "  " << (char)186; resetColor();
    setColor(CLR_TITLE);   cout << "       DAILY EXPENSE TRACKING SYSTEM                   ";
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

    setColor(CLR_BORDER);  cout << "  " << (char)186; resetColor();
    setColor(CLR_DATA);    cout << "         Console GUI  |  Pure C++  |  Windows          ";
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

    // ── Logged-in user row ───────────────────────────────────
    if (!currentUser.empty())
    {
        setColor(CLR_BORDER);  cout << "  " << (char)186 << " "; resetColor();
        setColor(CLR_LABEL);   cout << "  Logged in as: "; resetColor();
        setColor(CLR_SUCCESS); cout << left << setw(44) << currentUser; resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
    }

    // ── Monthly budget row (only shown when a budget is set) ──
    double budget = loadBudget();
    if (budget > 0)
    {
        drawMidBorder(62);

        // Calculate current-month spending for the progress display
        SYSTEMTIME st;
        GetLocalTime(&st);
        char monthPrefix[8];
        sprintf(monthPrefix, "%04d-%02d", st.wYear, st.wMonth);

        // We read inline here (can't call loadExpenses yet — no circular dep)
        double monthTotal = 0.0;
        {
            ifstream f(FILE_ALL());
            string line;
            getline(f, line); // skip header
            while (getline(f, line))
            {
                if (line.empty()) continue;
                stringstream ss(line);
                string tok;
                vector<string> p;
                while (getline(ss, tok, ',')) p.push_back(tok);
                if ((int)p.size() >= 5 && p[1].size() >= 7 &&
                    p[1].substr(0, 7) == string(monthPrefix))
                {
                    try { monthTotal += stod(p[4]); } catch (...) {}
                }
            }
            f.close();
        }

        double pct       = (monthTotal / budget) * 100.0;
        double remaining = budget - monthTotal;
        int    barColor  = (pct >= 100.0) ? CLR_ERROR :
                           (pct >= 80.0)  ? CLR_WARNING : CLR_SUCCESS;

        // Line 1: Budget / Spent / Remaining
        setColor(CLR_BORDER);  cout << "  " << (char)186 << " "; resetColor();
        setColor(CLR_LABEL);   cout << "Budget: "; resetColor();
        setColor(CLR_DATA);
        char bline[70];
        sprintf(bline, "PKR %.0f", budget);
        cout << left << setw(14) << bline;
        setColor(CLR_LABEL);   cout << "Spent: "; resetColor();
        setColor(barColor);
        sprintf(bline, "PKR %.0f", monthTotal);
        cout << left << setw(14) << bline;
        setColor(CLR_LABEL);   cout << "Left: "; resetColor();
        setColor(remaining >= 0 ? CLR_SUCCESS : CLR_ERROR);
        sprintf(bline, "PKR %.0f", remaining >= 0 ? remaining : 0.0);
        cout << left << setw(10) << bline;
        resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

        // Line 2: mini progress bar
        setColor(CLR_BORDER);  cout << "  " << (char)186 << " "; resetColor();
        setColor(CLR_LABEL);   cout << "Progress: ["; resetColor();
        int filled = (int)((pct > 100 ? 100 : pct) / 5.0); // 20-block bar
        setColor(barColor);
        for (int i = 0; i < filled; i++)      cout << (char)219;
        resetColor();
        for (int i = filled; i < 20; i++)     cout << (char)176;
        setColor(barColor);
        char pline[20];
        sprintf(pline, "] %.1f%%", pct);
        cout << pline;
        resetColor();
        // pad to fill the box width (20 bar + label "Progress: [" = 11 + 20 + rest)
        // total inner width = 58; used: 1(space) + 11 + 20 + ~8 = ~40 => pad 18
        cout << left << setw(16) << " ";
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
    }

    drawBottomBorder(62);
    cout << "\n";
}

// ============================================================
//  DISPLAY HELPERS
// ============================================================

void printLine()
{
    setColor(CLR_BORDER);
    cout << "  +----------------------------------------------------------+\n";
    resetColor();
}

void printHeader(string title)
{
    cout << "\n";
    drawTopBorder(62);

    setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
    setColor(CLR_HEADER);  cout << left << setw(58) << title; resetColor();
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

    drawBottomBorder(62);
    cout << "\n";
}

void showMenu()
{
    showBanner();

    drawTopBorder(62);

    setColor(CLR_BORDER);  cout << "  " << (char)186; resetColor();
    setColor(CLR_HEADER);  cout << "                     MAIN MENU                        ";
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

    drawMidBorder(62);

    struct MenuItem { int key; string label; };
    MenuItem items[9] = {
        {1, "Add New Expense"},
        {2, "View All Expenses"},
        {3, "View Expenses by Category"},
        {4, "Search Expense by ID"},
        {5, "Delete an Expense"},
        {6, "Show Summary"},
        {7, "Export Report to File"},
        {8, "Set Monthly Budget"},
        {0, "Exit"}
    };

    for (int i = 0; i < 9; i++)
    {
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_MENU);    cout << "[" << items[i].key << "]  "; resetColor();
        setColor(CLR_RESET);   cout << left << setw(50) << items[i].label; resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
    }

    drawBottomBorder(62);
}

void printExpenses(vector<Expense> list)
{
    if (list.empty())
    {
        setColor(CLR_WARNING);
        cout << "  No expenses found.\n";
        resetColor();
        return;
    }

    // Table top
    setColor(CLR_BORDER);
    cout << "  " << (char)201;
    for (int i = 0; i < 98; i++) cout << (char)205;
    cout << (char)187 << "\n";

    // Column headers
    cout << "  " << (char)186 << " ";
    setColor(CLR_HEADER);
    cout << left
         << setw(4)  << "ID"
         << setw(12) << "Date"
         << setw(14) << "Category"
         << setw(18) << "Item"
         << setw(10) << "Amount"
         << setw(10) << "Payment"
         << setw(8)  << "Priority"
         << setw(16) << "Notes";
    setColor(CLR_BORDER);
    cout << (char)186 << "\n";

    // Mid divider
    cout << "  " << (char)204;
    for (int i = 0; i < 98; i++) cout << (char)205;
    cout << (char)185 << "\n";

    // Rows
    double total = 0;
    for (int i = 0; i < (int)list.size(); i++)
    {
        Expense e = list[i];
        cout << "  " << (char)186 << " ";

        if (i % 2 == 0) setColor(CLR_DATA);
        else             setColor(CLR_RESET);

        cout << left
             << setw(4)  << e.id
             << setw(12) << e.date
             << setw(14) << e.category
             << setw(18) << e.item.substr(0, 16)
             << setw(10) << fixed << setprecision(2) << e.amount
             << setw(10) << e.paymentMethod
             << setw(8)  << e.priority
             << setw(16) << e.notes.substr(0, 14);

        setColor(CLR_BORDER);
        cout << (char)186 << "\n";
        total += e.amount;
    }

    // Footer divider
    cout << "  " << (char)204;
    for (int i = 0; i < 98; i++) cout << (char)205;
    cout << (char)185 << "\n";

    // Total row
    cout << "  " << (char)186 << " ";
    setColor(CLR_SUCCESS);
    string totalStr = "TOTAL:  PKR " + to_string((long long)total) + ".00   (" +
                      to_string((int)list.size()) + " records)";
    cout << left << setw(97) << totalStr;
    setColor(CLR_BORDER);
    cout << (char)186 << "\n";

    // Bottom border
    cout << "  " << (char)200;
    for (int i = 0; i < 98; i++) cout << (char)205;
    cout << (char)188 << "\n";

    resetColor();
}

// ============================================================
//  CATEGORY HELPERS
// ============================================================

void showCategories()
{
    cout << "\n";
    drawTopBorder(34);

    setColor(CLR_BORDER);  cout << "  " << (char)186; resetColor();
    setColor(CLR_HEADER);  cout << "     SELECT CATEGORY          "; resetColor();
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

    drawMidBorder(34);

    string cats[6] = {"Food","Transport","Health","Education","Shopping","Other"};
    for (int i = 0; i < 6; i++)
    {
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_MENU);    cout << "[" << (i+1) << "]  "; resetColor();
        setColor(CLR_DATA);    cout << left << setw(22) << cats[i]; resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
    }

    drawBottomBorder(34);
    cout << "\n";
}

string getCategory(int choice)
{
    if (choice == 1) return "Food";
    if (choice == 2) return "Transport";
    if (choice == 3) return "Health";
    if (choice == 4) return "Education";
    if (choice == 5) return "Shopping";
    return "Other";
}

string getCategoryFile(string category)
{
    if (category == "Food")       return FILE_FOOD();
    if (category == "Transport")  return FILE_TRANSPORT();
    if (category == "Health")     return FILE_HEALTH();
    if (category == "Education")  return FILE_EDUCATION();
    if (category == "Shopping")   return FILE_SHOPPING();
    return FILE_OTHER();
}

// ============================================================
//  INPUT HELPERS
// ============================================================

int getIntInput(string prompt, int lo, int hi)
{
    int val;
    while (true)
    {
        setColor(CLR_LABEL);   cout << "  >> " << prompt; setColor(CLR_RESET);
        cin >> val;
        if (!cin.fail() && val >= lo && val <= hi)
        {
            cin.ignore();
            return val;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        setColor(CLR_ERROR);
        cout << "  [!] Invalid! Enter a number between " << lo << " and " << hi << ".\n";
        resetColor();
    }
}

double getDoubleInput(string prompt)
{
    double val;
    while (true)
    {
        setColor(CLR_LABEL);   cout << "  >> " << prompt; setColor(CLR_RESET);
        cin >> val;
        if (!cin.fail() && val > 0)
        {
            cin.ignore();
            return val;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        setColor(CLR_ERROR);
        cout << "  [!] Invalid! Enter a positive number.\n";
        resetColor();
    }
}

string getStringInput(string prompt)
{
    string val;
    setColor(CLR_LABEL);   cout << "  >> " << prompt; setColor(CLR_RESET);
    getline(cin, val);
    return val;
}

// ============================================================
//  FILE HELPERS
// ============================================================

void initFiles()
{
    string files[7] = {
        FILE_ALL(), FILE_FOOD(), FILE_TRANSPORT(),
        FILE_HEALTH(), FILE_EDUCATION(), FILE_SHOPPING(), FILE_OTHER()
    };
    string header = "ID,Date,Category,Item,Amount,Payment,Priority,Notes\n";
    for (int i = 0; i < 7; i++)
    {
        ifstream check(files[i]);
        if (!check.good())
        {
            ofstream out(files[i]);
            out << header;
            out.close();
        }
        check.close();
    }
}

string toCSV(Expense e)
{
    return to_string(e.id) + "," + e.date + "," + e.category + "," +
           e.item + "," + to_string(e.amount) + "," +
           e.paymentMethod + "," + e.priority + "," + e.notes;
}

Expense fromCSV(string line)
{
    Expense e; e.id = 0;
    stringstream ss(line);
    string token;
    vector<string> parts;
    while (getline(ss, token, ',')) parts.push_back(token);
    if ((int)parts.size() < 8) return e;
    e.id            = stoi(parts[0]);
    e.date          = parts[1];
    e.category      = parts[2];
    e.item          = parts[3];
    e.amount        = stod(parts[4]);
    e.paymentMethod = parts[5];
    e.priority      = parts[6];
    e.notes         = parts[7];
    return e;
}

void saveExpense(Expense e)
{
    ofstream master(FILE_ALL(), ios::app);
    master << toCSV(e) << "\n";
    master.close();

    ofstream catFile(getCategoryFile(e.category), ios::app);
    catFile << toCSV(e) << "\n";
    catFile.close();
}

vector<Expense> loadExpenses()
{
    vector<Expense> list;
    ifstream f(FILE_ALL());
    string line;
    getline(f, line);  // skip header
    while (getline(f, line))
    {
        if (line.empty()) continue;
        Expense e = fromCSV(line);
        if (e.id > 0) list.push_back(e);
    }
    f.close();
    return list;
}

vector<Expense> loadByCategory(string category)
{
    vector<Expense> list;
    ifstream f(getCategoryFile(category));
    string line;
    getline(f, line);  // skip header
    while (getline(f, line))
    {
        if (line.empty()) continue;
        Expense e = fromCSV(line);
        if (e.id > 0) list.push_back(e);
    }
    f.close();
    return list;
}

void rewriteFile(vector<Expense> list)
{
    string header = "ID,Date,Category,Item,Amount,Payment,Priority,Notes\n";
    ofstream master(FILE_ALL());
    master << header;
    for (int i = 0; i < (int)list.size(); i++)
        master << toCSV(list[i]) << "\n";
    master.close();

    string cats[6] = {"Food","Transport","Health","Education","Shopping","Other"};
    for (int i = 0; i < 6; i++)
    {
        ofstream cf(getCategoryFile(cats[i]));
        cf << header;
        for (int j = 0; j < (int)list.size(); j++)
            if (list[j].category == cats[i])
                cf << toCSV(list[j]) << "\n";
        cf.close();
    }
}

int getNextID()
{
    vector<Expense> list = loadExpenses();
    if (list.empty()) return 1;
    int maxID = 0;
    for (int i = 0; i < (int)list.size(); i++)
        if (list[i].id > maxID) maxID = list[i].id;
    return maxID + 1;
}

// ============================================================
//  BUDGET HELPERS
// ============================================================

double loadBudget()
{
    ifstream f(FILE_BUDGET());
    if (!f.good()) return 0.0;
    double budget = 0.0;
    f >> budget;
    f.close();
    return budget;
}

void saveBudget(double budget)
{
    ofstream f(FILE_BUDGET());
    f << fixed << setprecision(2) << budget;
    f.close();
}

// Prompts user to set/update the monthly budget
void setBudget()
{
    showBanner();
    printHeader("SET MONTHLY BUDGET");

    double current = loadBudget();
    if (current > 0)
    {
        setColor(CLR_DATA);
        cout << "  Current monthly budget: PKR " << fixed << setprecision(2) << current << "\n\n";
        resetColor();
    }

    double newBudget = getDoubleInput("Enter new monthly budget (PKR): ");
    saveBudget(newBudget);

    cout << "\n";
    drawTopBorder(50);
    setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
    setColor(CLR_SUCCESS);
    char msg[80];
    sprintf(msg, "Budget set to PKR %.2f successfully!", newBudget);
    cout << left << setw(46) << msg;
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
    drawBottomBorder(50);
}

/*
 * Calculates the current month's total spending (from all_expenses.txt),
 * adds newExpenseAmount, and warns the user if the budget is exceeded or
 * close (>= 80 %).  Reads the month from the current system date.
 */
void checkBudgetWarning(double newExpenseAmount)
{
    double budget = loadBudget();
    if (budget <= 0) return;  // no budget set — nothing to check

    // Get current year-month string, e.g. "2025-07"
    SYSTEMTIME st;
    GetLocalTime(&st);
    char monthPrefix[8];
    sprintf(monthPrefix, "%04d-%02d", st.wYear, st.wMonth);

    // Sum existing expenses for this month
    vector<Expense> list = loadExpenses();
    double monthTotal = 0.0;
    for (int i = 0; i < (int)list.size(); i++)
    {
        // Date field starts with "YYYY-MM"
        if (list[i].date.size() >= 7 &&
            list[i].date.substr(0, 7) == string(monthPrefix))
        {
            monthTotal += list[i].amount;
        }
    }
    monthTotal += newExpenseAmount;  // include the expense just saved

    double pct = (monthTotal / budget) * 100.0;

    if (pct >= 100.0)
    {
        // ── BUDGET EXCEEDED ──────────────────────────────────────
        cout << "\n";
        setColor(CLR_ERROR);
        cout << "  " << (char)201;
        for (int i = 0; i < 58; i++) cout << (char)205;
        cout << (char)187 << "\n";

        cout << "  " << (char)186 << "  ";
        cout << left << setw(56) << "  !! BUDGET EXCEEDED !!";
        cout << (char)186 << "\n";

        char line1[80], line2[80];
        sprintf(line1, "  Spent : PKR %.2f  /  Budget : PKR %.2f", monthTotal, budget);
        sprintf(line2, "  Over by PKR %.2f  (%.1f%% of budget used)", monthTotal - budget, pct);

        cout << "  " << (char)186 << "  " << left << setw(56) << line1 << (char)186 << "\n";
        cout << "  " << (char)186 << "  " << left << setw(56) << line2 << (char)186 << "\n";

        cout << "  " << (char)200;
        for (int i = 0; i < 58; i++) cout << (char)205;
        cout << (char)188 << "\n";
        resetColor();
    }
    else if (pct >= 80.0)
    {
        // ── APPROACHING BUDGET ───────────────────────────────────
        cout << "\n";
        setColor(CLR_WARNING);
        cout << "  " << (char)201;
        for (int i = 0; i < 58; i++) cout << (char)205;
        cout << (char)187 << "\n";

        cout << "  " << (char)186 << "  ";
        cout << left << setw(56) << "  ** BUDGET WARNING: Approaching Limit **";
        cout << (char)186 << "\n";

        char line1[80], line2[80];
        sprintf(line1, "  Spent : PKR %.2f  /  Budget : PKR %.2f", monthTotal, budget);
        sprintf(line2, "  Remaining : PKR %.2f  (%.1f%% used)", budget - monthTotal, pct);

        cout << "  " << (char)186 << "  " << left << setw(56) << line1 << (char)186 << "\n";
        cout << "  " << (char)186 << "  " << left << setw(56) << line2 << (char)186 << "\n";

        cout << "  " << (char)200;
        for (int i = 0; i < 58; i++) cout << (char)205;
        cout << (char)188 << "\n";
        resetColor();
    }
    // else: under 80 % — no warning needed
}

// ============================================================
//  SEARCH BY ID
// ============================================================

void searchByID()
{
    showBanner();
    printHeader("SEARCH EXPENSE BY ID");

    int id = getIntInput("Enter Expense ID to search: ", 1, 99999);

    vector<Expense> list = loadExpenses();
    bool found = false;

    for (int i = 0; i < (int)list.size(); i++)
    {
        if (list[i].id == id)
        {
            found = true;
            Expense e = list[i];
            cout << "\n";

            // ── Result card ──────────────────────────────────────
            drawTopBorder(56);

            setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
            setColor(CLR_HEADER);
            cout << left << setw(52) << "EXPENSE DETAILS";
            setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

            drawMidBorder(56);

            auto printField = [&](string label, string value)
            {
                setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
                setColor(CLR_LABEL);   cout << left << setw(18) << label; resetColor();
                setColor(CLR_DATA);    cout << left << setw(34) << value; resetColor();
                setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
            };

            char amtBuf[32];
            sprintf(amtBuf, "PKR %.2f", e.amount);

            printField("ID       :", to_string(e.id));
            printField("Date     :", e.date);
            printField("Category :", e.category);
            printField("Item     :", e.item);
            printField("Amount   :", string(amtBuf));
            printField("Payment  :", e.paymentMethod);
            printField("Priority :", e.priority);
            printField("Notes    :", e.notes);

            drawBottomBorder(56);
            break;
        }
    }

    if (!found)
    {
        cout << "\n";
        drawTopBorder(50);
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_ERROR);
        char msg[60];
        sprintf(msg, "No expense found with ID: %d", id);
        cout << left << setw(46) << msg;
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
        drawBottomBorder(50);
    }
}

// ============================================================
//  CORE FUNCTIONS
// ============================================================

void addExpense()
{
    showBanner();
    printHeader("ADD NEW EXPENSE");

    Expense e;
    e.id = getNextID();

    e.date = getStringInput("Date (YYYY-MM-DD)      : ");

    showCategories();
    int catChoice = getIntInput("Select Category (1-6)  : ", 1, 6);
    e.category = getCategory(catChoice);

    e.item   = getStringInput("Item / Product Name    : ");
    e.amount = getDoubleInput("Amount (PKR)           : ");

    // Payment method
    cout << "\n";
    drawTopBorder(34);
    setColor(CLR_BORDER); cout << "  " << (char)186; resetColor();
    setColor(CLR_HEADER); cout << "     PAYMENT METHOD           "; resetColor();
    setColor(CLR_BORDER); cout << (char)186 << "\n"; resetColor();
    drawMidBorder(34);
    string payLabels[3] = {"Cash", "Card", "Online"};
    for (int i = 0; i < 3; i++)
    {
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_MENU);    cout << "[" << (i+1) << "]  "; resetColor();
        setColor(CLR_DATA);    cout << left << setw(22) << payLabels[i]; resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
    }
    drawBottomBorder(34);
    int payChoice = getIntInput("Select (1-3)           : ", 1, 3);
    e.paymentMethod = payLabels[payChoice - 1];

    // Priority
    cout << "\n";
    drawTopBorder(34);
    setColor(CLR_BORDER); cout << "  " << (char)186; resetColor();
    setColor(CLR_HEADER); cout << "       PRIORITY LEVEL         "; resetColor();
    setColor(CLR_BORDER); cout << (char)186 << "\n"; resetColor();
    drawMidBorder(34);
    string priLabels[3] = {"Low", "Medium", "High"};
    for (int i = 0; i < 3; i++)
    {
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_MENU);    cout << "[" << (i+1) << "]  "; resetColor();
        setColor(CLR_DATA);    cout << left << setw(22) << priLabels[i]; resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
    }
    drawBottomBorder(34);
    int priChoice = getIntInput("Select (1-3)           : ", 1, 3);
    e.priority = priLabels[priChoice - 1];

    e.notes = getStringInput("Notes (Enter to skip)  : ");
    if (e.notes.empty()) e.notes = "-";

    saveExpense(e);

    // Success message
    cout << "\n";
    drawTopBorder(42);
    setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
    setColor(CLR_SUCCESS);
    string msg = "Expense saved successfully!  ID: " + to_string(e.id);
    cout << left << setw(38) << msg;
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
    drawBottomBorder(42);

    // Budget check AFTER saving
    checkBudgetWarning(e.amount);
}

void viewExpenses()
{
    showBanner();
    printHeader("ALL EXPENSES");
    vector<Expense> list = loadExpenses();
    printExpenses(list);
}

void viewByCategory()
{
    showBanner();
    printHeader("VIEW BY CATEGORY");
    showCategories();
    int choice = getIntInput("Select Category (1-6): ", 1, 6);
    string cat = getCategory(choice);
    printHeader(cat + " EXPENSES");
    vector<Expense> list = loadByCategory(cat);
    printExpenses(list);
}

void deleteExpense()
{
    showBanner();
    printHeader("DELETE EXPENSE");
    vector<Expense> list = loadExpenses();
    printExpenses(list);
    if (list.empty()) return;

    int id = getIntInput("Enter ID to delete: ", 1, 99999);

    bool found = false;
    vector<Expense> updated;
    for (int i = 0; i < (int)list.size(); i++)
    {
        if (list[i].id == id)
        {
            found = true;
            setColor(CLR_WARNING);
            cout << "\n  Removing: " << list[i].item
                 << "  |  PKR " << list[i].amount << "\n";
            resetColor();
        }
        else
        {
            updated.push_back(list[i]);
        }
    }

    if (!found)
    {
        setColor(CLR_ERROR);
        cout << "\n  [!] ID not found.\n";
        resetColor();
        return;
    }

    rewriteFile(updated);
    setColor(CLR_SUCCESS);
    cout << "  Deleted successfully.\n";
    resetColor();
}

void showSummary()
{
    showBanner();

    drawTopBorder(50);
    setColor(CLR_BORDER);  cout << "  " << (char)186; resetColor();
    setColor(CLR_HEADER);  cout << "           EXPENSE SUMMARY                    "; resetColor();
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
    drawMidBorder(50);

    vector<Expense> list = loadExpenses();

    if (list.empty())
    {
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_WARNING); cout << left << setw(46) << "No expenses found."; resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
        drawBottomBorder(50);
        return;
    }

    // Column header row
    setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
    setColor(CLR_HEADER);
    cout << left << setw(12) << "Category"
         << setw(6)  << "Count"
         << setw(10) << "Amount"
         << "Bar";
    resetColor();
    setColor(CLR_BORDER);  cout << "                   " << (char)186 << "\n"; resetColor();
    drawMidBorder(50);

    string cats[6] = {"Food","Transport","Health","Education","Shopping","Other"};
    int    colors[6] = {10, 11, 12, 13, 14, 9};
    double grandTotal   = 0;
    double catTotals[6] = {0};
    int    catCounts[6] = {0};

    for (int i = 0; i < 6; i++)
        for (int j = 0; j < (int)list.size(); j++)
            if (list[j].category == cats[i])
            {
                catTotals[i] += list[j].amount;
                catCounts[i]++;
                grandTotal   += list[j].amount;
            }

    double maxVal = 1;
    for (int i = 0; i < 6; i++)
        if (catTotals[i] > maxVal) maxVal = catTotals[i];

    for (int i = 0; i < 6; i++)
    {
        if (catCounts[i] == 0) continue;

        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(colors[i]);
        cout << left << setw(12) << cats[i]
             << setw(6)  << catCounts[i]
             << setw(10) << fixed << setprecision(0) << catTotals[i];

        int barLen = (int)(catTotals[i] / maxVal * 14);
        for (int b = 0; b < barLen; b++) cout << (char)219;
        cout << string(14 - barLen, ' ');

        resetColor();
        setColor(CLR_BORDER);  cout << "   " << (char)186 << "\n"; resetColor();
    }

    drawMidBorder(50);

    setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
    setColor(CLR_SUCCESS);
    string tot = "TOTAL   " + to_string((int)list.size()) + " records   PKR " +
                 to_string((long long)grandTotal);
    cout << left << setw(46) << tot;
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

    drawBottomBorder(50);

    // ── Monthly budget status block ───────────────────────────
    double budget = loadBudget();
    if (budget > 0)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char monthPrefix[8];
        sprintf(monthPrefix, "%04d-%02d", st.wYear, st.wMonth);

        double monthTotal = 0.0;
        int    monthCount = 0;
        for (int i = 0; i < (int)list.size(); i++)
        {
            if (list[i].date.size() >= 7 &&
                list[i].date.substr(0, 7) == string(monthPrefix))
            {
                monthTotal += list[i].amount;
                monthCount++;
            }
        }

        double pct       = (monthTotal / budget) * 100.0;
        double remaining = budget - monthTotal;

        cout << "\n";
        drawTopBorder(56);

        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_HEADER);  cout << left << setw(52) << "MONTHLY BUDGET STATUS"; resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

        drawMidBorder(56);

        // Budget / Spent / Remaining rows
        auto budRow = [&](string label, string value, int clr)
        {
            setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
            setColor(CLR_LABEL);   cout << left << setw(16) << label; resetColor();
            setColor(clr);         cout << left << setw(36) << value; resetColor();
            setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
        };

        char b1[50], b2[50], b3[50], b4[50];
        sprintf(b1, "PKR %.2f", budget);
        sprintf(b2, "PKR %.2f  (%d records)", monthTotal, monthCount);
        sprintf(b3, "PKR %.2f", remaining > 0 ? remaining : 0.0);
        sprintf(b4, "%.1f%%", pct);

        budRow("Budget  :",  b1, CLR_DATA);
        budRow("Spent   :",  b2, pct >= 100 ? CLR_ERROR : CLR_WARNING);
        budRow("Remaining:", b3, pct >= 100 ? CLR_ERROR : CLR_SUCCESS);
        budRow("Used     :", b4, pct >= 100 ? CLR_ERROR :
                                 pct >= 80  ? CLR_WARNING : CLR_SUCCESS);

        // Progress bar (20 blocks)
        int filled = (int)((pct > 100 ? 100 : pct) / 5.0);
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_LABEL);   cout << left << setw(16) << "Progress :"; resetColor();
        cout << "[";
        setColor(pct >= 100 ? CLR_ERROR : pct >= 80 ? CLR_WARNING : CLR_SUCCESS);
        for (int i = 0; i < filled; i++) cout << (char)219;
        resetColor();
        for (int i = filled; i < 20; i++) cout << (char)176;
        cout << "]";
        setColor(CLR_BORDER);
        cout << "              " << (char)186 << "\n";
        resetColor();

        drawBottomBorder(56);
    }
}

void exportReport()
{
    showBanner();
    printHeader("EXPORT REPORT");

    vector<Expense> list = loadExpenses();
    ofstream f(FILE_REPORT());
    if (!f.is_open())
    {
        setColor(CLR_ERROR);
        cout << "  [!] Could not create report file.\n";
        resetColor();
        return;
    }

    f << "================================\n";
    f << " DAILY EXPENSE TRACKER - REPORT\n";
    f << "================================\n\n";

    string cats[6] = {"Food","Transport","Health","Education","Shopping","Other"};
    double grandTotal = 0;

    f << left << setw(14) << "Category" << setw(8) << "Count" << "Total\n";
    f << string(34, '-') << "\n";
    for (int i = 0; i < 6; i++)
    {
        double total = 0; int count = 0;
        for (int j = 0; j < (int)list.size(); j++)
            if (list[j].category == cats[i]) { total += list[j].amount; count++; }
        if (count > 0)
        {
            f << setw(14) << cats[i] << setw(8) << count
              << fixed << setprecision(2) << total << "\n";
            grandTotal += total;
        }
    }
    f << string(34, '-') << "\n";
    f << setw(14) << "TOTAL" << setw(8) << list.size()
      << fixed << setprecision(2) << grandTotal << "\n\n";

    // Budget section in report
    double budget = loadBudget();
    if (budget > 0)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char monthPrefix[8];
        sprintf(monthPrefix, "%04d-%02d", st.wYear, st.wMonth);

        double monthTotal = 0.0;
        for (int i = 0; i < (int)list.size(); i++)
            if (list[i].date.size() >= 7 &&
                list[i].date.substr(0, 7) == string(monthPrefix))
                monthTotal += list[i].amount;

        f << "MONTHLY BUDGET STATUS\n" << string(34, '-') << "\n";
        f << left << setw(14) << "Budget"    << "PKR " << fixed << setprecision(2) << budget << "\n";
        f << left << setw(14) << "Spent"     << "PKR " << monthTotal << "\n";
        f << left << setw(14) << "Remaining" << "PKR " << (budget - monthTotal) << "\n";
        f << left << setw(14) << "Used"      << fixed << setprecision(1) << (monthTotal/budget*100) << "%\n\n";
    }

    f << "FULL EXPENSE LIST\n" << string(80, '-') << "\n";
    f << left << setw(5)  << "ID"   << setw(12) << "Date"
              << setw(14) << "Category" << setw(18) << "Item"
              << setw(10) << "Amount"   << setw(10) << "Payment"
              << setw(8)  << "Priority" << "Notes\n";
    f << string(80, '-') << "\n";
    for (int i = 0; i < (int)list.size(); i++)
    {
        Expense e = list[i];
        f << setw(5)  << e.id
          << setw(12) << e.date
          << setw(14) << e.category
          << setw(18) << e.item
          << setw(10) << fixed << setprecision(2) << e.amount
          << setw(10) << e.paymentMethod
          << setw(8)  << e.priority
          << e.notes << "\n";
    }
    f.close();

    setColor(CLR_SUCCESS);
    cout << "  Report saved to: " << FILE_REPORT() << "\n";
    resetColor();
}

// ============================================================
//  LOGIN / AUTH SYSTEM
// ============================================================

string currentUser = "";   // set after successful login

// Read password character-by-character, print '*' for each
string getPasswordInput(string prompt)
{
    setColor(CLR_LABEL);
    cout << "  >> " << prompt;
    setColor(CLR_RESET);

    string pass = "";
    char ch;
    while (true)
    {
        ch = _getch();        // Windows: reads without echo
        if (ch == '\r' || ch == '\n') break;   // Enter
        if (ch == '\b')                         // Backspace
        {
            if (!pass.empty())
            {
                pass.pop_back();
                cout << "\b \b";
            }
            continue;
        }
        pass += ch;
        cout << '*';
    }
    cout << "\n";
    return pass;
}

// Simple djb2-style hash → positive int
int simpleHash(string s)
{
    unsigned long h = 5381;
    for (int i = 0; i < (int)s.size(); i++)
        h = ((h << 5) + h) + (unsigned char)s[i];
    return (int)(h & 0x7FFFFFFF);   // keep positive
}

string hashToString(int h)
{
    char buf[16];
    sprintf(buf, "%d", h);
    return string(buf);
}

bool userExists(string username)
{
    ifstream f(FILE_USERS);
    if (!f.good()) return false;
    string line;
    while (getline(f, line))
    {
        if (line.empty()) continue;
        // format: username|hashedPassword
        size_t pos = line.find('|');
        if (pos == string::npos) continue;
        if (line.substr(0, pos) == username)
        {
            f.close();
            return true;
        }
    }
    f.close();
    return false;
}

bool registerUser(string username, string password)
{
    if (userExists(username)) return false;
    ofstream f(FILE_USERS, ios::app);
    if (!f.is_open()) return false;
    f << username << "|" << hashToString(simpleHash(password)) << "\n";
    f.close();
    return true;
}

bool validateLogin(string username, string password)
{
    ifstream f(FILE_USERS);
    if (!f.good()) return false;
    string stored = hashToString(simpleHash(password));
    string line;
    while (getline(f, line))
    {
        if (line.empty()) continue;
        size_t pos = line.find('|');
        if (pos == string::npos) continue;
        if (line.substr(0, pos) == username &&
            line.substr(pos + 1)  == stored)
        {
            f.close();
            return true;
        }
    }
    f.close();
    return false;
}

// ── Login banner (separate from main banner — no budget row yet) ──
void showLoginBanner()
{
    clearScreen();
    cout << "\n";
    drawTopBorder(62);

    setColor(CLR_BORDER);  cout << "  " << (char)186; resetColor();
    setColor(CLR_TITLE);   cout << "       DAILY EXPENSE TRACKING SYSTEM                   ";
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

    setColor(CLR_BORDER);  cout << "  " << (char)186; resetColor();
    setColor(CLR_DATA);    cout << "         Console GUI  |  Pure C++  |  Windows          ";
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

    drawBottomBorder(62);
    cout << "\n";
}

// Returns true on successful login
bool showLoginPage()
{
    showLoginBanner();
    printHeader("LOGIN TO YOUR ACCOUNT");

    string username = getStringInput("Username : ");
    string password = getPasswordInput("Password : ");

    if (validateLogin(username, password))
    {
        currentUser = username;

        cout << "\n";
        drawTopBorder(50);
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_SUCCESS);
        char msg[60];
        sprintf(msg, "Welcome back, %s!", username.c_str());
        cout << left << setw(46) << msg;
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
        drawBottomBorder(50);

        Sleep(1200);
        return true;
    }

    cout << "\n";
    drawTopBorder(50);
    setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
    setColor(CLR_ERROR);
    cout << left << setw(46) << "Invalid username or password!";
    setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
    drawBottomBorder(50);

    Sleep(1000);
    return false;
}

// Returns true when registration succeeds
bool showRegisterPage()
{
    showLoginBanner();
    printHeader("CREATE A NEW ACCOUNT");

    string username, password, confirm;

    // ── Username ────────────────────────────────────────────
    username = getStringInput("Choose Username : ");
    if (username.empty())
    {
        setColor(CLR_ERROR);
        cout << "  [!] Username cannot be empty.\n";
        resetColor();
        Sleep(1000);
        return false;
    }
    if (username.find('|') != string::npos)
    {
        setColor(CLR_ERROR);
        cout << "  [!] Username must not contain '|'.\n";
        resetColor();
        Sleep(1000);
        return false;
    }
    if (userExists(username))
    {
        cout << "\n";
        drawTopBorder(50);
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_ERROR);   cout << left << setw(46) << "Username already taken. Try another."; resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
        drawBottomBorder(50);
        Sleep(1200);
        return false;
    }

    // ── Password ────────────────────────────────────────────
    password = getPasswordInput("Choose Password : ");
    if (password.size() < 4)
    {
        setColor(CLR_ERROR);
        cout << "  [!] Password must be at least 4 characters.\n";
        resetColor();
        Sleep(1200);
        return false;
    }

    confirm = getPasswordInput("Confirm Password: ");
    if (password != confirm)
    {
        cout << "\n";
        drawTopBorder(50);
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_ERROR);   cout << left << setw(46) << "Passwords do not match!"; resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
        drawBottomBorder(50);
        Sleep(1200);
        return false;
    }

    if (registerUser(username, password))
    {
        cout << "\n";
        drawTopBorder(50);
        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_SUCCESS);
        char msg[60];
        sprintf(msg, "Account created! Welcome, %s!", username.c_str());
        cout << left << setw(46) << msg;
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

        setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
        setColor(CLR_DATA);    cout << left << setw(46) << "Please log in to continue."; resetColor();
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

        drawBottomBorder(50);
        Sleep(1500);
        return true;
    }

    setColor(CLR_ERROR);
    cout << "  [!] Registration failed. Try again.\n";
    resetColor();
    Sleep(1000);
    return false;
}

// ── Auth screen: shown on startup, loops until login succeeds ──
bool runAuthScreen()
{
    int attempts = 0;

    while (true)
    {
        showLoginBanner();

        // Auth menu
        drawTopBorder(62);

        setColor(CLR_BORDER);  cout << "  " << (char)186; resetColor();
        setColor(CLR_HEADER);  cout << "                  ACCOUNT ACCESS                      ";
        setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();

        drawMidBorder(62);

        struct { int key; string label; string desc; } items[3] = {
            {1, "Login",           "Sign in to your existing account"},
            {2, "Register",        "Create a new account"},
            {0, "Exit",            "Quit the application"}
        };

        for (int i = 0; i < 3; i++)
        {
            setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
            setColor(CLR_MENU);    cout << "[" << items[i].key << "]  "; resetColor();
            setColor(CLR_RESET);   cout << left << setw(14) << items[i].label; resetColor();
            setColor(CLR_DATA);    cout << left << setw(42) << items[i].desc; resetColor();
            setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
        }

        drawMidBorder(62);

        // Failed attempts warning
        if (attempts > 0)
        {
            setColor(CLR_BORDER);  cout << "  " << (char)186 << "  "; resetColor();
            setColor(CLR_ERROR);
            char warn[60];
            sprintf(warn, "Failed login attempt(s): %d", attempts);
            cout << left << setw(58) << warn;
            setColor(CLR_BORDER);  cout << (char)186 << "\n"; resetColor();
        }

        drawBottomBorder(62);

        int choice = getIntInput("Enter your choice: ", 0, 2);

        if (choice == 0)
        {
            showLoginBanner();
            setColor(CLR_SUCCESS);
            cout << "  Goodbye!\n\n";
            resetColor();
            return false;   // exit app
        }
        else if (choice == 1)
        {
            if (showLoginPage()) return true;
            attempts++;
        }
        else if (choice == 2)
        {
            showRegisterPage();
            // after register, loop back to login screen
        }
    }
}
