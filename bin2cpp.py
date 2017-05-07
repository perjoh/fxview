#!python

import sys
import os

# Usage:
# $ bin2cpp.py infile namespace1::namespace2::variablename

def __write_header(contents, source_file, size):
	print "#pragma once\n"
	print "// source: {0}\n".format(os.path.normpath(source_file))
	for ns in contents[:-1]:
		print "namespace {0} {{ ".format(ns)
	print "\tstatic const char {0}[{1}] = {{".format(contents[-1], size)

def __write_footer(contents):
	print "\t};"
	for ns in contents[:-1]:
		print "}"

def __format_char(c):
	return '0x{:02X}'.format(ord(c)) 

def process(input_filename, var_name):
    with open(input_filename, "rb") as file: 
        bytes = file.read()
        filesize = len(bytes)
        if filesize > 0:
            naming = var_name.split('::')
            __write_header(naming, input_filename, filesize)
            counter = 0
            num_bytes_per_row = 12
            work_str = ''
            for byte in bytes[:-1]:
                work_str += "{}, ".format(__format_char(byte))
                counter = counter + 1
                if counter == num_bytes_per_row:
                    counter = 0;
                    print "\t\t", work_str
                    work_str = ''
            work_str += "{}".format(__format_char(bytes[-1]))
            print "\t\t", work_str

            __write_footer(naming)

def __main(): 
    process(sys.argv[1], sys.argv[2])

def __test():
	lst = ['ns1', 'ns2', 'var']
	__write_header(lst, 'dummy')
	__write_footer(lst)

if __name__=='__main__':
	__main()
