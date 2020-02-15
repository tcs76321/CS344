# Trevor Stahl
# stahltr
# 2/15/2020
# CS 344 400 W2020
# program py - python exploration
# used lines from pynative.com/python-generate-random-string/
# ** URL typed by hand because on school server **
# and used some code from this class too I think, atleast the assignment page

import string
import random
import sys

fileNames = ["Uno.txt", "Dos.txt", "Tres.txt"]

for fileName in fileNames:
	file_ = open(fileName, "w") # truncates
	stuff = ''.join(random.choice(string.ascii_lowercase) for i in range(10))
	file_.write(stuff)
	file_.close()
	file_ = open(fileName, "a") # append
	file_.write('\n')
	file_.close()
	file_ = open(fileName, "r") # read
	sys.stdout.write(file_.read())

AAA = random.randint(1, 42) # randint is inclusive
BBB = random.randint(1, 42)
CCC = AAA * BBB
print(AAA)
print(BBB)
print(CCC)
