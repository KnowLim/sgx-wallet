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

#ifndef WALLET_H_
#define WALLET_H_


/***************************************************
 * Defines
 ***************************************************/
#define MAX_ITEMS 100
#define MAX_ITEM_SIZE 100
#define WALLET_FILE "wallet.seal"

#define RET_SUCCESS 0
#define ERR_PASSWORD_OUT_OF_RANGE 1
#define ERR_WALLET_ALREADY_EXISTS 2
#define ERR_CANNOT_SAVE_WALLET 3
#define ERR_CANNOT_LOAD_WALLET 4
#define ERR_WRONG_MASTER_PASSWORD 5
#define ERR_WALLET_FULL 6
#define ERR_ITEM_DOES_NOT_EXIST 7
#define ERR_ITEM_TOO_LONG 8


/***************************************************
 * Struct
 ***************************************************/
// item
struct Item {
	char  title[MAX_ITEM_SIZE];
	char  username[MAX_ITEM_SIZE];
	char  password[MAX_ITEM_SIZE];
};
typedef struct Item item_t;

// wallet
struct Wallet {
	item_t items[MAX_ITEMS];
	size_t size;
	char master_password[MAX_ITEM_SIZE];
};
typedef struct Wallet wallet_t;


/***************************************************
 * Functions
 ***************************************************/
void debug_print(const char* str);
int save_wallet(const wallet_t* wallet);
int load_wallet(const wallet_t* wallet);
int is_wallet(void);
int create_wallet(const char* master_password);
int show_wallet(const char* master_password, wallet_t* wallet);
int change_master_password(const char* old_password, const char* new_password);
int add_item(const char* master_password, const item_t* item, const size_t item_size);
int remove_item(const char* master_password, const int index);


#endif // WALLET_H_