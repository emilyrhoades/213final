The purpose of this program is to perform a textual analysis on word association.

Input:
It take a file to analyze as input. All files must be in .txt format and the file names must be less than 300 characters.
Then, it takes a list of words (comparison words) to look for in the text. Any word can be at most 30 characters and at most 100 words can be entered.
Each of these words should be on a new line. There may be no punctuation.
Finally, the program will prompt for a word that the others will be compared to (main word). This can be at most 30 characters long.
The main word may not be the same as any of the comparison words.

Output:
The results are written to a file called "resuts.txt". At the top, the main word and file will be listed.
Each word from the comparison word file will be listed with the corresponding number of times it ouccured in a sentence with the main word.
The proportion of the times one comparison word appeared in a sentence with the main word over the total number of times a comparison word
appeard in a sentence with the main word will also be listed. If the program is run again, the new results will be listed below the previous results.

Example:
1) Text file to be analyzed: Don Sebastian by Anna Porter (file: don.txt)
    Input when prompted: don.txt

2) Comparison Words: A file with the contents (these are the primary characters in the book):
    des_words.txt:  
    Sebastian
    Kara

    Input when prompted: des_words.txt

3) Main word
    Input when prompted: love

Results:

The selected file is 'don.txt'
The selected word is 'love'
Sebastian:   9   0.47
Kara:   10   0.53

Interpretation: "love" appears in a sentence with "Sebastian" 9 times and in a sentence with "Kara" 10 times.
                Of the total times "love" appeared in a sentence with "Sebastian" or "Kara", 47% was with "Sebastian" and 53% was with "Kara"

Notes:
- Becasue results are appended, different words or texts can easily be compared.
- Case does not matter when comparing words
- Plurals are considered different words due to complications such as ending in "es" which is in sometime an entirely different word and sometimes a plural
- A word will be considered complete when any form of punctuation is met - ex. ', -
- Becuase this is analyzing sentences, the last sentence must end in a period to be considered a sentence, as is usually the case for sentences