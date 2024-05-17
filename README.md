# Tokenizer
C++ Tokenizer (sentance piece like) for Ai Language Models

## Usage
There are only two files for the tokenizer (header and cpp), so just paste them into your project for use. To use this tokenizer, you'll need to have
libtorch installed to have access to torch tensors. Alternativly you can modify the tokenize and detokenize functions to use some other data structure
and avoid the torch dependency.

Here's how you can create a tokenizer
```
string training_text = ...;
Tokenizer tokenizer = Tokenizer::train(training_text, {"<s>", "<b>"}, 50000);
```
Here we provide a training text string, two custom tokens, and the max number of tokens we want the tokenizer to collect. The custom tokens are handy if
you want your model to be trained to indicate when it has finished it's output. You may also use a token for masked words or other use cases.

This should take ~1 min to run with a large file (4 million words or so). To avoid waiting everytime, save your
tokenizer like so
```
tokenizer.save("tokenizer-save-path");
```
If you want to inspect the tokens chosen by the tokenizer, inspecting the save file will make this easy.

You can use your tokenizer to convert an arbitrary string into a tensor of int values like so
```
torch::Tensor res = tokenizer.tokenize(text);
```

And detokenize an int tensor like so
```
string detokenized = tokenizer.detokenize(res);
```

