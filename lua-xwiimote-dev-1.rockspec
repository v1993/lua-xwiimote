package = "lua-xwiimote"
version = "dev-1"
source = {
   url = "*** please add URL for source tarball, zip or repository here ***"
}
description = {
   homepage = "*** please enter a project homepage ***",
   license = "MIT"
}
dependencies = {
   "lua >= 5.3, < 5.5"
}
external_dependencies = {
   XWIIMOTE = {
      library = "xwiimote";
      header = "xwiimote.h";
   }
}
build = {
   type = "builtin",
   modules = {
      xwiimote = {
         incdirs = {
            "$(XWIIMOTE_INCDIR)"
         },
         libdirs = {
            "$(XWIIMOTE_LIBDIR)"
         },
         libraries = {
            "xwiimote"
         },
         sources = {"src/monitor.c", "src/interface.c", "src/main.c", "src/event.c"}
      }
   }
}
