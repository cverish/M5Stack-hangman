# generate_dict

Takes in a txt file containing most-commonly-used English words, and outputs a txt file with excluded words removed.

## Setup

To generate a cleaned dictionary, navigate to 'utils/generate_dict' in a terminal and run 'main.py':
'''
$ cd utils/generate_dict
$ python3 main.py
'''

The file 'clean_dict.txt' will be output.

To exclude more words, either add individual words to 'exclusions/manual_entries.txt' or upload a txt file to 'exclusions' containing the words to exclude. Then, add the filename to the 'exclusions' list in 'main.py'.
