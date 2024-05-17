#include "Tokenizer.h"





vector<string> Tokenizer::white_space_and_punctuation = {
    "\n", "\t", " ", "\r", ".", ",",  "!", "?", 
    "~", "`", "@", "#", "$", "%", "^", "&", "*", 
    "(", ")", "-", "_", "=", "+", "\\", "|", "{", 
    "[", "}", "}", ";", ":", "'", "\"", ",", "<",
    ".", ">", "/", "?"
};

string Tokenizer::unknown_token = "<u>";


// updates the terminal output at the position (current - position)
static void updateTerminalCoutAtPosition(int position, const string& update)
{
    cout << "\033[" << position << "D" << update << flush;
}

// either adds whitespace or truncates a string to the desired length
static string fitToLength(string& str, int desired_length, bool append_on_front) {

    // truncate to size
    if (str.length() > desired_length) {
        str = str.substr(0, desired_length);
        return str;
    }


    // otherwise add whitespace
    int diff = desired_length - str.length();
    
    stringstream ss;
    if (!append_on_front) ss << str;

    for (int i = 0; i < diff; ++i) {
        ss << " ";
    }

    if (append_on_front) ss << str;

    str = ss.str();
    return str;
}

// converts a float to a string using the precision provided
static string floatToString(float num, int precision) {
    stringstream ss;
    ss << fixed << setprecision(precision) << num;
    return ss.str();
}

// initialize percent string for preporting progress
static void initializePercent() {
    cout << "   0.00%";
}

// for displaying percent string for reporting progress
static void reportPercent(int i, int total_iterations) {

    int percent_divisor = min(10000, total_iterations);
    int percent_length = total_iterations / percent_divisor;

    if (i % percent_length == 0) {
        int percent_length = total_iterations / percent_divisor;
        string percent = floatToString( (100.0 * i / percent_length / percent_divisor), 2) + "%";
        fitToLength(percent, 7, false);
        updateTerminalCoutAtPosition(7, percent);
    }
}

// for displaying percent string for reporting progress
static void report100Percent() {
    updateTerminalCoutAtPosition(7, "100.00%");
    cout << endl;
}

// replaces all 'words_to_replace' in 
static void replaceAll(string& str, const vector<string>& words_to_replace, const string& replacement) {
    stringstream ss;


    unordered_set<string> words_to_replace_set(words_to_replace.begin(), words_to_replace.end());
    unordered_set<int> word_lengths;
    for(string word : words_to_replace) 
        word_lengths.insert(word.length());


    initializePercent();
    for (int i = 0; i < str.length(); ++i) {

        string wordBeingReplaced = "";
        for (int word_length : word_lengths) {
            string sub = (word_length + i < str.length())? str.substr(i, word_length) : "";
            if ( words_to_replace_set.find(sub) != words_to_replace_set.end()) {
                wordBeingReplaced = sub;
                break;
            }
        }

        if (wordBeingReplaced != "") {
            i += wordBeingReplaced.length();
            ss << replacement;
            ss << str.at(i);
        }
        else 
            ss << str.at(i);

        reportPercent(i, str.length());
    }
    report100Percent();

    str = ss.str();
}

// splits a string by the given delimiters
static vector<string> split(const string& str, const vector<string>& single_char_delimiters) {

    vector<string> splits;
    unordered_set<char> delimiterFilter;
    for (string delimiter : single_char_delimiters)
        delimiterFilter.insert(delimiter.at(0));

    int last_split_end = 0;
    initializePercent();
    for (int i = 0; i < str.length(); ++i) {
        if ( delimiterFilter.find(str.at(i)) != delimiterFilter.end() ) {
            string split = str.substr(last_split_end, i - last_split_end);
            last_split_end = i + 1;
            if (!split.empty()) {
                bool isDelimiter = delimiterFilter.find(split.at(0)) != delimiterFilter.end();
                if (!isDelimiter)
                    splits.push_back(split);
            }
        }

        reportPercent(i, str.length());
    }

    string last_split = str.substr(last_split_end, str.length() - last_split_end);
    if (!last_split.empty()) {
        bool isDelimiter = delimiterFilter.find(last_split.at(0)) != delimiterFilter.end();
        if (!isDelimiter)
            splits.push_back(last_split);
    }

    report100Percent();

    return splits;
}

