============================================================
           DAILY EXPENSE TRACKING SYSTEM
              README & PROJECT GUIDE
============================================================

DESCRIPTION
-----------
A console-based C++ application for tracking daily expenses.
Users can add expenses by category, set a monthly budget,
view spending summaries, search/delete records, and export
reports to a text file. All data is stored in local .txt files.


FEATURES
--------
  [1]  Set Monthly Budget     -- Set a PKR budget; get warned at 80% and 100%
  [2]  Add New Expense        -- Log item, category, amount, payment, priority
  [3]  View All Expenses      -- Tabular view of every recorded expense
  [4]  View by Category       -- Filter expenses by one of 6 categories
  [5]  Search by ID           -- Look up a specific expense by its ID
  [6]  Delete an Expense      -- Remove a record by ID
  [7]  Show Summary           -- Bar chart breakdown + budget progress bar
  [8]  Export Report          -- Save a formatted report to summary_report.txt
  [0]  Exit


EXPENSE CATEGORIES
------------------
  Food | Transport | Health | Education | Shopping | Other


PAYMENT METHODS
---------------
  Cash | Card | Online


PRIORITY LEVELS
---------------
  Low | Medium | High


DATA FILES (auto-created on first run)
---------------------------------------
  all_expenses.txt    -- Master record of all expenses
  food.txt            -- Food category expenses
  transport.txt       -- Transport category expenses
  health.txt          -- Health category expenses
  education.txt       -- Education category expenses
  shopping.txt        -- Shopping category expenses
  other.txt           -- Other category expenses
  budget.txt          -- Stores the monthly budget value
  summary_report.txt  -- Generated when Export Report is used

  All files use CSV format:
  ID, Date, Category, Item, Amount, Payment, Priority, Notes


TEAM DIVISION
-------------
  Person 1 -- Core data & file system (hardest)
    struct Expense, file constants, initFiles(), toCSV(),
    fromCSV(), saveExpense(), loadExpenses(), loadByCategory(),
    rewriteFile(), getNextID(), deleteExpense()

  Person 2 -- Budget, input & add expense (medium)
    getIntInput(), getDoubleInput(), getStringInput(),
    loadBudget(), saveBudget(), setBudget(),
    checkBudgetWarning(), addExpense(), showSummary()

  Person 3 -- Display, search & UI (easiest)
    printLine(), showMenu(), showCategories(), getCategory(),
    getCategoryFile(), printExpenses(), viewExpenses(),
    viewByCategory(), searchByID(), exportReport(), main()

  Integration tip: Person 1 finishes file helpers first,
  then Person 2 and 3 build on top of them.


NOTES
-----
  - Dates must be entered in YYYY-MM-DD format.
  - Budget warning triggers at >= 80% of monthly spend.
  - All amounts are in Pakistani Rupees (PKR).
  - Data files must stay in the same folder as the executable.

============================================================
