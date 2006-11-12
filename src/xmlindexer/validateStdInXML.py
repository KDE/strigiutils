#! /usr/bin/python
import sys
from xml.sax import make_parser 

# this script reads from standard input and parses it as xml
# if the xml is invalid, it will print an error message

parser = make_parser()
parser.parse(sys.stdin)
