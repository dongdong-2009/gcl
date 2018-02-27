
print("begin 000.lua")

print(color.r)
print(color.g)
print(color.b)

ms = {}

for i=1, 100000 do
	msa = {}
	msa['q'] = 11
	msa['t'] = 12
	msa['v'] = 13
	ms[i] = msa
end

color.fn_test(ms)
