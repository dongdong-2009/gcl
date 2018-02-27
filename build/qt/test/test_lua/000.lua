print("begin 000.lua")

if (ary1 ~= nil) then
	print("begin ary1")
	local ary1_n = #ary1
	print(ary1_n)
	for i=1 , ary1_n do
	    a = ary1[i]
		s = ""
		for j=1 , 9 do
			s = s .. " " .. a["field" .. j]	
		end
--		print(s)
	end
end
