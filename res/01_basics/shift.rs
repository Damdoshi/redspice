# Admiral Business Machines 1954-2042
# 4 bits adder

.chipsets:
clock		clock
input		data
input		strobe
input		oe

4094		shifter

output		q0
output		q1
output		q2
output		q3
output		q4
output		q5
output		q6
output		q7

output		qs0
output		qs1

.links:
clock:1		shifter:3
data:1		shifter:2
strobe:1	shifter:1
oe:1		shifter:15
q0:1		shifter:4
q1:1		shifter:5
q2:1		shifter:6
q3:1		shifter:7
q4:1		shifter:14
q5:1		shifter:13
q6:1		shifter:12
q7:1		shifter:11

qs0:1		shifter:9
qs1:1		shifter:10

