#include "save_ascii_2024.h"
#include "netxpto_20200819.h"
#include <typeinfo>
#include <iostream>
#include <memory>
#include "key_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define KEY_SIZE_BITS 32
#define SEED_SIZE_BITS 32 // Duas seeds de 32 bits cada
/* #define SIGNAL_SIZE_BITS (KEY_SIZE_BITS / 2) // Metade do tamanho da chave
 */
void KeyGenerator::initialize(void){   
}


bool KeyGenerator::runBlock(){

     // Gera a chave "oblivious"
        uint8_t tx_key[KEY_SIZE_BITS+1];
        uint8_t rx_key[KEY_SIZE_BITS+1];

    switch (Key_type)
    {
    case 0:
        if (getTerminated()) return false;


        if (getFirstTime()){
            seed ;
        }
        
        std::cout << "seed " << seed << "\n";

        // Verifica se o tamanho da chave é válido
        if (!validate_key_size(KEY_SIZE_BITS)) {
            printf("Erro: O tamanho da chave deve ser maior que 0.\n");
            return 1;
        }

 

        if (!getFirstTime()){
            seed = seed + 1;
        }

        

        generate_random_key(tx_key, KEY_SIZE_BITS, seed);
        
        // Adiciona a chave "oblivious" ao output signal
        for(int i = 0; i < KEY_SIZE_BITS; i++){
            outputSignals[0]->bufferPut((t_binary)(tx_key[i]));
        }

        for(int i = 0; i < KEY_SIZE_BITS; i++){
            outputSignals[1]->bufferPut((t_binary)(tx_key[i]));
        }

        setFirstTime(false);
        return true;
        
    case 1:
    
        if (getTerminated()) return false;


        if (getFirstTime()){
            seed;
        }
        
        std::cout << "seed tx: " << seed << "\n";

        // Verifica se o tamanho da chave é válido
        if (!validate_key_size(KEY_SIZE_BITS)) {
            printf("Erro: O tamanho da chave deve ser maior que 0.\n");
            return 1;
        }


        if (!getFirstTime()){
            seed = seed + 1;
        }

        std::cout << "seed tx: " << seed << "\n";
        rx_seed = seed + 1;

        generate_random_key(tx_key, KEY_SIZE_BITS, seed);
        generate_oblivious_key(tx_key, rx_key, rx_seed);


        // Adiciona a chave "oblivious" ao output signal
        for(int i = 0; i < KEY_SIZE_BITS; i++){
            outputSignals[0]->bufferPut((t_binary)(rx_key[i]));
        }

        for(int i = 0; i < KEY_SIZE_BITS; i++){
            outputSignals[1]->bufferPut((t_binary)(tx_key[i]));
        }

        
        for (int i = 0; i < KEY_SIZE_BITS; i++) {
            printf("%d ", tx_key[i]);
        }

        printf("\n");
        
        // Imprime a chave "oblivious" gerada
        
        for (int i = 0; i < KEY_SIZE_BITS; i++) {
            printf("%d ", rx_key[i]);
        }
        printf("\n");

        //if (outputSignals[0]->space() == 0) alive = false;
        setFirstTime(false);
        return true;
     
    
   
    default:
        break;
    }
   
}