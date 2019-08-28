local wii = require 'xwiimote'

-- Nothing special in this part
local mon = wii.monitor(true)
print('Wiimotes connected on startup:')
for path in mon:iter() do
	print(path)
end

local lgi = require 'lgi'
local GLib, Gio = lgi.GLib, lgi.Gio

local source

do
	-- You can also simply use blocking mode, but this way program can keep running things (show windows, handle user input, etc.)
	local res, fd = mon:set_blocking(false)
	assert(res)
	local stream = Gio.UnixInputStream.new(fd, false)
	source = stream:create_source()
	source:set_callback(function()
		-- It sometimes fires as false alarm
		local res = mon:poll()
		if res then
			print('New WiiRemote detected:', res)
		end
		return true
	end)
end

local loop = lgi.GLib.MainLoop()
source:attach(loop:get_context())

print()
print('Now newly connected WiiMotes will be printed below:')
loop:run()