// get's all unique characters in the text
static unordered_set<char> getUniqueCharacters(const string& str) {
    unordered_set<char> unique_characters;

    initializePercent();
    for (int i = 0; i < str.length(); ++i) {
        unique_characters.insert(str.at(i));
        reportPercent(i, str.length());
    }

    report100Percent();

    return unique_characters;
}

// checks for custom tokens at the given index i
static string checkForCustomTokens(const string& text, const int& i, const unordered_set<string>& custom_tokens) {
    for (string token : custom_tokens) {
        if (i + token.length() <= text.length()) {
            string toke = text.substr(i, token.length());
            if (toke == token) {
                return token;
            }
        }
    }

    return "";
}

// returns the word if it exists in tokens, otherwise returns subtokens that match the word
static vector<string> determineTokensForWord(const string& word, unordered_map<string, int>& tokens) {

    vector<string> result;


    // check if word is in tokens
    if (tokens.find(word) != tokens.end()) 
        return {word};
    

    // otherwise, split into largest tokens possible (sort of)
    for (int i = 0; i < word.length(); ++i) {

        bool found_token = false;
        for (int len = word.length() - i; len > 0; --len) {
            string token = word.substr(i, len);
            if (tokens.find(token) != tokens.end()) {
                result.push_back(token);
                i += token.length() - 1;
                found_token = true;
                break;
            }
        }

        // otherwise 
        if (!found_token)
            result.push_back(Tokenizer::unknown_token);
    }
    
    
    return result;
}


// DON'T USE NORMALLY (use 'train' instead): Constructor for making a tokenizer. Probably shouldn't be used
Tokenizer::Tokenizer(unordered_map<string, int> tokens, unordered_map<int, string> value_to_token, unordered_set<string> custom_tokens) {
    this->tokens = tokens;
    this->value_to_token = value_to_token;
    this->custom_tokens = custom_tokens;
}

