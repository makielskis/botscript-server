#!/usr/bin/python

import subprocess
import sys
import os, os.path
import gzip
import sha
from os.path import basename


#####################
# UTILITY FUNCTIONS #
#####################

# Writes zipped file from the given file
# -> gzipped file: filaname + ".gz"
def gzip_file(filename):
    print("gzip " + filename)
    f_in = open(filename, 'rb')
    f_out = gzip.open(filename + ".gz", 'wb')
    f_out.writelines(f_in)
    f_out.close()
    f_in.close()
    return filename + ".gz"

# Writes the filecontents to the specified output
# -> count of bytes read
def write_bytes(output, filename):
    byte_count = 0
    with open(filename, "rb") as f:
        count = 0
        byte = f.read(1)
        while byte != "":
            if count == 0: output.write("    ")
            output.write("(char) 0x" + byte.encode('hex'))
            byte_count += 1
            byte = f.read(1)
            if byte != "":
                output.write(", ")
            if count > 10:
                count = 0
                output.write("\n")
            else:
                count += 1
    return byte_count

def write_header(output, directory, gzip):
    print("write_header(" + directory + ")")

    for dirname, dirnames, filenames in os.walk(directory):
        # SUB DIRECTORY: RECURSION RECURSION RECURSION ...
        for subdirname in dirnames:
            if subdirname.startswith("."):
                dirnames.remove(subdirname)

        # WRITE ALL FILES
        for name in filenames:
            name = os.path.join(dirname, name)
            content_path = "/" + name[len(directory) :]

            # SKIP HIDDEN FILES
            if name.startswith("."):
                continue

            print("writing: " + name)

            # APPLY GZIP
            if use_gzip:
              name = gzip_file(name)

            # WRITE CHAR ARRAY CONTENT
            varname = sha.new(content_path).hexdigest()
            output.write("  // " + content_path + "\n")
            output.write("  const char _" + varname + "[] = {\n")
            byte_count = write_bytes(output, name)
            output.write("\n  };\n")
            output.write("  modules[\"" + varname + "\"] = std::string(_" + varname + ", " + str(byte_count) + ");\n\n")

            # CLEANUP GZIP OUTPUT
            if use_gzip:
                os.remove(name)

########################
# CHECK ARGUMENT COUNT #
########################
if len(sys.argv) != 2 and len(sys.argv) != 3:
  print("usage: " + sys.argv[0] + " input_folder [G]")
  print("       G - Use gzip compression")
  print(len(sys.argv))
  exit(1)

################
# SET DEFAULTS #
################
use_gzip        = False

#####################
# READ FLAGS IF SET #
#####################
if len(sys.argv) >= 3:
  for c in sys.argv[2]:
    if c == 'G':
      use_gzip = True
    else:
      print("unknown argument " + c)
      exit(1)

###################
# BASIC VARIABLES #
###################
extension = '.h'
folder = sys.argv[1] + "/"
output = open("contents.cpp", "w")

######################
# WRITE LIBRARY CODE #
######################

# INCLUDES
output.write("\
#include \"./contents.h\"\n\
\n\
#include <string>\n\
#include <map>\n\
\n\
namespace contents {\n\
\n")

# FUNCTION DEFINITION
output.write("std::map<std::string, std::string> load() {\n\
  std::map<std::string, std::string> modules;\n\n")

# CALL RECURSIVE DIRECTORY ITERATOR
print("lets go! " + folder)
write_header(output, folder, gzip)

# FINISH FUNCTION
output.write("\
  return modules;\n\
}\n\
\n\
}  // namespace contents")

# CLOSE STREAM
output.close()
