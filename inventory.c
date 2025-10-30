#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> 

// --- System Constants ---
#define MAX_ITEMS 100
#define MAX_ORDERS 50 
#define MAX_NAME_LEN 50
#define MAX_SKU_LEN 10
#define DATA_FILE "inventory_data.dat" 
#define ORDER_FILE "orders_data.dat"    
#define USER_FILE "users.txt" 

// --- Credentials & Logs ---
#define ADMIN_ID "Admin" 
#define ADMIN_PASS "1234" 
#define MAX_LOGIN_ATTEMPTS 3
char loggedInUser[MAX_NAME_LEN] = ""; 

// --- Structures ---
typedef struct {
    char sku[MAX_SKU_LEN]; 
    char name[MAX_NAME_LEN];
    int quantity;
    double price;
} InventoryItem;

typedef struct {
    int orderID; 
    char sku[MAX_SKU_LEN];
    int quantity;
    char customerName[MAX_NAME_LEN]; 
} OrderItem;

// --- Global Arrays and Counters ---
InventoryItem inventory[MAX_ITEMS];
int itemCount = 0;
OrderItem pendingOrders[MAX_ORDERS];
int orderCount = 0;
int nextOrderID = 1001; 

// --- Function Prototypes ---
void displayMenu();
int checkCredentials(const char* username, const char* password);
int adminLogin();
void generateReports();
void logAction(const char* action);
void addUser(); 
void processMultipleOrders(); 
int findItemIndex(const char* sku);
int findOrderIndex(int orderID);

void loadInventory();
void saveInventory();
void loadOrders();
void saveOrders();

void addItem();
void updateDetails(); 
void deleteProduct(); 
void viewInventory(); 
void searchProduct(); 

void viewPendingOrders();
void addNewOrder();
void processOrder();
void recordIncomingStock(); 

// ---------------------------------------------------------------------
// --- AUTHENTICATION & LOGGING ---
// ---------------------------------------------------------------------

int checkCredentials(const char* username, const char* password) {
    FILE *file = fopen(USER_FILE, "r");
    
    if (file == NULL) {
        if (strcmp(username, ADMIN_ID) == 0 && strcmp(password, ADMIN_PASS) == 0) {
            return 1; 
        }
        return 0; 
    }

    char file_username[MAX_NAME_LEN];
    char file_password[MAX_NAME_LEN];
    int found = 0;

    while (fscanf(file, "%s %s", file_username, file_password) == 2) {
        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
            found = 1;
            break;
        }
    }
    fclose(file);
    return found;
}

void logAction(const char* action) {
    if (strlen(loggedInUser) == 0) return;

    char log_filename[MAX_NAME_LEN + 10];
    snprintf(log_filename, sizeof(log_filename), "%s_log.txt", loggedInUser);

    FILE *log_file = fopen(log_filename, "a");
    if (log_file == NULL) {
        return;
    }
    
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    
    time(&timer);
    tm_info = localtime(&timer);
    
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(log_file, "[%s] %s\n", buffer, action);
    
    fclose(log_file);
}

int adminLogin() {
    char input_id[MAX_NAME_LEN];
    char input_password[MAX_NAME_LEN];
    int attempts = 0;

    printf("\n============================================\n");
    printf("     Supply Chain Management System Login\n");
    printf("============================================\n");
    
    while (attempts < MAX_LOGIN_ATTEMPTS) {
        printf("Attempt %d of %d:\n", attempts + 1, MAX_LOGIN_ATTEMPTS);
        
        printf("  Enter Username: ");
        if (scanf("%s", input_id) != 1) {
             while(getchar() != '\n');
             attempts++;
             continue;
        }
        
        printf("  Enter Password: ");
        if (scanf("%s", input_password) != 1) {
             while(getchar() != '\n');
             attempts++;
             continue;
        }
        
        while(getchar() != '\n'); 

        if (checkCredentials(input_id, input_password)) {
            printf("\nLogin successful! Welcome, %s.\n", input_id);
            strcpy(loggedInUser, input_id);
            logAction("USER_LOGIN: Successful login.");
            return 1;
        } else {
            printf("\n*** Invalid Username or Password. Please try again. ***\n");
            attempts++;
        }
    }
    
    printf("\n*** Maximum login attempts reached. Program exiting. ***\n");
    return 0;
}

