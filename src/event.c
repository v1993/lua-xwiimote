#include "luawii.h"

const char* luawii_event_keyname(const enum xwii_event_keys key) {
	switch(key) {
		case XWII_KEY_LEFT:		return "left";
		case XWII_KEY_RIGHT:	return "right";
		case XWII_KEY_UP:		return "up";
		case XWII_KEY_DOWN:		return "down";
		case XWII_KEY_A:		return "A";
		case XWII_KEY_B:		return "B";
		case XWII_KEY_PLUS:		return "plus";
		case XWII_KEY_MINUS:	return "minus";
		case XWII_KEY_HOME:		return "home";
		case XWII_KEY_ONE:		return "one";
		case XWII_KEY_TWO:		return "two";
		case XWII_KEY_X:		return "X";
		case XWII_KEY_Y:		return "Y";
		case XWII_KEY_TL:		return "TL";
		case XWII_KEY_TR:		return "TR";
		case XWII_KEY_ZL:		return "ZL";
		case XWII_KEY_ZR:		return "ZR";

		case XWII_KEY_THUMBL:	return "thumb L";
		case XWII_KEY_THUMBR:	return "thumb R";
	};
	return "invalid";
};

void luawii_event_pushabs(lua_State* L, const struct xwii_event_abs* abs) {
	lua_createtable(L, 0, 3);
	lua_pushinteger(L, abs->x); lua_setfield(L, -2, "x");
	lua_pushinteger(L, abs->y); lua_setfield(L, -2, "y");
	lua_pushinteger(L, abs->z); lua_setfield(L, -2, "z");
};

bool luawii_event_push(lua_State* L, const struct xwii_event* ev) {
	lua_newtable(L);

	// Full timestamp
	lua_createtable(L, 0, 3);
	lua_pushinteger(L, ev->time.tv_sec);
	lua_setfield(L, -2, "sec");
	lua_pushinteger(L, ev->time.tv_usec);
	lua_setfield(L, -2, "usec");
	lua_setfield(L, -2, "timestamp_full");

	// Simple timestamp
	lua_pushnumber(L, (lua_Number)ev->time.tv_sec + ((lua_Number)ev->time.tv_usec)/1000000L);
	lua_setfield(L, -2, "timestamp");

	switch(ev->type) {
		case XWII_EVENT_KEY:
			lua_pushstring(L, luawii_event_keyname(ev->v.key.code));
			lua_setfield(L, -2, "key");
			lua_pushinteger(L, ev->v.key.state);
			lua_setfield(L, -2, "state");
			break;

		case XWII_EVENT_ACCEL:
			luawii_event_pushabs(L, &ev->v.abs[0]);
			lua_setfield(L, -2, "accel");
			break;

		case XWII_EVENT_IR:
			lua_createtable(L, 4, 0);
			size_t luaIdx = 0;
			for (size_t i = 0; i < 4; ++i) {
				const struct xwii_event_abs* abs = &ev->v.abs[i];
				// Workaround xwiimote bug #88: inital IR position is 0, 0 and not 1023
				if (xwii_event_ir_is_valid(abs) && abs->x != 0 && abs->y != 0) {
					lua_createtable(L, 0, 2);
					lua_pushinteger(L, abs->x); lua_setfield(L, -2, "x");
					lua_pushinteger(L, abs->y); lua_setfield(L, -2, "y");
					lua_rawseti(L, -2, ++luaIdx);
				}
			}
			lua_setfield(L, -2, "ir");
			break;

		case XWII_EVENT_BALANCE_BOARD:
			lua_createtable(L, 4, 0);
			for (size_t i = 0; i < 4; ++i) {
				const struct xwii_event_abs* abs = &ev->v.abs[i];
				lua_pushinteger(L, abs->x);
				lua_rawseti(L, -2, i+1);
			}
			lua_setfield(L, -2, "balance");
			break;

		case XWII_EVENT_MOTION_PLUS:
			luawii_event_pushabs(L, &ev->v.abs[0]);
			lua_setfield(L, -2, "mplus");
			break;

		case XWII_EVENT_PRO_CONTROLLER_KEY:
			lua_pushstring(L, luawii_event_keyname(ev->v.key.code));
			lua_setfield(L, -2, "pro_key");
			lua_pushinteger(L, ev->v.key.state);
			lua_setfield(L, -2, "state");
			break;

		case XWII_EVENT_PRO_CONTROLLER_MOVE:
			lua_createtable(L, 0, 2);
			for (size_t i = 0; i < 2; ++i) {
				const struct xwii_event_abs* abs = &ev->v.abs[i];
				lua_pushinteger(L, abs->x); lua_setfield(L, -2, "x");
				lua_pushinteger(L, abs->y); lua_setfield(L, -2, "y");
				lua_setfield(L, -2,  i == 0 ? "left" : "right");
			}
			lua_setfield(L, -2, "pro_move");
			break;

		case XWII_EVENT_WATCH:
			lua_pushboolean(L, true);
			lua_setfield(L, -2, "watch");
			break;

		default:
			return false;

	}

	return true;
};
