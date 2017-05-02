#!python

import sys

# Usage:
# $ bin2cpp.py infile namespace1::namespace2::variablename

def write_header(contents):
	print "#pragma once\n"
	print "// $ {0} {1} {2}\n".format(sys.argv[0], sys.argv[1], sys.argv[2])
	for ns in contents[:-1]:
		print "namespace {0} {{ ".format(ns)
	print "\tstatic const char {0} = {{".format(contents[-1])

def write_footer(contents):
	print "\t};"
	for ns in contents[:-1]:
		print "}"

def format_char(c):
	return '0x{:02X}'.format(ord(c)) 

def main(): 
	if len(sys.argv[2]) >= 2: 
		naming = sys.argv[2].split('::')
		with open(sys.argv[1], "rb") as file: 
			write_header(naming)
			bytes = file.read()
			counter = 0
			num_bytes_per_row = 12
			work_str = ''
			for byte in bytes[:-1]:
				work_str += "{}, ".format(format_char(byte))
				counter = counter + 1
				if counter == num_bytes_per_row:
					counter = 0;
					print "\t\t", work_str
					work_str = ''
			work_str += "{}".format(format_char(bytes[-1]))
			print "\t\t", work_str

			write_footer(naming)

def test():
	lst = ['ns1', 'ns2', 'var']
	write_header(lst)
	write_footer(lst)

if __name__=='__main__':
	main()
	#test()
