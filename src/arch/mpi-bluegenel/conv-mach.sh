BGL_INC=/BlueLight/floor/bglsys/include
BGL_LIB=/BlueLight/floor/bglsys/lib
BGL_BIN=/BlueLight/floor/blrts-gnu/bin

BGL_CFLAGS="-I$BGL_INC -gdwarf-2 -Wno-deprecated"

CMK_CPP_CHARM="$BGL_BIN/powerpc-bgl-blrts-gnu-cpp -P"
CMK_CPP_C="$BGL_BIN/powerpc-bgl-blrts-gnu-cpp -E "
CMK_CC="$BGL_BIN/powerpc-bgl-blrts-gnu-gcc $BGL_CFLAGS "
CMK_CXX="$BGL_BIN/powerpc-bgl-blrts-gnu-g++ $BGL_CFLAGS "
CMK_CXXPP="$BGL_BIN/powerpc-bgl-blrts-gnu-g++ -E "
CMK_CF77="$BGL_BIN/powerpc-bgl-blrts-gnu-g77 "
CMK_CF90='f90'
CMK_RANLIB="$BGL_BIN/powerpc-bgl-blrts-gnu-ranlib "
CMK_AR="$BGL_BIN/powerpc-bgl-blrts-gnu-ar q "
CMK_LD="$CMK_CC -L$BGL_LIB "
CMK_LDXX="$CMK_CXX -L$BGL_LIB "
CMK_LIBS='-lckqt -lmpich.rts -lvdevices.rts -lcoprocessor.rts -lspad.rts -lrts.rts -ldevices.rts'
CMK_LD_LIBRARY_PATH="-Wl,-rpath,$CHARMLIBSO/"
CMK_SEQ_LIBS=''
CMK_SEQ_CC="$BGL_BIN/powerpc-bgl-blrts-gnu-gcc -Wno-deprecated "
CMK_SEQ_LD="$CMK_SEQ_CC"
CMK_SEQ_CXX="$BGL_BIN/powerpc-bgl-blrts-gnu-g++ -Wno-deprecated "
CMK_SEQ_LDXX="$CMK_SEQ_CXX"
CMK_NATIVE_CXX='g++ -Wno-deprecated '
CMK_NATIVE_LDXX='g++'
CMK_F90LIBS='-L/usr/absoft/lib -L/opt/absoft/lib -lf90math -lfio -lU77 -lf77math '
CMK_MOD_NAME_ALLCAPS=1
CMK_MOD_EXT="mod"
CMK_F90_USE_MODDIR=1
CMK_F90_MODINC="-p"
