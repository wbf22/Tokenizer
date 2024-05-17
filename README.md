# Tokenizer
C++ Tokenizer (sentance piece like) for Ai Language Models

## Open Source
This code is open source and free to use in any situation under the MIT license. Feel free to make a pr if you'd like to change something. Cheers!

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
you want your model to be trained to indicate when it has finished it's output. You may also use a token for masked words or other use cases. These custom
tokens MUST be greater than 1 character long and cannot be the reserved character '\<u\>' for unknown tokens.

Training should take ~1 min to run with a large file (4 million words or so). To avoid waiting everytime, save your
tokenizer like so
```
tokenizer.save("tokenizer-save-path");
```
Then load it like this
```
Tokenizer tokenizer = tokenizer.load("tokenizer-save-path");
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


## Technical Details
During training, tokens are selected as follows:
- the '\<u\>' token for unknown tokens
- user provided custom tokens
- common whitespace and punctuation tokens
- all unique characters in the training text
- common words in training text
- common word pieces in training text
All instance of these tokens are included in the tokenizers tokens, except for the words and word pieces. Found word and word pieces are chosen by how common they appear in the training text up to the num_tokens limit provided by the user.

When tokenizing the tokenizer walks through the string looking for custom tokens or whitespace and punctuation to split up words. These words are then matched to tokens or split up into sub pieces that match tokens.
During detokenization, the values are simply converted back to their respective tokens and appended together. 

### Example
Say the training text is 'I am pretty cool'
Running the training with the training text and a limit of 200 tokens, the tokenizer finds the following tokens
```
0,<u>
1,<s>
2,\n
3,\t
4, 
5,\r
6,.
7,,
8,!
9,?
10,~
11,`
12,@
13,#
14,$
15,%
16,^
17,&
18,*
19,(
20,)
21,-
22,_
23,=
24,+
25,\
26,|
27,{
28,[
29,}
30,;
31,:
32,'
33,"
34,<
35,>
36,/
37,a
38,I
39,p
40,e
41,m
42,r
43,t
44,y
45,c
46,o
47,l
48,ret
49,tty
50,am
51,co
52,pr
53,prett
54,pret
55,pre
56,re
57,rett
58,retty
59,et
60,coo
61,ett
62,etty
63,tt
64,ty
65,pretty
66,oo
67,ool
68,ol
69,cool
```
When parsing 'You are pretty cool' (tokenizing and then detokenizing) we get this:
```
<u>o<u> are pretty cool
```
The tokenizer matched everything except the characters 'Y' and 'u' since those weren't in the training text ('\<u\>' is for unknown characters). This typically won't happen with a large training text, but it's there still.

For more specifics I'd just look at the files. Tried my best to comment explain stuff, hopefully it's not to crazy in there. Chow!


