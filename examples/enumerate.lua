local wii = require 'xwiimote'

-- Way one
local mon = wii.monitor()
print('Connected wiimotes:')
for path in mon:iter() do
	print(path)
end

print()

-- Way two
local mon = wii.monitor()
print('Connected wiimotes:')
for i=1,math.maxinteger or math.huge do
	local path = mon:poll()
	if not path then break end
	print(('%d: %s'):format(i, path))
end
