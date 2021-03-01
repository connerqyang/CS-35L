#!/usr/bin/python

"""
Shuffles input by outputting a random permutation of its input lines.
Each output permutation is equally likely.

Copyright 2005, 2007 Paul Eggert.
Copyright 2010 Darrell Benjamin Carbajal.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Please see <http://www.gnu.org/licenses/> for a copy of the license.

$Id: randline.py,v 1.4 2010/04/05 20:04:43 eggert Exp $
"""

import random, sys
from optparse import OptionParser

class Shuf:
    def __init__(self, input_lines, count, should_repeat):
        self.index = 0;
        random.shuffle(input_lines)
        self.count = count
        self.should_repeat = should_repeat
        self.input_lines = input_lines
    def __iter__(self):
        return self
    def __next__(self):
        if self.index == len(self.input_lines) or self.count == 0:
            raise StopIteration
        #Random sampling with permutation for -n and -r options both being used
        if self.should_repeat and self.count == -3696:
            return random.choice(self.input_lines)
        elif self.should_repeat and self.count >= 0:
            self.count = self.count - 1
            return random.choice(self.input_lines)
        else:
            self.count = self.count - 1
            self.index = self.index + 1
            return self.input_lines[self.index - 1]
        
def main():
    version_msg = "%prog 2.0"
    usage_msg = """%prog [OPTION]... FILE

Shuffles input by outputting a random permutation of its input lines.
Each output permutation is equally likely """

    #First, parse through the options
    parser = OptionParser(version=version_msg,
                          usage=usage_msg)
    parser.add_option("-i", "--input-range",
                      action="store", type="string", dest="input_range",
                      default=None,
                      help="act as if input came from a file containing range lo...hi of unsigned decimal integers, one per line.")
    parser.add_option("-n", "--head-count", type="int",
                      action="store", dest="head_count", default = -3696,
                      help="output at most head_count lines")
    parser.add_option("-r", "--repeat", action="store_true",
                      dest="repeat", default=False,
                      help="each output line randomly chosen out of inputs (selection with replacement)")

    options, args = parser.parse_args(sys.argv[1:])

    #Check the syntax of the options
    if options.input_range is not None:
        #The count cannot be negative. -3696 is the default value.
        if options.head_count < 0 and options.head_count != -3696:
            parser.error("The head_count cannot be negative")
        #There should be no input file when using the -i option
        if len(args) > 0:
            parser.error("There is an extra positional argument '%s'" %args[0])
        try:
            lo_str, hi_str = (options.input_range).split("-")
            lo = int(lo_str)
            hi = int(hi_str)
        #Handles error checking statement    
        except ValueError:
            parser.error("Invalid lo-hi range: %s" %options.input_range)
            return
        #The hi value cannot be lesser than the lo value
        if hi < lo:
            parser.error("Invalid lo-hi range: %s" %options.input_range)
        #Creation of the range input file
        input_lines = [str(x) for x in range(lo, hi + 1)]
    #If there is at least one positional argument
    elif len(args) > 0:
        if len(args) > 1:
            parser.error("There is an extra positional argument '%s'" %args[1])
            return
        if args[0] == '-':
            input_lines = sys.stdin.readlines()
        else:
            file = open(args[0])
            input_lines = file.read().splitlines()
            file.close()
    #If no positional arguments, also read from stdin 
    elif len(args) == 0:
        input_lines = sys.stdin.read().splitlines()
    
    shuf = Shuf(input_lines, options.head_count, options.repeat)
    for input_line in shuf:
        sys.stdout.write(input_line + "\n")

if __name__ == "__main__":              
    main()
