#! /usr/bin/perl -w
use strict;
use File::Basename;

die "Usage: $0 [interfacename] [headerfile.h]\n"
    unless @ARGV == 2 || @ARGV == 3;

# global variables
my $interfacename = $ARGV[0];
my $interfaceheader = $ARGV[1];
my $extrainclude;
if ($ARGV[2]) {
    $extrainclude = $ARGV[2];
}

die "File $interfaceheader is not readable.\n" unless -r $interfaceheader;

my $class;
my %functionsIn;
my %functionsOut;
my %responses;
my %signals;
my $classname;
my $constructorargs = "";

open FILE, $interfaceheader or die "Can't open file: $!\n";
my @lines = <FILE>;
close FILE;

my %typemapping = (
	"void" => "ignore",
	"void*" => "ignore",
	"char*" => "ignore",
	"std::string" => "s",
	"int32_t" => "i",
	"uint32_t" => "u",
	"bool" => "b",
	"std::vector<std::string>" => "as",
	"std::set<std::string>" => "as",
	"Hits" => "a(sdsssxxa{sas})",
	"std::map<std::string, std::string>" => "a{ss}",
	"std::vector<char>" => "ay",
	"std::multimap<int, std::string>" => "a(is)",
	"std::vector<std::pair<bool,std::string> >" => "a(bs)",
	"std::vector<std::pair<std::string, uint32_t> >" => "a(su)",
	"uint64_t" => "t",
	"Strigi::Variant" => "v",
	"std::vector<std::vector<Strigi::Variant> >" => "aav",
	"std::vector<int32_t>" => "ai",
	"std::vector<uint32_t>" => "au"
);

