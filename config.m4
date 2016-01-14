dnl $Id$
dnl config.m4 for extension cg_bcode

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(cg_bcode, for cg_bcode support,
dnl Make sure that the comment is aligned:
dnl [  --with-cg_bcode             Include cg_bcode support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(cg_bcode, whether to enable cg_bcode support,
dnl Make sure that the comment is aligned:
dnl [  --enable-cg_bcode           Enable cg_bcode support])

if test "$PHP_CG_BCODE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-cg_bcode -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/cg_bcode.h"  # you most likely want to change this
  dnl if test -r $PHP_CG_BCODE/$SEARCH_FOR; then # path given as parameter
  dnl   CG_BCODE_DIR=$PHP_CG_BCODE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for cg_bcode files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       CG_BCODE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$CG_BCODE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the cg_bcode distribution])
  dnl fi

  dnl # --with-cg_bcode -> add include path
  dnl PHP_ADD_INCLUDE($CG_BCODE_DIR/include)

  dnl # --with-cg_bcode -> check for lib and symbol presence
  dnl LIBNAME=cg_bcode # you may want to change this
  dnl LIBSYMBOL=cg_bcode # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CG_BCODE_DIR/$PHP_LIBDIR, CG_BCODE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_CG_BCODELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong cg_bcode lib version or lib not found])
  dnl ],[
  dnl   -L$CG_BCODE_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(CG_BCODE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(cg_bcode, cg_bcode.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