void addUser() {
    char admin_username[MAX_NAME_LEN];
    char admin_password[MAX_NAME_LEN];
    char new_username[MAX_NAME_LEN];
    char new_password[MAX_NAME_LEN];
    
    printf("\n--- Add New User (Admin Approval Required) ---\n");
    
    printf("--- ADMIN APPROVAL ---\n");
    printf("Enter CURRENT Admin/User Username: ");
    if (scanf("%s", admin_username) != 1) return;
    printf("Enter CURRENT Admin/User Password: ");
    if (scanf("%s", admin_password) != 1) return;
    
    if (!checkCredentials(admin_username, admin_password)) {
        printf("\n*** ACCESS DENIED: Invalid Admin credentials. User not added. ***\n");
        logAction("USER_MANAGEMENT: Failed attempt to add user (Bad Admin Auth).");
        return;
    }
    printf("--- Approval Granted ---\n");
    
    printf("Enter new username: ");
    if (scanf("%s", new_username) != 1) return;
    
    if (checkCredentials(new_username, "dummy_check")) { 
        printf("*** Error: User '%s' already exists. Cannot add. ***\n", new_username);
        logAction("USER_MANAGEMENT: Failed to add user (Username exists).");
        return;
    }
    
    printf("Enter password for %s: ", new_username);
    if (scanf("%s", new_password) != 1) return;
    
    while(getchar() != '\n'); 

    FILE *file = fopen(USER_FILE, "a");
    if (file == NULL) {
        printf("*** Error: Could not open user file for writing! ***\n");
        return;
    }
    
    fprintf(file, "%s %s\n", new_username, new_password);
    fclose(file);
    
    printf("\nSuccessfully added user: %s.\n", new_username);
    char log_msg[100];
    snprintf(log_msg, 100, "USER_MANAGEMENT: Approved and added new user %s.", new_username);
    logAction(log_msg);
}

// ---------------------------------------------------------------------
// --- Main Function ---
// ---------------------------------------------------------------------
int main() {
    int choice;
    
    if (!adminLogin()) {
        return 0;
    }

    loadInventory(); 
    loadOrders(); 

    do {
        displayMenu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("\n\n*** Invalid input. Please enter a number. ***\n\n");
            choice = 0;
        }

        while(getchar() != '\n'); 

        switch (choice) {
            case 1: viewInventory(); break;
            case 2: addItem(); break;
            case 3: recordIncomingStock(); break; // Looping function
            case 4: updateDetails(); break;
            case 5: deleteProduct(); break;
            case 6: viewPendingOrders(); break;
            case 7: addNewOrder(); break; // Looping function
            case 8: processOrder(); break;
            case 9: processMultipleOrders(); break; 
            case 10: searchProduct(); break;
            case 11: generateReports(); break;
            case 12: addUser(); break; 
            case 13: 
                saveInventory(); 
                saveOrders(); 
                printf("\nData saved successfully! Exiting Inventory Tracker.\n");
                logAction("USER_LOGOUT: Data saved and system exited.");
                break;
            default:
                printf("\n\n*** Invalid choice. Please try again. ***\n\n");
        }
    } while (choice != 13); 

    return 0;
}

// ---------------------------------------------------------------------
// --- UTILITY & SEARCH FUNCTIONS ---
// ---------------------------------------------------------------------
int findItemIndex(const char* sku) {
    for (int i = 0; i < itemCount; i++) {
        if (strcmp(inventory[i].sku, sku) == 0) {
            return i;
        }
    }
    return -1;
}

int findOrderIndex(int orderID) {
    for (int i = 0; i < orderCount; i++) {
        if (pendingOrders[i].orderID == orderID) {
            return i;
        }
    }
    return -1;
}

