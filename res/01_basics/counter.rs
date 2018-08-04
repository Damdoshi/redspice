# Admiral Business Machines 1954-2042
# 12 bits counter

.chipsets:
clock		clock
input		reset

output		a
output		b
output		c
output		d
output		e
output		f
output		g
output		h
output		i
output		j
output		k
output		l

4040		counter

.links:
clock:1		counter:10
reset:1		counter:11

a:1		counter:9
b:1		counter:7
c:1		counter:6
d:1		counter:5
e:1		counter:3
f:1		counter:2
g:1		counter:4
h:1		counter:13
i:1		counter:12
j:1		counter:14
k:1		counter:15
l:1		counter:1

