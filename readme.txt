------------------------------------------------------
SETUP AND INITIAL CONFIGURATION
------------------------------------------------------

1. COMPILATION: 
   *Download This Whole Folder
   *Check for username and password in the users.txt file
   *Double click scms.exe if prompted Go to moreinfo and run anyway 
   *Voila program is Running
   
  
2. INITIAL USER SETUP:
   * The program requires 'users.txt' for login.
   * ACTION: navigate to file named **users.txt** in the program directory and add your first credentials (separated by a space):
        Admin 1234
   * Rerun the program and log in. Use Option 12 to add new users securely.
======================================================
PROGRAM OVERVIEW: SUPPLY CHAIN MANAGEMENT SYSTEM (SCMS)
======================================================

The Supply Chain Management System (SCMS) is a robust, console-based application written in C. It manages inventory, tracks orders, and ensures secure, multi-user access with auditing capabilities. The project showcases strong skills in C file handling, modular design, and data management.

------------------------------------------------------
SYSTEM FEATURES AND DATA MANAGEMENT
------------------------------------------------------

1. SECURITY & ACCESS CONTROL:
* AUTHENTICATION: User accounts (Username/Password) are stored in the external 'users.txt' file.
* AUDITING: Every major transaction is recorded with a timestamp in a per-user log file (e.g., 'Username_log.txt').
* ACCESS CONTROL: Adding new users (Option 12) requires approval from an existing user/admin.

2. DATA PERSISTENCE:
* INVENTORY: Saved and loaded from 'inventory_data.dat'.
* ORDERS: Pending orders are saved and loaded from 'orders_data.dat'.

3. USABILITY & EFFICIENCY:
* BATCH ENTRY: Options 3 (Receive Stock) and 7 (Add New Order) run in loops, allowing continuous input until the user types 'E'.
* SKU REFERENCE: Options 3, 4, and 5 display a quick list of available SKUs and Names for easy identification.
* BATCH DISPATCH: Option 9 supports processing multiple orders at once.



------------------------------------------------------
MENU FUNCTIONALITY
------------------------------------------------------

| OPTION | ACTION | NOTE |
|--------|-------------------------------------|----------------------------------------------------|
| **1** | View Existing Supplies              | Displays the full current inventory list. |
| **2** | Add New Supply / Product            | Enters details for a new item. |
| **3** | Receive New Stock                   | Loops until 'E' is entered. Adds quantity to stock. |
| **4** | Update Product Details              | Modifies Name/Price of an item. Enter 'E' to exit. |
| **5** | Delete a Product                    | Removes an item permanently. Enter 'E' to exit. |
| **7** | Add New Order                       | Loops until 'E' is entered to quickly place orders. |
| **8** | Process Single Order                | Displays orders, then dispatches one by ID. |
| **9** | Process Multiple Orders             | Accepts space-separated Order IDs for batch dispatch. |
| **11** | Generate Reports                    | Creates a financial summary of inventory and orders. |
| **13** | **\*Save Data & Exit\*** | **MUST USE** to save all changes permanently. |
