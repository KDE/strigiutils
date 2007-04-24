#! /usr/bin/python
import sys
from xml.sax import make_parser, handler, SAXException 

class UriLogger(handler.ContentHandler):
    def startElement(self, name, attrs):
        if attrs.has_key('uri'):
            self.uri = attrs['uri']

# this script reads from standard input and parses it as xml
# if the xml is invalid, it will print an error message

parser = make_parser()
urilogger = UriLogger()
parser.setContentHandler(urilogger)

try:
   parser.parse(sys.stdin)
except SAXException, e :
   print "Error after "+urilogger.uri
   print e;
