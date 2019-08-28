local wii = require 'xwiimote'

local pattern = ...

if not pattern or #pattern ~= 4 then
	print 'usage: set_led.lua pattern'
	print "where pattern 4 chars long string of 1's and 0's"
	return 0
end

-- Nothing special in this part
local path = assert(wii.monitor():poll(), 'No wiimotes found!')
print('Using wiimote at '..path)
local wiiremote = wii.iface(path)

-- LEDs don't have to be opened

for i=1,4 do
	local res, err = wiiremote:set_led(i, pattern:sub(i,i) == '1')
	if not res then
		print(('Failed to set LED state: %s. You may need root priviliges.'):format(err))
		return 1
	end
end