void searchProduct() {
    char sku_to_find[MAX_SKU_LEN];
    
    printf("\n--- Search Product by SKU ---\n");
    
    // Quick Reference Display
    if (itemCount > 0) {
        printf("\n--- Available SKUs ---\n");
        printf("| %-8s | %-30s |\n", "SKU", "NAME");
        printf("----------------------------------------\n");
        for (int i = 0; i < itemCount; i++) {
            printf("| %-8s | %-30s |\n", inventory[i].sku, inventory[i].name);
        }
        printf("----------------------------------------\n");
    } else {
        printf("\n(Inventory is empty. Use Option 2 to add items.)\n");
    }
    
    printf("Enter SKU to search: ");
    if (scanf("%s", sku_to_find) != 1) return;
    
    int index = findItemIndex(sku_to_find);
    
    if (index == -1) {
        printf("\n*** Error: Product with SKU '%s' not found. ***\n", sku_to_find);
        logAction("SEARCH: Failed search for SKU.");
        return;
    }

    // Display details for confirmation/reference
    printf("\n-- Found Product Details (SKU: %s) --\n", inventory[index].sku);
    printf("Name: %s\n", inventory[index].name);
    printf("Current Quantity: %d\n", inventory[index].quantity);
    printf("Unit Price: Rs.%.2lf\n", inventory[index].price);
    printf("------------------------------------\n");
    
    logAction("SEARCH: Successful search for SKU.");
}

// ---------------------------------------------------------------------
// --- DATA PERSISTENCE FUNCTIONS ---
// ---------------------------------------------------------------------
void loadInventory() {
    FILE *file = fopen(DATA_FILE, "rb");
    if (file == NULL) {
        printf("No existing inventory data found. Starting fresh.\n");
        return;
    }

    if (fread(&itemCount, sizeof(int), 1, file) != 1) {
        printf("Warning: Error reading item count from inventory file.\n");
        fclose(file);
        return;
    }
    
    if (itemCount > MAX_ITEMS) {
        itemCount = MAX_ITEMS;
    }

    size_t items_read = fread(inventory, sizeof(InventoryItem), itemCount, file);
    if (items_read != (size_t)itemCount) {
        itemCount = (int)items_read; 
    }
    printf("Successfully loaded %d items from inventory file.\n", itemCount);

    fclose(file);
}

void saveInventory() {
    FILE *file = fopen(DATA_FILE, "wb");
    if (file == NULL) {
        printf("*** Error: Could not open inventory file for saving! ***\n");
        return;
    }

    fwrite(&itemCount, sizeof(int), 1, file);
    fwrite(inventory, sizeof(InventoryItem), itemCount, file);

    fclose(file);
}

void loadOrders() {
    FILE *file = fopen(ORDER_FILE, "rb");
    if (file == NULL) {
        printf("No existing order data found. Starting fresh.\n");
        return;
    }

    if (fread(&orderCount, sizeof(int), 1, file) != 1) {
         printf("Warning: Error reading order count from file.\n");
         fclose(file);
         return;
    }
    if (fread(&nextOrderID, sizeof(int), 1, file) != 1) {
         printf("Warning: Error reading next Order ID from file.\n");
         fclose(file);
         return;
    }
    
    if (orderCount > MAX_ORDERS) {
         orderCount = MAX_ORDERS;
    }

    size_t orders_read = fread(pendingOrders, sizeof(OrderItem), orderCount, file);
    
    if (orders_read != (size_t)orderCount) {
        orderCount = (int)orders_read; 
    }
    printf("Successfully loaded %d pending orders.\n", orderCount);

    fclose(file);
}

void saveOrders() {
    FILE *file = fopen(ORDER_FILE, "wb");
    if (file == NULL) {
        printf("*** Error: Could not open orders file for saving! ***\n");
        return;
    }

    fwrite(&orderCount, sizeof(int), 1, file);
    fwrite(&nextOrderID, sizeof(int), 1, file);
    fwrite(pendingOrders, sizeof(OrderItem), orderCount, file);

    fclose(file);
}

// ---------------------------------------------------------------------
// --- MENU AND CRUD FUNCTIONS ---
// ---------------------------------------------------------------------
void displayMenu() {
    printf("\n--- Supply Chain Management System ---\n");
    printf("Current User: %s\n", loggedInUser);
    printf("1. View Existing Supplies\n");
    printf("2. Add New Supply / Product\n");
    printf("3. Receive New Stock (Record Incoming Supply)\n"); 
    printf("4. Update Product Details (Name/Price)\n");       
    printf("5. Delete a Product\n");                          
    printf("6. View Pending Orders\n");                       
    printf("7. Add New Order\n");                             
    printf("8. Process Single Order (Dispatch)\n");           
    printf("9. Process Multiple Orders (Dispatch Batch)\n");  
    printf("10. Search Product by ID (SKU)\n");               
    printf("11. Generate Reports\n");                         
    printf("12. Add New User\n");                             
    printf("13. *Save Data & Exit*\n");                       
    printf("--------------------------------------------\n");
}

