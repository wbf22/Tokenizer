
#pragma once


#include <torch/torch.h>
#include <iostream>
#include <fstream>
#include "../util/Util.h"

using namespace std;
using namespace torch;
using namespace torch::nn;



class Tokenizer {
public:
    static vector<string> white_space_and_punctuation;
    static string unknown_token;
    unordered_map<string, int> tokens;
    unordered_map<int, string> value_to_token;
    unordered_set<string> custom_tokens;



    Tokenizer(unordered_map<string, int> tokens, unordered_map<int, string> value_to_token, unordered_set<string> customTokens);
    
    static Tokenizer train(string trainingText, vector<string> customTokens, int numTokens);
    
    Tensor tokenize(string text);

    string detokenize(Tensor tensor);

    vector<string> detokenizeToVector(Tensor tensor);

    void save(string file_path);

    static Tokenizer load(string file_path);

private:




};