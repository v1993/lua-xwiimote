local wii = require 'xwiimote'

-- Nothing special in this part
local path = assert(wii.monitor():poll(), 'No wiimotes found!')
print('Using wiimote at '..path)
local wiiremote = wii.iface(path)

local lgi = require 'lgi'
local GLib, Gio = lgi.GLib, lgi.Gio

local source

do
	local fd = wiiremote:get_fd()
	local stream = Gio.UnixInputStream.new(fd, false)
	source = stream:create_source()
	source:set_callback(function()
		for ev in wiiremote:iter() do
			local key = ev.key or ev.pro_key
			if key then
				print(("%s button %s %s"):format(
					ev.key and 'WiiMote' or 'ProController',
					key,
					(ev.state == 0 and 'up') or
					(ev.state == 1 and 'down') or
					'auto-repeat'
				))
			end
		end
		return true
	end)
end

local loop = lgi.GLib.MainLoop()
source:attach(loop:get_context())
wiiremote:open(wii.core)

print()
loop:run()
