#!/usr/bin/env python
import sys
import dbus
from qt import *

class StrigiResults(QListBox):

    def __init__(self, strigi, *args):
        apply(QListBox.__init__, (self,) + args)
        self.strigi = strigi

    def slotStartSearch(self, q):
        results = self.strigi.getHits(str(q), 10, 0)
        self.clear()
        for i in results:
            self.insertItem(i[0])

def getStrigi():
    # connect to DBus
    try:
        bus = dbus.SessionBus()
    except:
        print "DBus is not running."
    # make a connection to the strigi daemon
    try:
        obj = bus.get_object('vandenoever.strigi', '/search')
        strigi = dbus.Interface(obj, 'vandenoever.strigi')
        strigi.getStatus()
    except:
        strigi = None
        print "Strigi is not running."
    return strigi

def main(args):
    app = QApplication(sys.argv)
    
    strigi = getStrigi()
    if strigi == None:
        app.quit()
        return
    QObject.connect(app, SIGNAL('lastWindowClosed()'),
                    app, SLOT('quit()'))
    vbox = QVBox()
    text = QLineEdit(vbox)
    box = StrigiResults(strigi, vbox)
    QObject.connect(text, SIGNAL('textChanged(const QString &)'),
                    box.slotStartSearch)
    app.setMainWidget(vbox)
    vbox.show()
    app.exec_loop()

if __name__=="__main__":
    main(sys.argv)
