ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = foreign

SUBDIRS = doc src

EXTRA_DIST = win \
src/ini/AUTHORS \
src/ini/COPYING \
src/ini/README

AM_CPPFLAGS = -DHAVE_UNIX -I $(top_builddir)

# Remove svn directories from any subdirs that get
# included completely (e.g. win, directx,...).

dist-hook:
	@rm -rf `find $(distdir) -path \*/.svn`;
