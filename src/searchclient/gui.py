import sys
import dbus
from qt import *

class StrigiResults(QListBox):

    def __init__(self, *args):
        apply(QListBox.__init__, (self,) + args)
        bus = dbus.SessionBus()
        # make a connection to the strigi daemon
        obj = bus.get_object('vandenoever.strigi', '/search')
        self.strigi = dbus.Interface(obj, 'vandenoever.strigi')
        #self.strigi.setIndexedDirectories("/home/oever/dbuspython")
        #self.strigi.startIndexing()

    def slotStartSearch(self, q):
        results = self.strigi.getHits(str(q), 10, 0)
        self.clear()
        for i in results:
            self.insertItem(i[0])

def main(args):
    app = QApplication(sys.argv)
    QObject.connect(app, SIGNAL('lastWindowClosed()'),
                    app, SLOT('quit()'))
    vbox = QVBox()
    text = QLineEdit(vbox)
    box = StrigiResults(vbox)
    QObject.connect(text, SIGNAL('textChanged(const QString &)'),
                    box.slotStartSearch)
    app.setMainWidget(vbox)
    vbox.show()
    app.exec_loop()

if __name__=="__main__":
    main(sys.argv)
