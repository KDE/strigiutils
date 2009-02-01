#! /usr/bin/python
import sys
import time
from xml.sax import make_parser, handler, SAXException 

class UriLogger(handler.ContentHandler):

    def __init__(self):
        self.count = 0
        self.start = time.time()

    def startElement(self, name, attrs):
        if attrs.has_key('uri'):
            self.uri = attrs['uri']
            self.count += 1
            if (self.count % 1000 == 0):
                elapsed = time.time() - self.start
                elapsed = self.count/elapsed
                print '%9d %9d %s' % (self.count, elapsed, self.uri)

# this script reads from standard input and parses it as xml
# if the xml is invalid, it will print an error message

parser = make_parser()
urilogger = UriLogger()
parser.setContentHandler(urilogger)

try:
   parser.parse(sys.stdin)
except SAXException, e :
   if hasattr(urilogger, 'uri'):
     print "Error after "+urilogger.uri
   print e;
