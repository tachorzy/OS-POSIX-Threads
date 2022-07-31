Instructions:
Read input using input redirection
Calculate the number of bits of the codes based on the value of the code in the alphabet (base 10)
For base 10 numbers greater than 0 use formula:  
    
    ceiling(log_2(greatest_base_10_code_in_the_alphabet +1))

Create n child processes or threads (where n is the number of symbols in the alphabet).
Create m child processes or threads (where m is the number of characters in the decompressed message) to determine
each character of the decompressed message.
Print the info about the alphabet and the decompressed message