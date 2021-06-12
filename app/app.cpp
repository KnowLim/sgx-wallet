#include <cstring>
#include <getopt.h>
#include <cstdlib>
#include <stdio.h>
#include <ctype.h>

#include "utils.h"
#include "../include/debug.h"
#include "../wallet/wallet.h"
#include "test.h"

using namespace std;

int main(int argc, char** argv) {
    // declare return variables
    int updated, ret_status;


    ////////////////////////////////////////////////
    // read input arguments 
    ////////////////////////////////////////////////
    const char* options = "hvtn:p:c:sax:y:z:r:";
    opterr=0; // prevent 'getopt' from printing err messages
    char err_message[100];
    int opt, stop=0;
    int h_flag=0, v_flag=0, s_flag=0, a_flag=0, t_flag=0;
    char * n_value=NULL, *p_value=NULL, *c_value=NULL, *x_value=NULL, *y_value=NULL, *z_value=NULL, *r_value=NULL;
  
    // read user input
    while ((opt = getopt(argc, argv, options)) != -1) {
        switch (opt) {
            // help
            case 'h':
                h_flag = 1;
                break;

            // version
            case 'v':
                v_flag = 1;
                break;

            // run tests
            case 't':
                t_flag = 1;
                break;

            // create new wallet
            case 'n':
                n_value = optarg;
                break;

            // master-password
            case 'p':
                p_value = optarg;
                break;

            // change master-password
            case 'c':
                c_value = optarg;
                break;

            // show wallet
            case 's':
                s_flag = 1;
                break;

            // add item
            case 'a': // add item flag
                a_flag = 1;
                break;
            case 'x': // item's title
                x_value = optarg;
                break;
            case 'y': // item's username
                y_value = optarg;
                break;
            case 'z': // item's password
                z_value = optarg;
                break;

            // remove item
            case 'r':
                r_value = optarg;
                break;

            // exceptions
            case '?':
                if (optopt == 'n' || optopt == 'p' || optopt == 'c' || optopt == 'r' ||
                    optopt == 'x' || optopt == 'y' || optopt == 'z'
                ) {
                    sprintf(err_message, "Option -%c requires an argument.", optopt);
                }
                else if (isprint(optopt)) {
                    sprintf(err_message, "Unknown option `-%c'.", optopt);
                }
                else {
                    sprintf(err_message, "Unknown option character `\\x%x'.",optopt);
                }
                stop = 1;
                error_print(err_message);
                error_print("Program exiting.");
                break;

            default:
                error_print("Unknown option.");
        }
    }


    ////////////////////////////////////////////////
    // perform actions
    ////////////////////////////////////////////////
    if (stop != 1) {
        // show help
        if (h_flag) {
            show_help();
        }

        // show version
        else if (v_flag) {
            show_version();
        }

        // run tests
        else if(t_flag) {
            info_print("Running tests...");
            if (test() != 0) {error_print("One or more tests failed.");}
            else {info_print("All tests successfully passed.");}
        }

        // create new wallet
        else if(n_value!=NULL) {
            ret_status = create_wallet(n_value);
            if (ret_status != RET_SUCCESS) {
                error_print("Fail to create new wallet.");
            }
            else {
                info_print("Wallet successfully created.");
            }
        }

        // change master-password
        else if (p_value!=NULL && c_value!=NULL) {
            ret_status = change_master_password(p_value, c_value);
            if (ret_status != RET_SUCCESS) {
                error_print("Fail change master-password.");
            }
            else {
                info_print("Master-password successfully changed.");
            }
        }

        // show wallet
        else if(p_value!=NULL && s_flag) {
            wallet_t* wallet = (wallet_t*)malloc(sizeof(wallet_t));
            ret_status = show_wallet(p_value, wallet);
            if (ret_status != RET_SUCCESS) {
                error_print("Fail to retrieve wallet.");
            }
            else {
                info_print("Wallet successfully retrieved.");
                print_wallet(wallet);
            }
            free(wallet);
        }

        // add item
        else if (p_value!=NULL && a_flag && x_value!=NULL && y_value!=NULL && z_value!=NULL) {
            item_t* new_item = (item_t*)malloc(sizeof(item_t));
            strcpy(new_item->title, x_value); 
            strcpy(new_item->username, y_value); 
            strcpy(new_item->password, z_value);
            ret_status = add_item(p_value, new_item, sizeof(item_t));
            if (ret_status != RET_SUCCESS) {
                error_print("Fail to add new item to wallet.");
            }
            else {
                info_print("Item successfully added to the wallet.");
            }
            free(new_item);
        }

        // remove item
        else if (p_value!=NULL && r_value!=NULL) {
            char* p_end;
            int index = (int)strtol(r_value, &p_end, 10);
            if (r_value == p_end) {
                error_print("Option -r requires an integer argument.");
            }
            else {
                ret_status = remove_item(p_value, index);
                if (ret_status != RET_SUCCESS) {
                    error_print("Fail to remove item.");
                }
                else {
                    info_print("Item successfully removed from the wallet.");
                }
            }
        }

        // display help
        else {
            error_print("Wrong inputs.");
            show_help();
        }
    }


    ////////////////////////////////////////////////
    // exit success
    ////////////////////////////////////////////////
    info_print("Program exit success.");
    return 0;
}
