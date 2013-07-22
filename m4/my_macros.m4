AC_DEFUN([MY_CHECK_IOS_BIN],
[
    AC_MSG_CHECKING([whether standard member ios::binary is available])
    AC_CACHE_VAL(test_cv_have_ios_binary,
    [
        AC_TRY_COMPILE(
            [#include <fstream>],
            [std::ifstream myTest(std::ios::in|std::ios::binary);],
            [test_cv_have_ios_binary=yes],
            [test_cv_have_ios_binary=no]
        )
    ])
    AC_MSG_RESULT($test_cv_have_ios_binary)
    if test "$test_cv_have_ios_binary" = yes; then
        AC_DEFINE(HAVE_IOS_BIN,,
            [Define if standard member ``ios::binary'' is called ``ios::bin''.]
        )
    fi
])

dnl -------------------------------------------------------------------------
dnl Check whether C++ library has member ios::bin instead of ios::binary.
dnl Will substitute @HAVE_IOS_OPENMODE@ with either a def or undef line.
dnl -------------------------------------------------------------------------

AC_DEFUN([MY_CHECK_IOS_OPENMODE],
[
    AC_MSG_CHECKING([whether standard member ios::openmode is available])
    AC_CACHE_VAL(test_cv_have_ios_openmode,
    [
        AC_TRY_COMPILE(
            [#include <fstream>
             #include <iomanip>],
            [std::ios::openmode myTest = std::ios::in;],
            [test_cv_have_ios_openmode=yes],
            [test_cv_have_ios_openmode=no]
        )
    ])
    AC_MSG_RESULT($test_cv_have_ios_openmode)
    if test "$test_cv_have_ios_openmode" = yes; then
        AC_DEFINE(HAVE_IOS_OPENMODE,,
            [Define if ``ios::openmode'' is supported.]
        )
    fi
])