void viewInventory() {
    if (itemCount == 0) {
        printf("\n*** The inventory is currently empty. ***\n");
        logAction("INVENTORY: Viewed empty inventory.");
        return;
    }

    printf("\n=========================================================================\n");
    printf("| %-8s | %-25s | %-8s | %-6s | %-10s |\n", "SKU", "ITEM NAME", "QUANTITY", "PRICE", "TOTAL VALUE");
    printf("=========================================================================\n");

    for (int i = 0; i < itemCount; i++) {
        double total_value = inventory[i].quantity * inventory[i].price;
        printf("| %-8s | %-25s | %-8d | Rs.%-5.2lf | Rs.%-9.2lf |\n", 
               inventory[i].sku, 
               inventory[i].name, 
               inventory[i].quantity, 
               inventory[i].price,
               total_value);
    }
    printf("=========================================================================\n");
    printf("Total distinct items: %d\n", itemCount);
    logAction("INVENTORY: Viewed inventory list.");
}

void addItem() {
    if (itemCount >= MAX_ITEMS) {
        printf("\n*** Inventory is full. Cannot add more items. ***\n");
        logAction("ITEM_ADD: Failed to add item (Inventory full).");
        return;
    }

    InventoryItem newItem;
    char temp_sku[MAX_SKU_LEN];
    
    printf("\n--- Add New Item ---\n");
    printf("Enter Item SKU (max %d chars): ", MAX_SKU_LEN - 1);
    if (scanf("%s", temp_sku) != 1) return;
    
    if (findItemIndex(temp_sku) != -1) {
        printf("*** Error: Item with SKU '%s' already exists. ***\n", temp_sku);
        logAction("ITEM_ADD: Failed to add item (SKU exists).");
        return;
    }
    strcpy(newItem.sku, temp_sku);

    printf("Enter Item Name (max %d chars): ");
    while(getchar() != '\n'); 
    if (fgets(newItem.name, MAX_NAME_LEN, stdin) != NULL) {
        newItem.name[strcspn(newItem.name, "\n")] = 0;
    }

    printf("Enter Initial Quantity: ");
    if (scanf("%d", &newItem.quantity) != 1 || newItem.quantity < 0) {
        printf("*** Error: Invalid quantity entered. ***\n");
        return;
    }

    printf("Enter Unit Price: Rs.");
    if (scanf("%lf", &newItem.price) != 1 || newItem.price < 0) {
        printf("*** Error: Invalid price entered. ***\n");
        return;
    }

    inventory[itemCount] = newItem;
    itemCount++;
    printf("\nSuccessfully added item: %s (SKU: %s) with %d units.\n", newItem.name, newItem.sku, newItem.quantity);
    char log_msg[100];
    snprintf(log_msg, 100, "ITEM_ADD: Added %s (SKU %s).", newItem.name, newItem.sku);
    logAction(log_msg);
}

