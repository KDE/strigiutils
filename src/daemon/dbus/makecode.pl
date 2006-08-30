#! /usr/bin/perl -w
use strict;

# global variables
my $interface = "../clientinterface.h";

my $class;

sub printDbusFunction {
    my $out = shift;
    my $name = shift;
    my $in = shift;
    my @args = split ',', $in;
    print "void\n";
    print "DBus$class"."::$name(DBusMessage* msg, DBusConnection* conn) {\n";
    print "    DBusMessageReader reader(msg);\n";
    print "    DBusMessageWriter writer(conn, msg);\n";
    my @a;
    foreach (@args) {
        if (m/^\s*(.*)$/) {
            $_ = $1;
        }
        if (m/^const\s+(.*)$/) {
            $_ = $1;
        }
        my $type;
        my $name;
        if (m/^(.+)\&?\s+\&?(\w+)$/) {
            $type = $1;
            $name = $2;
        } else {
            die "PARSE ERROR: '$_'";
        }
        $type =~ s/\&?\s*\&?$//;
        push(@a, $type, $name);
        print "    $type $name;\n";
    }
    my $i;
    if (@a) {
        print "    reader";
        for ($i=1; $i < @a; $i+=2) {
	    print " >> ".$a[$i];
        }
        print ";\n";
    }
    print "    if (reader.isOk()) {\n        ";
    if (length($out) > 0) {
        print "writer << ";
    }
    print "impl->$name(";
    for ($i=1; $i < @a; $i+=2) {
        print $a[$i];
        if ($i < @a-2) {
            print ",";
        }
    }
    print ");\n    }\n";
    print "}\n";
}


my @lines = `cat $interface`;

foreach (@lines) {
    if (m/^class\s+(\w+)/) {
        $class = $1;
    }
}
foreach (@lines) {
    # match function line
    if (m/^\s*(virtual\s+)?(.*)\s+~?(\w+)\(\s*(.*)\s*\)/) {
        if ($3 eq $class) {
            next;
        }
        printDbusFunction($2, $3, $4);
    }
}
