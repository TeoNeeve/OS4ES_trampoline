A power plant supervision system is composed of an embedded computer that acquires data from 4 sensors. Sensors A and B are acquired every 250msec; sensors C and D are acquired every 500msec. Acquisition takes 5msec for each of these sensors. 

After acquisition the data are displayed on the video terminal of an operator that monitors the plant operations. Display operation takes 2msec.

The acquired data are also sent to a remote power grid coordinator at least every minute. The grid coordinator uses the data to manage energy distribution. The transmission takes 100msec and cannot be interrupted by acquisition tasks. If interrupted, the transmission is aborted and the grid coordinator is not able to perform its duty properly.

A dedicated hardware device implements a safety mechanism: in case the hardware detects a critical condition, it shuts down the power plant and triggers the supervision system to transmit an emergency message to the grid coordinator. The emergency transmission shall exhibit the lowest possible latency and takes 50msec for executing.

The plan resume operating only upon receiving a command from the power grid coordinator.

a. Propose a suitable software architecture and task scheduling
b. Simulate the system behavior using OSEK/posix. Emergency is activate at time 100msec. Normal operativity resumes at time 450ms.

