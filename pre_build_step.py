#!python 
import bin2cpp
import glob
import os
import sys

def strip_suffix(filename):
    return filename[:filename.rfind(".")]

def sourcify_shaders():
	# Include shaders in c++ code.
    filenames = glob.glob('src/graphics/shaders/glsl/*.glsl')
    for source_filename in filenames:
        directory = os.path.dirname(source_filename)
        bare_filename = os.path.basename(source_filename)
        filename_stripped = strip_suffix(bare_filename)
        target_filename = os.path.join(directory, "..", filename_stripped + '.hpp')
        with open(target_filename, 'w') as target_file: 
            stdout_prev = sys.stdout
            sys.stdout = target_file
            bin2cpp.process(source_filename, "graphics::shader::{0}".format(filename_stripped)) 
            sys.stdout = stdout_prev

if __name__=='__main__':
	sourcify_shaders() 
