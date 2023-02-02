# Admiral Business Machines 1954-2042
# One or gate

.chipsets:
input		a		[-10, -10]
input		b		[-10, +10]
output		s		[+10,   0]
4071		gate		[ -1,  -3]

.links:
a:1		gate:1		[ -3,  -3]
b:1		gate:2
s:1		gate:3
