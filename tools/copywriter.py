#!/usr/bin/env python

import re
import sys
from optparse import OptionParser

def main():
    parser = OptionParser(usage="usage: %prog [options] <notice_filename> <code_filename>")
    (options, args) = parser.parse_args()
    
    if len(args) != 2:
        parser.error('incorrect number of arguments')
    
    notice_filename = args[0]
    filename = args[1]
    
    notice_file = open(notice_filename)
    notice = notice_file.read()
    notice_file.close()
    
    code_file = open(filename, 'r+')
    code = code_file.read()
    
    if notice in code[:len(notice*2)]:
        print 'Copyright notice already in file %s' % filename
        sys.exit(0)
    
    print 'Appending copyright notice to beginning of %s' % filename
    
    code_file.seek(0)
    
    split_first_line = code.split('\n', 1)
    first_line = split_first_line[0]
    if first_line[:2] == '#!':
        print 'Shebang found, preserving'
        code = split_first_line[1]
        code_file.write(first_line+'\n')
    code_file.write(notice)
    # if the first line was not whitespace, add a padding line
    if (re.search("\S", first_line)):
        code_file.write("\n")
    code_file.write(code)
    code_file.close()

    sys.exit(0)
if __name__ == "__main__":
    main()
