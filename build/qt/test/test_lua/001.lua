print("begin 000.lua")

print(color.r)
print(color.g)
print(color.b)

color.fn_test(1, 1.23, false)

function gcl_deal_received_message2(pram1, param2)
	print( type(param1) )
	printf( type(param2) )
	return 10
end
--[[
function printf(...) io.write(string.format(unpack(arg))) end

function Account:show()
  print("Account balance = $%0.02f\n", self:balance())
end

print("track 002")

a = Account(100)
b = Account:new(30)

print('a =', a)
print('b =', b)
print('metatable =', getmetatable(a))
print('Account =', Account)
table.foreach(Account, print)

a:show() a:deposit(50.30) a:show() a:withdraw(25.10) a:show()

parent = {}

function parent:rob(amount)
  amount = amount or self:balance()
  self:withdraw(amount)
  return amount
end

getmetatable(Account).__index = parent

debug.debug()
--]]