/**
 * Trains a tokenizer on a provided text. The resulting tokenizer will convert a string into a tensor of
 * integer values to be consumed by a langauge model. Takes ~1 min for a 4 million word training text.
 * 
 * @param custom_tokens tokens you want to always be tokenized out by the model. (Stop tokens, or whatever you like!)
 * These tokens must be longer than 1 character and cannot be '<u>' which is a reserved token for unkown symbols
*/  
Tokenizer Tokenizer::train(string training_text, vector<string> custom_tokens, int num_tokens = 50000)
{   

    int index = 0;
    unordered_map<string, int> tokens;

    // add unknown token
    tokens[Tokenizer::unknown_token] = index;
    ++index;

    // remove custom tokens and add those to tokens map first
    cout << "Preprocessing with custom tokens";
    replaceAll(training_text, custom_tokens, "");
    for (string custom_token : custom_tokens) {
        tokens[custom_token] = index;
        ++index;
        if (custom_token.length() < 2 || custom_token == Tokenizer::unknown_token) 
            throw runtime_error("Custom tokens must be at least 2 characters, and connot be '<u>'. ('<u>' is reserved for unknown tokens)");
    }

    // collect custom_tokens for later
    unordered_set<string> custom_tokens_set(custom_tokens.begin(), custom_tokens.end());

    // add whitespace and punctuation tokens
    for (string token : Tokenizer::white_space_and_punctuation) {
        if (tokens.find(token) == tokens.end()) {
            tokens[token] = index;
            ++index;
        }
    }

    // add all unique characters as tokens
    cout << endl << "Finding all unique characters";
    unordered_set<char> unique_chars = getUniqueCharacters(training_text);
    for (char c : unique_chars) {
        string char_str = string(1, c);
        if (tokens.find(char_str) == tokens.end()) {
            tokens[char_str] = index;
            ++index;
        }
    }
    cout << "Found " << to_string(unique_chars.size()) << " unique characters in the training_text" << endl;

    int numSubTokens = num_tokens / 10;
    if (num_tokens < index) 
        throw runtime_error("num_tokens is too small. The minimum amount of tokens needed for this training_text is " + to_string(index) + " tokens.");


    // split the text into words
    cout << endl << "Finding all unique words";
    vector<string> all_words = split(training_text, Tokenizer::white_space_and_punctuation);
    cout << "Analyzing " << to_string(all_words.size()) << " words";
    
    // collect token/word frequencies
    initializePercent();
    unordered_map<string, int> tokenToFrequency;
    for (int w = 0; w < all_words.size(); ++w) {
        string word = all_words[w];

        // collect token frequencies
        int word_length = word.length();
        for (int i = 0; i < word_length; ++i) {
            
            // try tokens of size 2-6
            for (int s = 2; s < 6; ++s) {

                int end = i + s;
                bool is_entire_word = ( i == 0 && end == word_length );
                if ( end <= word_length && !is_entire_word ) {
                    string sub_word = word.substr(i, s);
                    ++tokenToFrequency[sub_word];
                }
                else break;
                
            }
        }

        // collect word frequency
        ++tokenToFrequency[word];

        reportPercent(w, all_words.size());
    }

    report100Percent();


    // sort and add top frequencies up to 'num_tokens' to tokens
    cout << "Sorting to select " << to_string(num_tokens) << " (or less) tokens" << endl;
    vector<pair<string, int>> tokensSorted(tokenToFrequency.begin(), tokenToFrequency.end());
    sort(tokensSorted.begin(), tokensSorted.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
        }
    );

    for (pair<string, int> pair : tokensSorted) {
        if (index >= num_tokens) break;
        if (tokens.find(pair.first) == tokens.end()) {
            tokens[pair.first] = index;
            ++index;
        }
    }



    cout << endl << "Done!" << endl;
    cout << "Used " << to_string(index) << " tokens" << endl;

    
    unordered_map<int, string> value_to_token;
    for (pair<string, int> pair : tokens) {
        value_to_token[pair.second] = pair.first;
    }
    Tokenizer tokenizer = Tokenizer(tokens, value_to_token, custom_tokens_set);

    
    // perform test to analyze coverage
    cout << "Performing test ";
    initializePercent();
    long text_length = training_text.length();
    double tokens_observed = 0;
    vector<int> token_use_frequency = {0, 0, 0, 0, 0, 0};
    for (int i = 0; i < 100; ++i) {
        long test_start = (text_length - 1) * i / 100;
        int max_len = text_length - test_start;
        int test_length = (max_len < 1000)? max_len : 1000;
        string test_str = training_text.substr(test_start, test_length);
        
        // count up unknown tokens after tokenizing
        Tensor tensor = tokenizer.tokenize(test_str);
        vector<string> tokens = tokenizer.detokenizeToVector(tensor);
        for (string token : tokens) {
            int len = min(6, (int)token.length());
            token_use_frequency[len-1] += token.length();
        }

        tokens_observed += test_length;

        reportPercent(i, 100);
    }
    report100Percent();

    cout << "Achieved coverage on test samples:" << endl;
    cout << to_string( 100 * (token_use_frequency[5]) / tokens_observed ) << "%" << " of characters with tokens longer than 5 characters" << endl;
    cout << to_string( 100 * (token_use_frequency[4]) / tokens_observed ) << "%" << " of characters with 5 character tokens" << endl; 
    cout << to_string( 100 * (token_use_frequency[3]) / tokens_observed ) << "%" << " of characters with 4 character tokens" << endl; 
    cout << to_string( 100 * (token_use_frequency[2]) / tokens_observed ) << "%" << " of characters with 3 character tokens" << endl; 
    cout << to_string( 100 * (token_use_frequency[1]) / tokens_observed ) << "%" << " of characters with 2 character tokens" << endl; 
    cout << to_string( 100 * (token_use_frequency[0]) / tokens_observed ) << "%" << " of characters with 1 character tokens" << endl;
    cout << "To see selected tokens, save your tokenizer and inspect the save file" << endl;
    cout << endl; 


    
    return tokenizer;
}



