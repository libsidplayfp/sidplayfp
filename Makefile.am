
AUTOMAKE_OPTIONS = foreign

SUBDIRS = doc src

AM_CPPFLAGS = -I $(top_builddir)

# Remove svn directories from any subdirs that get
# included completely (e.g. win, directx,...).

dist-hook:
	@rm -rf `find $(distdir) -path \*/.svn`;