void recordIncomingStock() {
    char sku_to_find[MAX_SKU_LEN];
    int quantity_change;
    
    printf("\n--- Record Incoming Stock (Supply) ---\n");
    
    do {
        // Quick Reference Display
        if (itemCount > 0) {
            printf("\n--- Available SKUs ---\n");
            printf("| %-8s | %-30s |\n", "SKU", "NAME");
            printf("----------------------------------------\n");
            for (int i = 0; i < itemCount; i++) {
                printf("| %-8s | %-30s |\n", inventory[i].sku, inventory[i].name);
            }
            printf("----------------------------------------\n");
        } else {
            printf("\n(Inventory is empty. Use Option 2 to add items.)\n");
            break; // Exit loop if inventory is empty
        }

        printf("Enter SKU of the item (or type 'E' to Exit): ");
        if (scanf("%s", sku_to_find) != 1) continue;
        
        // Exit condition check
        if (strcmp(sku_to_find, "E") == 0 || strcmp(sku_to_find, "e") == 0) {
            break;
        }
        
        int index = findItemIndex(sku_to_find);
        
        if (index == -1) {
            printf("\n*** Error: Product with SKU '%s' not found. ***\n", sku_to_find);
            while(getchar() != '\n');
            continue;
        }
        
        // Display Found Details
        printf("\n-- Product Found --\n");
        printf("Name: %s\n", inventory[index].name);
        printf("Current Quantity: %d\n", inventory[index].quantity);
        printf("Unit Price: Rs.%.2lf\n", inventory[index].price);
        printf("-------------------\n");

        printf("Enter quantity to RECEIVE: ");
        
        if (scanf("%d", &quantity_change) != 1 || quantity_change <= 0) {
            printf("*** Error: Invalid quantity entered. ***\n");
            while(getchar() != '\n');
            continue;
        }
        
        inventory[index].quantity += quantity_change;
        printf("\n✅ Recorded %d units of %s incoming. New stock: %d.\n", quantity_change, inventory[index].name, inventory[index].quantity);
        char log_msg[100];
        snprintf(log_msg, 100, "STOCK_IN: Received %d units of %s. New qty: %d.", quantity_change, inventory[index].sku, inventory[index].quantity);
        logAction(log_msg);

    } while (1);

    printf("\n--- Exited Receive New Stock Section. ---\n");
}

void updateDetails() {
    char sku_to_update[MAX_SKU_LEN];
    char temp_name[MAX_NAME_LEN];
    double new_price;
    char log_msg[150];
    
    printf("\n--- Update Product Details ---\n");
    
    // Quick Reference Display
    if (itemCount > 0) {
        printf("\n--- Available SKUs ---\n");
        printf("| %-8s | %-30s |\n", "SKU", "NAME");
        printf("----------------------------------------\n");
        for (int i = 0; i < itemCount; i++) {
            printf("| %-8s | %-30s |\n", inventory[i].sku, inventory[i].name);
        }
        printf("----------------------------------------\n");
    } else {
        printf("\n(Inventory is empty. Use Option 2 to add items.)\n");
    }
    
    printf("Enter SKU of the item to update (or type 'E' to Exit): ");
    if (scanf("%s", sku_to_update) != 1) return;
    
    // Exit condition check
    if (strcmp(sku_to_update, "E") == 0 || strcmp(sku_to_update, "e") == 0) {
        printf("\n--- Exited Update Product Details Section. ---\n");
        return;
    }
    
    int index = findItemIndex(sku_to_update);
    
    if (index == -1) {
        printf("\n*** Error: Item with SKU '%s' not found. ***\n", sku_to_update);
        logAction("DETAILS_UPDATE: Failed to find SKU.");
        return;
    }

    // Display Current Details
    printf("\n-- Current Details (SKU: %s) --\n", inventory[index].sku);
    printf("Name: %s\n", inventory[index].name);
    printf("Current Price: Rs.%.2lf\n", inventory[index].price);
    printf("-------------------------------\n");

    printf("Enter NEW Item Name (Leave blank to keep current: %s): ", inventory[index].name);
    while(getchar() != '\n'); 
    if (fgets(temp_name, MAX_NAME_LEN, stdin) != NULL) {
        temp_name[strcspn(temp_name, "\n")] = 0;
        if (strlen(temp_name) > 0) {
            strcpy(inventory[index].name, temp_name);
            printf("  > Name updated to: %s\n", inventory[index].name);
        }
    }
    
    printf("Enter NEW Unit Price (Enter -1 to keep current: Rs.%.2lf): Rs.", inventory[index].price);
    if (scanf("%lf", &new_price) == 1) {
        if (new_price >= 0) {
            inventory[index].price = new_price;
            printf("  > Price updated to: Rs.%.2lf\n", inventory[index].price);
        } else if (new_price != -1) {
            printf("*** Invalid price entered. Price remains unchanged. ***\n");
        }
    } else {
        while(getchar() != '\n'); 
        printf("*** Invalid input for price. Price remains unchanged. ***\n");
    }
    
    printf("\nDetails for SKU %s updated.\n", sku_to_update);
    snprintf(log_msg, 150, "DETAILS_UPDATE: Updated name/price for SKU %s.", sku_to_update);
    logAction(log_msg);
}

