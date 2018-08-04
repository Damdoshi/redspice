# Admiral Business Machines 1954-2042
# Some gate linked after a counter

.chipsets:
clock			clock
false			gnd
4040			counter
4071			or

output			L
output			H


.links:
clock:1			counter:10
gnd:1			or:1

counter:9		or:5
counter:7		or:6
counter:6		or:9
counter:5		or:8

L:1			or:4
H:1			or:10

# If you do not succed in execute this file, exchange those lines
#gnd:1			or:2
counter:3		or:2

or:3			counter:11