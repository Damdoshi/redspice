# Admiral Business Machines 1954-2042
# One or gate

.chipsets:
input		a		[ 3,  5]
input		b		[ 3, 15]
output		s		[50, 10]
4071		gate		[30, 10]

.links:
a:1		gate:1		[3,5 b3,7 b30,10]
b:1		gate:2
s:1		gate:3
