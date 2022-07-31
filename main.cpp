#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <map>

struct args { //args for the frequency function
    char key;
    int val;
    int size;
    int bits;
    std::string msg; //compressed msg
    int freq;
    std::string binval; //binary value as a string
};

struct args2 {
    char* keys; //array of keys
    std::string* binvals; //array of binary values
    std::string decomp_msg;
    std::string msg; //compressed msg
    int size;
    int bits;
};

std::string bin(int n, int b){ //converts an int (n) to a binary value in the size of b (number of bits)
    std::string s;
    for(int i = b-1; i >= 0; i--){
        int k = n >> i;
        s += (k & 1) ? "1" : "0"; //prob unnecessary to use a ternery operator here but meh I like my code compact
    }
    return s;
}

void* frequency(void* void_ptr){ 
    struct args *ptr = (struct args*)void_ptr;
    ptr->binval = bin(ptr->val, ptr->bits);
    for(int i = 0; i < ptr->msg.length(); i += ptr->bits){ //finds the frequency of a given char by comparing it's binary value to every 'b' bits of the compressed msg
        if(ptr->msg.substr(i, ptr->bits) == ptr->binval)
            ptr->freq += 1;
    }
    return NULL;
}
//decompresses by traversing every 'b' bits and searching for its corresponding key in the arrays of keys and binvals, the key is then concated into the string
void* decompress(void* void_ptr2){
    struct args2 *ptr = (struct args2*)void_ptr2;    
    for(int i = 0; i < ptr->msg.length(); i += ptr->bits){ 
        for(int j = 0; j < ptr->size; j++) {
            if(ptr->msg.substr(i, ptr->bits) == ptr->binvals[j])
                ptr->decomp_msg += ptr->keys[j];
        }
    }
    return NULL;
}

int main(){
    std::string line, comp_msg;
    
    std::string n;
    getline(std::cin, n);
    const int numOfSymbols = std::stoi(n);
    std::vector<std::pair<char, int>> m; //vector of pairs will act as an unordered_map in order to retain the input order of key-value pairs

    while(getline(std::cin, line)){     
        if(line.length() <= 5){ //when the length is greater than 4 (one key, one space, and up to 2 digits for the val): compressed msg
            std::string s = line.substr(2);
            int val = std::stoi(s);
            m.push_back(std::make_pair(line[0], val));
        }
        else { 
            comp_msg = line;
            break;
        }
    }

    //finding max using max_element, iterates and finds the pair with the highest val
    const auto maxp = std::max_element(m.begin(), m.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
    static int numOfBits = std::ceil(std::log2(maxp->second + 1));

    //mooolti-threading for the frequency n threads = numOfSymbols
    pthread_t* tid = new pthread_t[numOfSymbols];

    static struct args* x = new args[numOfSymbols];
    
    int i = 0;
    for (auto it = m.begin(); it != m.end(); ++it) { //filling from the "unordered map" to x
        x[i].key = it->first;
        x[i].val = it->second;
        x[i].bits = numOfBits;
        x[i].msg = comp_msg;
        x[i].size = numOfSymbols;
        i++;
    }

    //creating the threads, and calling pthread_join afterwards
    for(int i = 0; i < numOfSymbols; i++){
        if(pthread_create(&tid[i], NULL, frequency, &x[i])){
            std::cout << "error creating thread:" << stderr << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < numOfSymbols; i++)
        pthread_join(tid[i], NULL); 
    
    //printing binary values and frequencies from x
    std::cout << "Alphabet:" << std::endl;
    for(int i = 0; i < numOfSymbols; i++)
        std::cout << "Character: " << x[i].key << ", Code: " << x[i].binval << ", Frequency: " << x[i].freq << std::endl;

    //pthreads for the decompression
    const int MTHREADS = comp_msg.length() / numOfBits;
    static struct args2* y = new args2 [MTHREADS];
    //filling up y
    y->size = numOfSymbols;
    y->bits = numOfBits;
    y->msg = comp_msg;
    y->keys = new char[numOfSymbols];
    y->binvals = new std::string[numOfSymbols];

    for (int i = 0; i < numOfSymbols; i++) { //filling the dynamic arrays of y
        y->keys[i] = x[i].key;
        y->binvals[i] = x[i].binval;
    }   
    //creating m threads, calling pthread_join, printing our decompressed message, and finally deallocating the memory form our dynamic arrays :)
    pthread_t* tid2 = new pthread_t [MTHREADS];
    for(int i = 0; i < MTHREADS; i++){
        if(pthread_create(&tid2[i], NULL, decompress, &y[i])){
            std::cout << "error creating thread:" << stderr << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < MTHREADS; i++)
        pthread_join(tid2[i], NULL);
    
    std::cout << "\nDecompressed message: " << y->decomp_msg<< std::endl;

    delete[] tid;
    delete[] tid2;
    delete[] x;
    delete[] y;
}