sub splitArguments {
    my $s = shift;
    my @args = ();
    my $i = 0;
    my $b = 0;
    while ($i < length($s)) {
        my $c = substr($s, $i, 1);
        if ($c eq ',' && $b eq 0) {
            my $a = substr($s, 0, $i);
            push(@args, $a);
            $s = substr($s, $i+1);
            $i = -1;
        } elsif ($c eq '<') {
            $b++;
        } elsif ($c eq '>') {
            $b--;
        }
        $i++;
    }
    if (length($s)) {
        push(@args, $s);
    }
    return @args;
}
sub parseArguments {
    my $in = shift;
    my @args = splitArguments($in);
    my @a;
    foreach (@args) {
        if (m/^\s*(.*)$/) {
            $_ = $1;
        }
        if (m/^const\s+(.*)$/) {
            $_ = $1;
        }
        my $atype;
        my $aname;
        if (m/^(.+)\&?\s+\&?(\w+)$/) {
            $atype = $1;
            $aname = $2;
        } else {
            die "PARSE ERROR: '$_'";
        }
        $atype =~ s/\&?\s*\&?$//;
        die "Type $atype cannot be mapped." unless $typemapping{$atype};
        push(@a, $atype, $aname);
    }
    return @a;
}
sub printFunctionDefinition {
    my $name = shift;
    print FH "void\n";
    print FH "Private$classname\::$name(DBusMessage* msg, DBusConnection* conn) {\n";
    print FH "    DBusMessageWriter writer(conn, msg);\n";
    print FH "    try {\n";
    print FH "        DBusMessageReader reader(msg);\n";
    my $i = $functionsIn{$name};
    my @a = parseArguments($i);
    for ($i=1; $i < @a; $i+=2) {
        print FH "        ".$a[$i-1]." ".$a[$i].";\n";
    }
    if (@a) {
        print FH "        reader";
        for ($i=1; $i < @a; $i+=2) {
	    print FH " >> ".$a[$i];
        }
        print FH ";\n";
    }
    print FH "        if (!reader.isOk()) {\n";
    print FH "            writer.setError(\"Invalid input.\");\n";
    print FH "        } else if (!reader.atEnd()) {\n";
    print FH "            writer.setError(\"Too many arguments.\");\n";
    print FH "        } else {\n            ";
    if (length($functionsOut{$name}) > 0 && $functionsOut{$name} ne "void") {
        print FH "writer << ";
    }
    print FH "impl.$name(";
    for ($i=1; $i < @a; $i+=2) {
        print FH $a[$i];
        if ($i < @a-2) {
            print FH ",";
        }
    }
    print FH ");\n";
    print FH "        }\n";
    print FH "    } catch (const std::exception& e) {\n";
    print FH "        writer.setError(e.what());\n";
    print FH "    } catch (...) {\n";
    print FH "        writer.setError(\"\");\n";
    print FH "    }\n";
    print FH "}\n";
}
sub printASyncFunctionDefinition {
    my $name = shift;
    print FH "void\n";
    print FH "Private$classname\::$name(DBusMessage* dbm, DBusConnection* conn) {\n";
    print FH "    DBusMessageReader reader(dbm);\n";
    my $i = $functionsIn{$name};
    my @a = parseArguments($i);
    for ($i=3; $i < @a; $i+=2) {
        print FH "    ".$a[$i-1]." ".$a[$i].";\n";
    }
    if (@a) {
        print FH "    reader";
        for ($i=3; $i < @a; $i+=2) {
	    print FH " >> ".$a[$i];
        }
        print FH ";\n";
    }
    print FH "    if (!reader.isOk()) {\n";
    print FH "        DBusMessageWriter writer(conn, dbm);\n";
    print FH "        writer.setError(\"Invalid input.\");\n";
    print FH "    } else if (!reader.atEnd()) {\n";
    print FH "        DBusMessageWriter writer(conn, dbm);\n";
    print FH "        writer.setError(\"Too many arguments.\");\n";
    print FH "    } else {\n";
    print FH "        dbus_message_ref(dbm);\n";
    print FH "        try {\n            ";
    print FH "impl.$name(dbm, ";
    for ($i=3; $i < @a; $i+=2) {
        print FH $a[$i];
        if ($i < @a-2) {
            print FH ",";
        }
    }
    print FH ");\n        } catch (const std::exception& e) {\n";
    print FH "            DBusMessageWriter writer(conn, dbm);\n";
    print FH "            writer.setError(e.what());\n";
    print FH "            dbus_message_unref(dbm);\n";
    print FH "        }\n";
    print FH "    }\n";
    print FH "}\n";
}
sub printIntrospectionXML {
    my $name = shift;
    my $i = $functionsIn{$name};
    my @a = parseArguments($i);
    print FH "    << \"    <method name='$name'>\\n\"\n";
    for ($i=1; $i < @a; $i+=2) {
        my $type = $a[$i-1];
        $type =~ s/^\s*const\s*//;
        $type = $typemapping{$a[$i-1]};
        print FH "    << \"      <arg name='$a[$i]' type='$type' direction='in'/>\\n\"\n";
    }
    my $type = $functionsOut{$name};
    $type =~ s/^\s*const\s*//;
    if (length($type) > 0 && $type ne "void") {
        my $outname = "out";
        # try to find the name of the output variable
        if ($type =~ m#\s*/\*\s*(\S+)\s*\*/\s*#) {
            $outname = $1;
            $type =~ s#\s*/\*\s*\S+\s*\*/\s*##g;
        }
        $type = $typemapping{$type};
        print FH"    << \"      <arg name='$outname' type='$type' direction='out'/>\\n\"\n";
    }
    print FH "    << \"    </method>\\n\"\n";
}
sub printASyncIntrospectionXML {
    my $name = shift;
    my $i = $functionsIn{$name};
    my @a = parseArguments($i);
    print FH "    << \"    <method name='$name'>\\n\"\n";
    for ($i=3; $i < @a; $i+=2) {
        my $type = $typemapping{$a[$i-1]};
        print FH "    << \"      <arg name='$a[$i]' type='$type' direction='in'/>\\n\"\n";
    }
    $i = $responses{$name."Response"};
    @a = parseArguments($i);
    die "Not enough arguments for ${name}Response." unless @a > 3;
    for ($i=5; $i < @a; $i+=2) {
        my $type = $typemapping{$a[$i-1]};
        print FH "    << \"      <arg name='$a[$i]' type='$type' direction='out'/>\\n\"\n";
    }
    print FH "    << \"    </method>\\n\"\n";
}
sub printSignalIntrospectionXML {
    my $name = shift;
    my $i = $signals{$name};
    my @a = parseArguments($i);
    print FH "    << \"    <signal name='$name'>\\n\"\n";
    for ($i=1; $i < @a; $i+=2) {
        my $type = $typemapping{$a[$i-1]};
        print FH "    << \"      <arg name='$a[$i]' type='$type'/>\\n\"\n";
    }
    print FH "    << \"    </signal>\\n\"\n";
}

