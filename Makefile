#
# Makefile for blobs library for OpenCV Version 4 and its examples
#
# Modifications to the Makefile by Carl J. Nobile
#
AR      	= ar rcs # ru
CXX		= g++

MAJORVERSION	= 8
MINORVERSION	= 3
PATCHLEVEL	= 0
BASEDIR		= $(shell pwd)
PACKAGE_DIR     = $(shell echo $${PWD\#\#*/})
VERSION		= ${MAJORVERSION}.${MINORVERSION}.${PATCHLEVEL}
DISTNAME	= ${PACKAGE_DIR}-$(VERSION)

PREFIX		= /usr/local
LIBDIR		= $(PREFIX)/lib
INCDIR		= $(PREFIX)/include
TESTDIR		= testBlobs

CFLAGS		= `pkg-config --cflags opencv gtk+-2.0`
LDFLAGS		= `pkg-config --libs opencv gtk+-2.0`
INCFLAGS	=  -I.
LIBFLAGS	=  -L. -lblob


SHARED		= -fPIC
OPTIONS		= -O3 -ansi -pipe -fstrength-reduce -finline-functions -Wall \
                  -Wno-unused-result

OBJS1		= Blob.o BlobContour.o BlobOperators.o BlobResult.o \
                  ComponentLabeling.o

HEADERS		= BlobContour.h Blob.h BlobLibraryConfiguration.h \
                  BlobOperators.h BlobResult.h ComponentLabeling.h counted_ptr.h

CFLAGS		+= $(SHARED) $(OPTIONS)

.SUFFIXES: .cpp.o
.cpp.o:	; echo 'Compiling $*.cpp' ; $(CXX) $(CFLAGS) -c $*.cpp

.SILENT:

all	:
	make shared
	make static

# Target to build shared objects.
shared	: $(OBJS1)
	$(CXX) -shared -Wl,-soname,libblob.so.$(MAJORVERSION) \
         -o libblob.so.$(VERSION) $(OBJS1)
	-ln -sf libblob.so.$(VERSION) libblob.so.$(MAJORVERSION)
	-ln -sf libblob.so.$(VERSION) libblob.so

# Target to build the static library.
static	: $(OBJS1)
	$(AR) libblob.a $(OBJS1)
	ranlib libblob.a

# Target to build the test program.
test	: shared
	@echo Build tests, examples and tools...
	$(CXX) -g $(TESTDIR)/main.cpp $(LDFLAGS) $(CFLAGS) $(INCFLAGS) \
         $(LIBFLAGS) -o $(TESTDIR)/blobdemo

# Header file dependencies. This forces a recompilation if headers change.
$(OBJS1): $(HEADERS)

# Target to run the test program without having to install the libraries.
runtest	: test
	(export LD_LIBRARY_PATH=`pwd`:$(LD_LIBRARY_PATH); \
         cd testBlobs; ./blobdemo)

# Target to install all libraries. (Must be run as root)
install	: all
	install -m0644 -D libblob.so.$(VERSION) $(LIBDIR)
	install -m0644 -D BlobResult.h $(INCDIR)
	-(cd $(LIBDIR); ln -fs libblob.so.$(VERSION) libblob.so.$(MAJORVERSION))
	-(cd $(LIBDIR); ln -fs libblob.so.$(VERSION) libblob.so)
	/sbin/ldconfig
	install -m0644 libblob.a $(LIBDIR)

# Target to uninstall all library and header. (Must be run as root)
uninstall:
	(cd $(LIBDIR); rm -f libblob.*)
	(cd $(INCDIR); rm -f BlobResult.h)

# Target to package both tar, zip and md5sum files.
package	: tarball zip

# Target to create the distribution tarball and md5 checksum file.
tarball	: clean
	@(cd ..; tar -czvf $(DISTNAME).tar.gz --exclude .svn $(PACKAGE_DIR); \
         md5sum $(DISTNAME).tar.gz > $(DISTNAME).md5)

# Target to create the distribution zip and md5 checksum file.
zip	: clean
	@(cd ..; zip -r $(DISTNAME).zip $(PACKAGE_DIR) -x /*/.svn/*; \
        md5sum $(DISTNAME).zip >> $(DISTNAME).md5)

# Target to clean the project.
clean	:
	@echo Cleaning...
	@rm -f *.o *~ \#*\#
	@rm -f libblob.*
	@(cd $(TESTDIR); rm -f *.o *~ \#*\# blobdemo)
