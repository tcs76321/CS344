# Trevor Stahl
# stahltr
# 11/11/2019
# CS 344 400 F2019
# program py - python exploration
# used lines from pynative.com/python-generate-random-string/
# and from bytes.com/topic/answers/673613-how-insert-new-line-end-file-logfile
# ** URLS typed by hand because on school server **
# and used some code from this class too I think, atleast the assignment page

import string
import random
import sys

fileNames = ["Uno.txt", "Dos.txt", "Tres.txt"]

for x in fileNames:
	f = open(x, "w")
	stuff = ''.join(random.choice(string.ascii_lowercase) for i in range(10))
	f.write(stuff)
	f.close()
	f =  open(x, "a")
	f.write('\n')
	f.close()
	f = open(x, "r")
	sys.stdout.write(f.read())

AAA = random.randint(1, 42)
BBB = random.randint(1, 42)
CCC = AAA * BBB
print(AAA)
print(BBB)
print(CCC)
