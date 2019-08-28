# lua-xwiimote
This is XWiimote bindings for Lua 5.3.

This library allow you to use Linux XWiimote stack in your Lua application. It binds all functional of the first. Visit our [wiki](wiki) for more details on usage.

# Notes

While this is completely standalone library, you may wish to use `poll` from [luaposix](https://github.com/luaposix/luaposix) or `Gio.UnixInputStream` from [lgi](https://github.com/pavouk/lgi) to get noticed hen events become available to read to avoid overloading your CPU and/or blocking the flow of application. Many of examples use `Gio.UnixInputStream` for this very reason.
