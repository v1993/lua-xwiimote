local wii = require 'xwiimote'

-- Nothing special in this part
local path = assert(wii.monitor():poll(), 'No wiimotes found!')
print('Using wiimote at '..path)
local wiiremote = wii.iface(path)

assert(wiiremote:open(wii.core | wii.writable))

local res, err = wiiremote:rumble(true)
if not res then
	print(('Failed to set rumble state: %s.'):format(err))
	return 1
end

print('Press ENTER to switch rumble, Ctrl+C to stop and exit')

while true do
	io.read()
	assert(wiiremote:rumble(false))
	io.read()
	assert(wiiremote:rumble(true))
end