void deleteProduct() {
    char sku_to_delete[MAX_SKU_LEN];
    
    printf("\n--- Delete a Product ---\n");
    
    // Quick Reference Display
    if (itemCount > 0) {
        printf("\n--- Available SKUs ---\n");
        printf("| %-8s | %-30s |\n", "SKU", "NAME");
        printf("----------------------------------------\n");
        for (int i = 0; i < itemCount; i++) {
            printf("| %-8s | %-30s |\n", inventory[i].sku, inventory[i].name);
        }
        printf("----------------------------------------\n");
    } else {
        printf("\n(Inventory is empty. Use Option 2 to add items.)\n");
    }
    
    printf("Enter SKU of the item to DELETE (or type 'E' to Exit): ");
    if (scanf("%s", sku_to_delete) != 1) return;
    
    // Exit condition check
    if (strcmp(sku_to_delete, "E") == 0 || strcmp(sku_to_delete, "e") == 0) {
        printf("\n--- Exited Delete Product Section. ---\n");
        return;
    }
    
    int index = findItemIndex(sku_to_delete);
    
    if (index == -1) {
        printf("\n*** Error: Item with SKU '%s' not found. ***\n", sku_to_delete);
        logAction("ITEM_DELETE: Failed to find SKU for deletion.");
        return;
    }

    // Display Product Details
    printf("\n-- Product to DELETE (SKU: %s) --\n", inventory[index].sku);
    printf("Name: %s\n", inventory[index].name);
    printf("Current Quantity: %d\n", inventory[index].quantity);
    printf("Unit Price: Rs.%.2lf\n", inventory[index].price);
    printf("--------------------------------\n");
    
    printf("WARNING: You are about to delete %s (SKU: %s).\n", inventory[index].name, inventory[index].sku);
    printf("Confirm deletion (y/n): ");
    char confirm;
    if (scanf(" %c", &confirm) != 1 || (confirm != 'y' && confirm != 'Y')) {
        printf("\n*** Deletion cancelled. ***\n");
        return;
    }

    for (int i = index; i < itemCount - 1; i++) {
        inventory[i] = inventory[i+1];
    }
    
    itemCount--;
    printf("\nSuccessfully deleted item with SKU: %s.\n", sku_to_delete);
    char log_msg[100];
    snprintf(log_msg, 100, "ITEM_DELETE: Deleted SKU %s.", sku_to_delete);
    logAction(log_msg);
}

// ---------------------------------------------------------------------
// --- ORDER MANAGEMENT FUNCTIONS ---
// ---------------------------------------------------------------------
void viewPendingOrders() {
    if (orderCount == 0) {
        printf("\n*** There are currently no pending orders. ***\n");
        logAction("ORDERS: Viewed empty pending orders list.");
        return;
    }

    printf("\n============================================================\n");
    printf("| %-8s | %-8s | %-8s | %-20s |\n", "ORDER ID", "SKU", "QTY", "CUSTOMER");
    printf("============================================================\n");

    for (int i = 0; i < orderCount; i++) {
        printf("| %-8d | %-8s | %-8d | %-20s |\n", 
               pendingOrders[i].orderID, 
               pendingOrders[i].sku, 
               pendingOrders[i].quantity, 
               pendingOrders[i].customerName);
    }
    printf("============================================================\n");
    printf("Total pending orders: %d\n", orderCount);
    logAction("ORDERS: Viewed pending orders list.");
}