# find the classname
foreach (@lines) {
    if (m/^class\s+(\w+)/) {
        $class = $1;
    }
}
die "No class found." unless defined $class;

# parse the functions from the header file
my @codelines;
my $line = "";
foreach (@lines) {
    chomp;
    s/\/\/.*//; # remove inline comments
    $line .= " $_";
    if ($line =~ m/[;}{]/) {
        push @codelines, $line;
        $line = "";
    }
}
foreach (@codelines) {
    # match function line
    if (m/^\s*             # leading whitespace
            (virtual\s+)?  # 1: optional 'virtual' keyword
            (\S?.*)\s+       # 2: output argument
            ~?(\w+)        # 3: function name
            \(\s*(.*)\s*\) # 4: function arguments
            (\s*=\s*0)?    # 5: pure virtual syntax
            /x) {
        my $out = $2;
        my $name = $3;
        my $in = $4;
        my $null = $5;
        # skip constructor, destructor and static functions
        if ($name eq $class) {
            if (defined $in) {
                $in =~ s/^\s*//;   # remove leading space
                $in =~ s/\s*$//;   # remove trailing space
                $in =~ s/\).*//;   # remove chars after ')'
                $in =~ s/=[^,]*//; # remove default arguments
                if (length($in)) {
                    $constructorargs = $in;
                    #print "constructor $name '$in'\n";
                }
            }
            next;
        } elsif ($out =~ m/static/) {
            next;
        }
        if ($out eq "void" && defined $null) {
            if ($name =~ m/Response$/) {
                # response
                $responses{$name} = $in;
            } else {
                # signal
                $signals{$name} = $in;
            }
        } else {
            $functionsIn{$name} = $in;
            $functionsOut{$name} = $out;
            die "Function $name in $interfaceheader cannot be abstract."
                unless !defined $5;
        }
    }
}
# check that all responses match
foreach my $response (keys %responses) {
    my $function = $response;
    $function =~ s/Response$//;
    my $out = $functionsOut{$function};
    die "Function $function is not ok." unless $out eq "void";
}
# print the inherited class
$classname = "DBus$class";
my $headerfile = lc($classname).".h";
my $cppfile = lc($classname).".cpp";
my $relativeinterfaceheader = $interfaceheader;
$relativeinterfaceheader =~ s#.*/src/daemon/##;

open (FH, "> $headerfile") or die;
print FH "// generated by makecode.pl\n";
print FH "#ifndef ".uc($classname)."_H\n";
print FH "#define ".uc($classname)."_H\n";
print FH "#include \"$relativeinterfaceheader\"\n";
print FH "#define DBUS_API_SUBJECT_TO_CHANGE 1\n";
print FH "#include <dbus/dbus.h>\n";
print FH "#include <exception>\n";
print FH "#ifdef  __APPLE__ \n";
print FH "#define uint64_t dbus_uint64_t\n";
print FH "#endif\n";
print FH "class DBusObjectInterface;\n";
print FH "class $classname : public $class {\n";
print FH "private:\n";
print FH "    std::string object;\n";
print FH "    DBusConnection* const conn;\n";
print FH "    DBusObjectInterface* const iface;\n";
foreach (keys %responses) {
    print FH "    void $_(".$responses{$_}.");\n";
}
foreach (keys %signals) {
    print FH "    void $_(".$signals{$_}.");\n";
}
print FH "public:\n";
print FH "    $classname(const std::string& objectname, DBusConnection* c";
if (length($constructorargs)) {
    print FH ", $constructorargs";
}
print FH ");\n";
print FH "    ~$classname();\n";
print FH "    DBusObjectInterface* interface() { return iface; }\n";
print FH "};\n";
print FH "#endif\n";
close(FH);

