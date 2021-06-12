/*
 * Copyright 2018 Alberto Sonnino
 * 
 * This file is part of SGX-WALLET.
 * 
 * SGX-WALLET is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SGX-WALLET is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SGX-WALLET.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cstring>
#include <cstdlib>

#include "test.h"
#include "utils.h"
#include "../wallet/wallet.h"


/**
 * @brief      Runs the tests.
 *
 */
int test() {
    // declare return variables
	int ret_status;

    // test inputs
	const char master_password[MAX_ITEM_SIZE] = "This is the master-password";
    const char new_master_password[MAX_ITEM_SIZE] = "This is the new master-password";
    const char title[MAX_ITEM_SIZE] = "New Item Title";
    const char username[MAX_ITEM_SIZE] = "asonnino";
    const char password[MAX_ITEM_SIZE] = "test1234";

    
    warning_print("Only 'happy path' is tested.");
    

    ////////////////////////////////////////////////
    // test create wallet
    ////////////////////////////////////////////////
    // happy path
    ret_status = create_wallet(master_password);
    if (ret_status != RET_SUCCESS) {
        error_print("[TEST] Fail to create new wallet.");
        return 1;
    }
    info_print("[TEST] Wallet successfully created.");


    ////////////////////////////////////////////////
    // test add items
    ////////////////////////////////////////////////
    // happy path
    item_t* new_item = (item_t*)malloc(sizeof(item_t));
    strcpy(new_item->title, title); 
    strcpy(new_item->username, username); 
    strcpy(new_item->password, password);
    for (int i = 0; i < 2; ++i) {
        ret_status = add_item(master_password, new_item, sizeof(item_t));
        if (ret_status != RET_SUCCESS) {
            error_print("[TEST] Fail to add new item to wallet.");
            return 1;
        }
        info_print("[TEST] Item successfully added to the wallet.");
    }
    free(new_item);
    

    ////////////////////////////////////////////////
    // test change master password
    ////////////////////////////////////////////////
    // happy path
    ret_status = change_master_password(master_password, new_master_password);
    if (ret_status != RET_SUCCESS) {
        error_print("[TEST] Fail change master-password.");
        return 1;
    }
    info_print("[TEST] Master-password successfully changed.");



    ////////////////////////////////////////////////
    // test remove item
    ////////////////////////////////////////////////
    // happy path
    const int index = 1;
    ret_status = remove_item(new_master_password, index);
    if (ret_status != RET_SUCCESS) {
        error_print("[TEST] Fail to remove item.");
        return 1;
    }
    info_print("[TEST] Item successfully removed from the wallet.");


    ////////////////////////////////////////////////
    // test show wallet
    ////////////////////////////////////////////////
    // happy path
    wallet_t* wallet = (wallet_t*)malloc(sizeof(wallet_t));
    ret_status = show_wallet(new_master_password, wallet);
    if (ret_status != RET_SUCCESS) {
        error_print("[TEST] Fail to retrieve wallet.");
        return 1;
    }
    info_print("[TEST] Wallet successfully retrieved.");
    print_wallet(wallet);
    free(wallet);


    return 0;
}