void addNewOrder() {
    char sku_to_check[MAX_SKU_LEN];
    
    printf("\n--- Add New Order ---\n");
    
    do {
        if (itemCount == 0) {
            printf("*** Cannot add orders: Inventory is empty. Add products first (Option 2). ***\n");
            logAction("ORDER_ADD: Failed to add order (Inventory empty).");
            break;
        }
        
        if (orderCount >= MAX_ORDERS) {
            printf("\n*** Order list is full. Cannot add more orders. ***\n");
            logAction("ORDER_ADD: Failed to add order (list full).");
            break;
        }

        OrderItem newOrder;
        int inv_index;
        int temp_qty;

        // Display available SKUs for reference
        if (itemCount > 0) {
            printf("\n--- Available SKUs ---\n");
            printf("| %-8s | %-30s |\n", "SKU", "NAME");
            printf("----------------------------------------\n");
            for (int i = 0; i < itemCount; i++) {
                printf("| %-8s | %-30s |\n", inventory[i].sku, inventory[i].name);
            }
            printf("----------------------------------------\n");
        }

        printf("Enter Product SKU (or type 'E' to Exit Order Entry): ");
        if (scanf("%s", sku_to_check) != 1) continue;
        
        // Exit condition check
        if (strcmp(sku_to_check, "E") == 0 || strcmp(sku_to_check, "e") == 0) {
            break;
        }

        inv_index = findItemIndex(sku_to_check);

        if (inv_index == -1) {
            printf("*** Error: Product with SKU '%s' not found in inventory. ***\n", sku_to_check);
            while(getchar() != '\n'); 
            continue;
        }
        strcpy(newOrder.sku, sku_to_check);

        printf("Enter Order Quantity: ");
        if (scanf("%d", &temp_qty) != 1 || temp_qty <= 0) {
            printf("*** Error: Invalid quantity entered. ***\n");
            while(getchar() != '\n');
            continue;
        }
        newOrder.quantity = temp_qty;

        printf("Enter Customer Name (max %d chars): ");
        while(getchar() != '\n'); 
        if (fgets(newOrder.customerName, MAX_NAME_LEN, stdin) != NULL) {
            newOrder.customerName[strcspn(newOrder.customerName, "\n")] = 0;
        }

        newOrder.orderID = nextOrderID++;
        pendingOrders[orderCount] = newOrder;
        orderCount++;
        
        printf("\n✅ Order %d placed successfully for %d units of %s.\n", 
               newOrder.orderID, newOrder.quantity, inventory[inv_index].name);
        
        char log_msg[150];
        snprintf(log_msg, 150, "ORDER_ADD: Placed order %d for %d units of SKU %s.", newOrder.orderID, newOrder.quantity, newOrder.sku);
        logAction(log_msg);

    } while (1);
    
    printf("\n--- Exited Add New Order Section. ---\n");
}

// Option 8 (Process Single Order)
void processOrder() {
    int order_id_to_process;
    int order_index;

    printf("\n--- Process Single Order (Dispatch) ---\n");
    
    viewPendingOrders(); 
    
    if (orderCount == 0) {
        return; 
    }
    
    printf("\n>>> Please enter the ORDER ID to dispatch: ");
    if (scanf("%d", &order_id_to_process) != 1) return;
    
    while(getchar() != '\n'); 
    
    order_index = findOrderIndex(order_id_to_process);
    
    if (order_index == -1) {
        printf("\n*** Error: Order with ID %d not found in the pending list. ***\n", order_id_to_process);
        logAction("ORDER_PROCESS: Invalid Order ID entered.");
        return;
    }

    OrderItem order = pendingOrders[order_index];
    int inv_index = findItemIndex(order.sku);
    
    if (inv_index == -1) {
         printf("\n*** Critical Error: Product SKU %s not found in inventory. Cannot process order. ***\n", order.sku);
         logAction("ORDER_PROCESS: Failed due to missing inventory SKU.");
         return;
    }
    
    if (inventory[inv_index].quantity < order.quantity) {
        printf("\n*** Error: Insufficient stock for Order %d. Requires %d, but only %d in stock. ***\n", 
               order.orderID, order.quantity, inventory[inv_index].quantity);
        logAction("ORDER_PROCESS: Failed due to insufficient stock.");
        return;
    }

    inventory[inv_index].quantity -= order.quantity;

    for (int i = order_index; i < orderCount - 1; i++) {
        pendingOrders[i] = pendingOrders[i+1];
    }
    orderCount--;

    printf("\nOrder %d (SKU: %s) successfully DISPATCHED to %s.\n", 
           order.orderID, order.sku, order.customerName);
    printf("  > %d units removed from stock. New stock of %s: %d.\n", 
           order.quantity, inventory[inv_index].name, inventory[inv_index].quantity);
    
    char log_msg[150];
    snprintf(log_msg, 150, "ORDER_PROCESS: Dispatched Order %d for SKU %s. Stock updated.", order.orderID, order.sku);
    logAction(log_msg);
}