open (FH, "> $cppfile") or die;
print FH "// generated by makecode.pl\n";
print FH "#include \"$headerfile\"\n";
if (defined $extrainclude && length($extrainclude)) {
    print FH "#include \"$extrainclude\"\n";
}
print FH "#include \"dbus/dbuscpp/dbusobjectinterface.h\"\n";
print FH "#include \"dbus/dbuscpp/dbusmessagereader.h\"\n";
print FH "#include \"dbus/dbuscpp/dbusmessagewriter.h\"\n";
print FH "#include <sstream>\n";
print FH "class Private$classname : public DBusObjectInterface {\n";
print FH "private:\n";
print FH "    $classname& impl;\n";
print FH "    typedef void (Private$classname\::*handlerFunction)\n";
print FH "        (DBusMessage* msg, DBusConnection* conn);\n";
print FH "    std::map<std::string, handlerFunction> handlers;\n";
print FH "    DBusHandlerResult handleCall(DBusConnection*c,DBusMessage* m);\n";
print FH "    std::string getIntrospectionXML();\n";
foreach (keys %functionsIn) {
    print FH "    void $_(DBusMessage* msg, DBusConnection* conn);\n";
}
print FH "public:\n";
print FH "    Private$classname($classname& i);\n";
print FH "};\n";
print FH "Private$classname\::Private$classname($classname& i)\n";
print FH "        :DBusObjectInterface(\"$interfacename\"), impl(i) {\n";
foreach (keys %functionsIn) {
    print FH "    handlers[\"$_\"] = &Private$classname\::".$_.";\n";
}
print FH "}\n";
print FH "DBusHandlerResult\n";
print FH "Private$classname\::handleCall(DBusConnection*connection, DBusMessage* msg) {\n";
print FH <<THEEND;
    std::map<std::string, handlerFunction>::const_iterator h;
    const char* i = getInterfaceName().c_str();
    for (h = handlers.begin(); h != handlers.end(); ++h) {
        if (dbus_message_is_method_call(msg, i, h->first.c_str())) {
            (this->*h->second)(msg, connection);
            return DBUS_HANDLER_RESULT_HANDLED;
        }
    }
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
THEEND
print FH "std::string\n";
print FH "Private$classname\::getIntrospectionXML() {\n";
print FH "    std::ostringstream xml;\n";
print FH "    xml << \"  <interface name='\"+getInterfaceName()+\"'>\\n\"\n";
foreach (keys %functionsIn) {
    if (defined $responses{$_."Response"}) {
        printASyncIntrospectionXML($_);
    } else {
        printIntrospectionXML($_);
    }
}
foreach (keys %signals) {
    printSignalIntrospectionXML($_);
}
print FH "    << \"  </interface>\\n\";\n";
print FH "    return xml.str();\n";
print FH "}\n";
foreach (keys %functionsIn) {
    if (defined $responses{$_."Response"}) {
        printASyncFunctionDefinition($_);
    } else {
        printFunctionDefinition($_);
    }
}
print FH "$classname\::$classname(const std::string& on, DBusConnection* c";
if (length($constructorargs)) {
    print FH ", $constructorargs) \n";
    print FH "        :$class(";
    my @args = splitArguments($constructorargs);
    for (my $i=0; $i<@args; ++$i) {
        if ($i != 0) { print FH ","; }
        my $arg = $args[$i];
        $arg =~ s/.*\W(\w+)\s*$/$1/;
        print FH $arg;
    }
    print FH "), ";
} else {
    print FH ")\n        :";
}
print FH "object(on), conn(c), iface(new Private$classname(*this)) {}\n";
print FH "$classname\::~$classname() {\n";
print FH "    delete iface;\n";
print FH "}\n";
foreach (keys %responses) {
    print FH "void\n";
    print FH "$classname\::$_(".$responses{$_}.") {\n";
    print FH "    DBusMessage* m = static_cast<DBusMessage*>(msg);\n";
    print FH "    DBusMessageWriter writer(conn, m);\n";
    print FH "    writer";
    my @args = parseArguments($responses{$_});
    for (my $i=5; $i <= $#args; $i += 2) {
        print FH " << ".$args[$i];
    }
    print FH ";\n";
    print FH "    dbus_message_unref(m);\n";
    print FH "}\n";
}
foreach (keys %signals) {
    print FH "void\n";
    print FH "$classname\::$_(".$signals{$_}.") {\n";
    print FH "    DBusMessageWriter writer(conn, object.c_str(), \"$interfacename\", \"$_\");\n";
    print FH "    writer";
    my @args = parseArguments($signals{$_});
    for (my $i=1; $i <= $#args; $i += 2) {
        print FH " << ".$args[$i];
    }
    print FH ";\n";
    print FH "}\n";
}
close (FH);
