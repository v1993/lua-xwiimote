#include "luawii.h"

const struct luaL_Reg luawii_global[] = {
	{"monitor", luawii_monitor_new},
	{"iface", luawii_iface_new},
	{NULL, NULL}
};

struct luawii_constants {
	const char* name;
	lua_Integer val;
};

const struct luawii_constants iface_type[] = {
	{ "core",		XWII_IFACE_CORE},
	{ "accel",		XWII_IFACE_ACCEL},
	{ "ir",			XWII_IFACE_IR},
	{ "mplus",		XWII_IFACE_MOTION_PLUS},
	{ "nunchuk",	XWII_IFACE_NUNCHUK},
	{ "classic",	XWII_IFACE_CLASSIC_CONTROLLER},
	{ "balance",	XWII_IFACE_BALANCE_BOARD},
	{ "pro",		XWII_IFACE_PRO_CONTROLLER},
	{ "all",		XWII_IFACE_ALL},
	{ "writable",	XWII_IFACE_WRITABLE},
	{ NULL, 0 }
};

int luaopen_xwiimote(lua_State* L) {
	luaL_newlib(L, luawii_global);
	//lua_newtable(L);
	for (size_t i = 0; iface_type[i].name != NULL; ++i) {
		lua_pushinteger(L, iface_type[i].val);
		lua_setfield(L, -2, iface_type[i].name);
	};
	//lua_setfield(L,-2, "const");
	return 1;
};