// Option 9 (Process Multiple Orders)
void processMultipleOrders() {
    char input_buffer[256];
    char* token;
    int orders_dispatched = 0;
    
    printf("\n--- Process Multiple Orders (Batch Dispatch) ---\n");
    viewPendingOrders(); 

    printf("\n>>> Enter the ORDER IDs to dispatch, separated by spaces (e.g., 1001 1005 1010):\n>>> ");
    
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) return;
    
    input_buffer[strcspn(input_buffer, "\n")] = 0;
    
    token = strtok(input_buffer, " ");
    
    printf("\n--- Starting Batch Dispatch ---\n");
    
    while (token != NULL) {
        int order_id;
        if (sscanf(token, "%d", &order_id) == 1) {
            int order_index = findOrderIndex(order_id);
            
            if (order_index == -1) {
                printf("  [SKIP] Order ID %d not found in pending list.\n", order_id);
            } else {
                OrderItem order = pendingOrders[order_index];
                int inv_index = findItemIndex(order.sku);
                
                if (inv_index == -1 || inventory[inv_index].quantity < order.quantity) {
                    printf("  [FAIL] Order ID %d (%s) - Insufficient Stock or Item Missing (Qty: %d).\n", 
                           order_id, order.sku, inv_index != -1 ? inventory[inv_index].quantity : 0);
                    logAction("ORDER_MULTI_PROCESS: Failed order due to stock/missing item.");
                } else {
                    inventory[inv_index].quantity -= order.quantity;

                    for (int i = order_index; i < orderCount - 1; i++) {
                        pendingOrders[i] = pendingOrders[i+1];
                    }
                    orderCount--;
                    
                    printf("  [OK] Dispatched Order %d (%s). New stock of %s: %d.\n", 
                           order_id, order.sku, inventory[inv_index].name, inventory[inv_index].quantity);
                    orders_dispatched++;
                    
                    char log_msg[150];
                    snprintf(log_msg, 150, "ORDER_MULTI_PROCESS: Dispatched Order %d for SKU %s.", order.orderID, order.sku);
                    logAction(log_msg);
                }
            }
        }
        token = strtok(NULL, " ");
    }
    
    printf("\n--- Batch Dispatch Complete ---\n");
    printf("Total orders processed successfully in this batch: %d\n", orders_dispatched);
    
    if (orders_dispatched > 0) {
        logAction("ORDER_MULTI_PROCESS: Completed batch dispatch.");
    }
}

// ---------------------------------------------------------------------
// --- REPORT GENERATION FUNCTION (Option 11) ---
// ---------------------------------------------------------------------
void generateReports() {
    double totalInventoryValue = 0.0;
    double totalOrderValue = 0.0;
    
    printf("\n============================================\n");
    printf("        Supply Chain Management Report\n");
    printf("============================================\n");

    for (int i = 0; i < itemCount; i++) {
        totalInventoryValue += inventory[i].quantity * inventory[i].price;
    }
    
    printf("\n--- Inventory Summary ---\n");
    printf("Total Distinct Products: %d\n", itemCount);
    printf("TOTAL CURRENT INVENTORY VALUE: Rs.%.2lf\n", totalInventoryValue);
    printf("------------------------------------\n");

    for (int i = 0; i < orderCount; i++) {
        int inv_index = findItemIndex(pendingOrders[i].sku);
        
        if (inv_index != -1) {
            double item_price = inventory[inv_index].price;
            totalOrderValue += pendingOrders[i].quantity * item_price;
        } 
    }
    
    printf("\n--- Order Pipeline Summary ---\n");
    printf("Total Pending Orders: %d\n", orderCount);
    printf("TOTAL VALUE OF PENDING DISPATCH: Rs.%.2lf\n", totalOrderValue);
    printf("------------------------------------\n");

    if (totalOrderValue > totalInventoryValue * 0.5 && itemCount > 0) {
        printf("\n*** HIGH DEMAND ALERT: Pending order value is over 50%% of total inventory value! ***\n");
    } else if (itemCount == 0 && orderCount > 0) {
         printf("\n*** CRITICAL ALERT: Inventory is empty but %d orders are pending! ***\n");
    }
    printf("\nReport Generation Complete.\n");
    logAction("REPORT: Generated financial and operational report.");
}