#include <cstring>
#include <stdio.h>
#include <fstream>
#include <cstdlib>

#include "../include/debug.h"
#include "wallet.h"

using namespace std;

/**
 * @brief      Prints a message to the console. 
 *
 */
void debug_print(const char* str) {
    printf("[DEBUG] %s\n", str);
}

/**
 * @brief      Save sealed data to file The sizes/length of 
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int save_wallet(const wallet_t* wallet) {
    FILE *file = fopen (WALLET_FILE, "w");
    if (file == NULL) {return 1;}
    fwrite (wallet, sizeof(wallet_t), 1, file);
    fclose (file);
    return 0;
}

/**
 * @brief      Load sealed data from file The sizes/length of 
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int load_wallet(wallet_t* wallet) {
    FILE *file = fopen (WALLET_FILE, "r");
    if (file == NULL) {return 1;}
    fread (wallet, sizeof(wallet_t), 1, file);
    fclose (file);
    return 0;
}

/**
 * @brief      Verifies if a wallet files exists.
 *
 */
int is_wallet(void) {
    FILE *file = fopen (WALLET_FILE, "r");
    if (file == NULL) {return 0;}
    fclose (file);
    return 1;
}


/**
 * @brief      Creates a new wallet with the provided master-password.
 *
 */
int create_wallet(const char* master_password) {

	//
	// OVERVIEW:
	//	1. check password policy
	//	2. [ocall] abort if wallet already exist
	//	3. create wallet
	//	4. seal wallet
	//	5. [ocall] save wallet
	//	6. exit enclave
	//

	DEBUG_PRINT("CREATING NEW WALLET...");


	// 1. check passaword policy
	if (strlen(master_password) < 8 || strlen(master_password)+1 > MAX_ITEM_SIZE) {
		return ERR_PASSWORD_OUT_OF_RANGE;
	}
	DEBUG_PRINT("[OK] Password policy successfully checked.");


	// 2. abort if wallet already exist
	if (is_wallet() != 0) {
		return ERR_WALLET_ALREADY_EXISTS;
	}
	DEBUG_PRINT("[OK] No pre-existing wallets.");


	// 3. create new wallet
	wallet_t* wallet = (wallet_t*)malloc(sizeof(wallet_t));
	wallet->size = 0;
	strncpy(wallet->master_password, master_password, strlen(master_password)+1);
	DEBUG_PRINT("[OK] New wallet successfully created.");


	// 4. save wallet
	int saving_status = save_wallet(wallet);
	free(wallet);
	if (saving_status != 0) {
		return ERR_CANNOT_SAVE_WALLET;
	}
	DEBUG_PRINT("[OK] New wallet successfully saved.");


	DEBUG_PRINT("WALLET SUCCESSFULLY CREATED.");
	return RET_SUCCESS;
}


/**
 * @brief      Provides the wallet content. The sizes/length of
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int show_wallet(const char* master_password, wallet_t* wallet) {

	//
	// OVERVIEW:
	//	1. [ocall] load wallet
	// 	2. unseal wallet
	//	3. verify master-password
	//	4. return wallet to app
	//	5. exit enclave
	//

	DEBUG_PRINT("RETURNING WALLET TO APP...");


	// 1. load wallet
	if (load_wallet(wallet) != 0) {
		free(wallet);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 2. verify master-password
	if (strcmp(wallet->master_password, master_password) != 0) {
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	DEBUG_PRINT("WALLET SUCCESSFULLY RETURNED TO APP.");
	return RET_SUCCESS;
}


/**
 * @brief      Changes the wallet's master-password.
 *
 */