Tensor Tokenizer::tokenize(string text)
{
    unordered_set<string> white_space_and_punctuation_set(Tokenizer::white_space_and_punctuation.begin(), Tokenizer::white_space_and_punctuation.end());

    Tensor tensor = torch::empty(0);

    for (int i = 0; i < text.length(); ++i) {
        string value = "";

        // check for custom tokens
        string toke = checkForCustomTokens(text, i, this->custom_tokens);
        if (!toke.empty()) {
            value = toke;
            i += toke.length();
        }

        // check for whitespace or punctuation
        if (value.empty()) {
            string currentChar = string(1, text.at(i));
            
            if (white_space_and_punctuation_set.find(currentChar) != white_space_and_punctuation_set.end())
                value = currentChar;
            
        }

        if (value.empty()) {
            // check for words (breaking out when you hit whitespace or hit a custom token)
            string word = "";
            string currentChar = string(1, text.at(i));
            string custom_token = checkForCustomTokens(text, i, this->custom_tokens);
            while(custom_token.empty() && white_space_and_punctuation_set.find(currentChar) == white_space_and_punctuation_set.end() && i+1 < text.length()) {
                ++i;

                // add onto word
                word += currentChar;
                currentChar = string(1, text.at(i));

                // check for custom tokens
                custom_token = checkForCustomTokens(text, i, this->custom_tokens);
                if (!custom_token.empty()) break;

            }
            

            // match to a word or multiple tokens
            vector<string> tokensInWord = determineTokensForWord(word, this->tokens);
            for (string word_toke : tokensInWord)
                tensor = torch::cat({tensor, torch::tensor({this->tokens[word_toke]})});
            

            if (!custom_token.empty()) {
                value = custom_token;
                i += custom_token.length() - 1;
            }
            else
                value = currentChar;
            
        }

        tensor = torch::cat({tensor, torch::tensor({this->tokens[value]})});

    }

    return tensor;
}

string Tokenizer::detokenize(Tensor tensor)
{
    stringstream ss;

    int len = tensor.sizes()[0];
    for (int i = 0; i < len; ++i) {
        int value = tensor[i].item<int>();
        string token = this->value_to_token[value];
        ss << token;
    }


    return ss.str();
}

vector<string> Tokenizer::detokenizeToVector(Tensor tensor)
{
    vector<string> strings;

    int len = tensor.sizes()[0];
    for (int i = 0; i < len; ++i) {
        int value = tensor[i].item<int>();
        string token = this->value_to_token[value];
        strings.push_back(token);
    }


    return strings;
}

void Tokenizer::save(string file_path) {
    unordered_map<string, string> escaped_special_values = {{"\n", "\\n"}, {"\t", "\\t"}, {"\r", "\\r"}};

    vector<pair<string, int>> tokensSorted(this->tokens.begin(), this->tokens.end());
    sort(tokensSorted.begin(), tokensSorted.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second < b.second;
        }
    );


    ofstream file(file_path);
    if (file.is_open()) {
        for (pair<string, int> token_and_value : tokensSorted) {
            string token = token_and_value.first;
            int value = token_and_value.second;
            if (escaped_special_values.find(token) != escaped_special_values.end()) {
                token = escaped_special_values[token];
            }

            file << to_string(value) << "," << token << endl;
        }

        file.close();
        cout << "Tokenizer saved successfully." << endl;
    } else {
        cout << "Unable to open the file." << endl;
    }
}


Tokenizer Tokenizer::load(string file_path) {
    unordered_map<string, string> escaped_special_values = {{"\\n", "\n"}, {"\\t", "\t"}, {"\\r", "\r"}};
    unordered_map<string, int> tokens;

    ifstream file(file_path);
    if (!file.is_open()) {
        throw runtime_error("Couldn't read file " + file_path);
    }

    string line;
    while (getline(file, line)) {

        int index = 0;
        while (line.at(index) != ',') 
            ++index;

        int value = stoi(line.substr(0, index));
        string token = line.substr(index + 1, line.length() - index - 1);
        if (escaped_special_values.find(token) != escaped_special_values.end()) {
            token = escaped_special_values[token];
        }
        tokens[token] = value;
    }

    file.close();

    unordered_map<int, string> value_to_token;
    for (pair<string, int> pair : tokens) {
        value_to_token[pair.second] = pair.first;
    }

    unordered_set<string> custom_tokens;
    int index = 1;
    string next_toke = value_to_token[index];
    while (next_toke != "\n") {
        custom_tokens.insert(next_toke);
        ++index;
        next_toke = value_to_token[index];
    }

    cout << "Tokenizer loaded successfully." << endl;
    return Tokenizer(tokens, value_to_token, custom_tokens);
}
