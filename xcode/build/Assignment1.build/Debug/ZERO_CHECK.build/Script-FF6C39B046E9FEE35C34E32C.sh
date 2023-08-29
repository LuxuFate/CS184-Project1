#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/Mattlu/Desktop/CS184/p1-rasterizer-sp23-susfx/xcode
  make -f /Users/Mattlu/Desktop/CS184/p1-rasterizer-sp23-susfx/xcode/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/Mattlu/Desktop/CS184/p1-rasterizer-sp23-susfx/xcode
  make -f /Users/Mattlu/Desktop/CS184/p1-rasterizer-sp23-susfx/xcode/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/Mattlu/Desktop/CS184/p1-rasterizer-sp23-susfx/xcode
  make -f /Users/Mattlu/Desktop/CS184/p1-rasterizer-sp23-susfx/xcode/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/Mattlu/Desktop/CS184/p1-rasterizer-sp23-susfx/xcode
  make -f /Users/Mattlu/Desktop/CS184/p1-rasterizer-sp23-susfx/xcode/CMakeScripts/ReRunCMake.make
fi

