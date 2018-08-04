# Admiral Business Machines 1954-2042
# Display ROM content on terminal

.chipsets:
clock		clock
false		gnd
4040		counter
2716		rom(rom.bin)
terminal	out


.links:
clock:1		counter:10
gnd:1		counter:11

counter:9	rom:8
counter:7	rom:7
counter:6	rom:6
counter:5	rom:5
counter:3	rom:4
counter:2	rom:3
counter:4	rom:2
counter:13	rom:1
counter:12	rom:23
counter:14	rom:22
counter:15	rom:19

rom:9		out:1
rom:10		out:2
rom:11		out:3
rom:13		out:4
rom:14		out:5
rom:15		out:6
rom:16		out:7
rom:17		out:8

gnd:1		rom:18
gnd:1		rom:20

clock:1		out:9
