"""
clean_dict.py
Clarissa Verish

Takes in a dictionary containing the 10,000 most common words in the English
dictionary (excluding swears). Cleans out any proper nouns (names, places) and
words that are below the min length or above the max length. Creates a new
file "clean_dict.txt" containing clean words.

sources:
dict:                   https://github.com/first20hours/google-10000-english
cities:                 self
countries:              https://gist.github.com/kalinchernev/486393efcca01623b18d
days_and_months:        self
names:                  https://www.usna.edu/Users/cs/roche/courses/s15si335/proj1/files.php%3Ff=names.txt.html
states_and_provinces:   self and https://gist.github.com/norcal82/c219ecf9b932d7db4ee6
"""

# file containing most common english words
dictfile = "dict.txt"
# files containing words to exclude
#  to add a new
exclusions = ["cities.txt", "countries.txt", "days_and_months.txt",
              "manual_entries.txt", "names.txt", "states_and_provinces.txt"]
output = "clean_dict.txt"
# max and min length of words to include
minLength = 5
maxLength = 10

# list containing output words
outputWords = []

# hasVowels: splits word into letters, filters letters that are values, returns true if vowels, false if no vowels


def hasVowels(word):
    vowels = ["a", "e", "i", "o", "u", "y"]
    # making new array of letters in word that are a vowel : if len > 0, word has vowel
    return len([l for l in word if l in vowels]) > 0


# adding all words in original dictionary to outputWords if within length range
with open(dictfile) as f:
    lines = [line.strip("\n") for line in f.readlines()]
    outputWords = [word.lower() for word in lines if (
        len(word) >= minLength and len(word) <= maxLength and hasVowels(word))]

# going through each exclusion file and removing words from outputWords
for e in exclusions:
    with open("exclusions/" + e, "r") as exf:
        exWords = []
        for exword in exf:
            exWords.append(exword.lower().strip("\n"))
        outputWords = [word for word in outputWords if word not in exWords]

print(len(outputWords))
# adding new list of words to output file
with open(output, "w") as o:
    for word in outputWords:
        o.write(word + "\n")