int change_master_password(const char* old_password, const char* new_password) {

	//
	// OVERVIEW:
	//	1. check password policy
	//	2. [ocall] load wallet
	// 	3. unseal wallet
	//	4. verify old password
	//	5. update password
	//	6. seal wallet
	// 	7. [ocall] save sealed wallet
	//	8. exit enclave
	//

	DEBUG_PRINT("CHANGING MASTER PASSWORD...");


	// 1. check passaword policy
	if (strlen(new_password) < 8 || strlen(new_password)+1 > MAX_ITEM_SIZE) {
		return ERR_PASSWORD_OUT_OF_RANGE;
	}
	DEBUG_PRINT("[ok] Password policy successfully checked.");


	// 2. load wallet
	wallet_t* wallet = (wallet_t*)malloc(sizeof(wallet_t));
	if (load_wallet(wallet) != 0) {
		free(wallet);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 3. verify master-password
	if (strcmp(wallet->master_password, old_password) != 0) {
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	// 5. update password
	strncpy(wallet->master_password, new_password, strlen(new_password)+1);
	DEBUG_PRINT("[ok] Successfully updated master-password.");


	// 6. save wallet
	int saving_status = save_wallet(wallet);
	free(wallet);
	if (saving_status != 0) {
		return ERR_CANNOT_SAVE_WALLET;
	}
	DEBUG_PRINT("[OK] Wallet successfully saved.");


	// 6. exit enclave
	DEBUG_PRINT("MASTER PASSWORD SUCCESSFULLY CHANGED.");
	return RET_SUCCESS;
}


/**
 * @brief      Adds an item to the wallet. The sizes/length of
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int add_item(const char* master_password, const item_t* item, const size_t item_size) {

	//
	// OVERVIEW:
	//	1. [ocall] load wallet
	//	2. unseal wallet
	//	3. verify master-password
	//	4. check input length
	//	5. add item to the wallet
	//	6. seal wallet
	//	7. [ocall] save sealed wallet
	//	8. exit enclave
	//

	DEBUG_PRINT("ADDING ITEM TO THE WALLET...");


	// 2. load wallet
	wallet_t* wallet = (wallet_t*)malloc(sizeof(wallet_t));
	if (load_wallet(wallet) != 0) {
		free(wallet);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 3. verify master-password
	if (strcmp(wallet->master_password, master_password) != 0) {
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	// 4. check input length
	if (strlen(item->title)+1 > MAX_ITEM_SIZE ||
		strlen(item->username)+1 > MAX_ITEM_SIZE ||
		strlen(item->password)+1 > MAX_ITEM_SIZE
	) {
		free(wallet);
		return ERR_ITEM_TOO_LONG;
	}
	DEBUG_PRINT("[ok] Item successfully verified.");


	// 5. add item to the wallet
	size_t wallet_size = wallet->size;
	if (wallet_size >= MAX_ITEMS) {
		free(wallet);
		return ERR_WALLET_FULL;
	}
	wallet->items[wallet_size] = *item;
	++wallet->size;
	DEBUG_PRINT("[OK] Item successfully added.");

	// 6. save wallet
	int saving_status = save_wallet(wallet);
	free(wallet);
	if (saving_status != 0) {
		return ERR_CANNOT_SAVE_WALLET;
	}
	DEBUG_PRINT("[OK] Wallet successfully saved.");


	DEBUG_PRINT("ITEM SUCCESSFULLY ADDED TO THE WALLET.");
	return RET_SUCCESS;
}


/**
 * @brief      Removes an item from the wallet. The sizes/length of
 *             pointers need to be specified, otherwise SGX will
 *             assume a count of 1 for all pointers.
 *
 */
int remove_item(const char* master_password, const int index) {

	//
	// OVERVIEW:
	//	1. check index bounds
	//	2. [ocall] load wallet
	//	3. unseal wallet
	//	4. verify master-password
	//	5. remove item from the wallet
	//	6. seal wallet
	//	7. [ocall] save sealed wallet
	//	8. exit enclave
	//

	DEBUG_PRINT("REMOVING ITEM FROM THE WALLET...");


	// 1. check index bounds
	if (index < 0 || index >= MAX_ITEMS) {
		return ERR_ITEM_DOES_NOT_EXIST;
	}
	DEBUG_PRINT("[OK] Successfully checked index bounds.");


	// 2. load wallet
	wallet_t* wallet = (wallet_t*)malloc(sizeof(wallet_t));
	if (load_wallet(wallet) != 0) {
		free(wallet);
		return ERR_CANNOT_LOAD_WALLET;
	}
	DEBUG_PRINT("[ok] Wallet successfully loaded.");


	// 3. verify master-password
	if (strcmp(wallet->master_password, master_password) != 0) {
		free(wallet);
		return ERR_WRONG_MASTER_PASSWORD;
	}
	DEBUG_PRINT("[ok] Master-password successfully verified.");


	// 4. remove item from the wallet
	size_t wallet_size = wallet->size;
	if (index >= wallet_size) {
		free(wallet);
		return ERR_ITEM_DOES_NOT_EXIST;
	}
	for (int i = index; i < wallet_size-1; ++i) {
		wallet->items[i] = wallet->items[i+1];
	}
	--wallet->size;
	DEBUG_PRINT("[OK] Item successfully removed.");



	// 5. save wallet
	int saving_status = save_wallet(wallet);
	free(wallet);
	if (saving_status != 0) {
		return ERR_CANNOT_SAVE_WALLET;
	}
	DEBUG_PRINT("[OK] Wallet successfully saved.");


	DEBUG_PRINT("ITEM SUCCESSFULLY REMOVED FROM THE WALLET.");
	return RET_SUCCESS;
}
