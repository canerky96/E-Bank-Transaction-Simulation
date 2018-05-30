# E-Bank Transaction Simulation with Pthreads
for((i=1;i<=100;i++));do  ./check.sh  | tail -2 ;done
./check.sh

# Input File Format
An example input file is given below:<br />
4 //Number of customers<br />
2 //Number of tellers<br />
5 //Number of simulation days<br />
1 1234 1000 500<br />
2 1235 2000 800<br />
3 1237 4000 1500<br />
4 1234 1000 200<br />
5 1239 8250 900<br />
6 1235 6000 1000<br />
<br />
1. The first three lines of the input file will have three integer values (in the given order):
Number of customers, number of tellers and number of simulation days.<br />
<br />
2. The third line of the input file is followed by up to A lines (where A is the number of
accounts) each of which has the following format:<br />
<br />
< A_ID C_ID Amount DayLimit ><br />
• Here A_ID is the account ID.<br />
• C_ID is the customer ID. <br />
• Amount is the amount of money in that account. (TL)<br />
• DayLimit is the maximum amount that can be withdrawn from the account during a day<br />

# Format of the Log File
1. As the first part of the log file, there should be a line of information in the log file in the
following format for each transaction:<br />
< A_ID C_ID T_ID Operation Amount Simulation_Day ><br />
A_ID : Account ID<br />
C_ID : Customer ID<br />
T_ID : Teller ID<br />
Operation : Operation Type (View, Deposit, Withdraw, Transfer)<br />
Amount : Amount of money in that account.<br />
Simulation_Day : The day of the operation.<br />
2. The second part of the log file gives<br />
a. Current balance for each customer,<br />
b. Number of transactions performed by each customer and teller,<br />
c. Total cash of the bank.<br />

# Details
• Each customer and teller as a separate thread. The initial main
thread is responsible for creating customer/teller threads, managing them, and
displaying the final results as soon as the simulation completes.<br />
• Shared resources among multiple customers/tellers (i.e. threads) are bank
accounts. Preserving consistency and preventing deadlocks are major issues.<br />
• Pick the number of operations and operation types to be performed
by each customer/teller in random manner in simulation.<br />
• The total simulation time is D days. All the transactions within that period will be logged
in a file.<br />
• There are C customers and T tellers with unique IDs used by the bank system. Accounts
also have unique IDs.<br />
• Number of simulation days, customers and tellers would be taken from the input file.
Input file format is described above.<br />
• 10 seconds correspond to one simulation day.<br />
• All tellers know the IDs of all the customers. These tellers have all access rights on the
accounts of all customers.<br />
• A customer can only access to her/his own accounts for view/withdrawal, but can access
to other accounts for deposit/money transfer to them.<br />
• The bank has a limitation for the money withdrawal during a day from each account. In
total, a customer can not withdraw more than the given limit for her/his account during
a day. However, there is no such limit for money transfers between accounts since the
transfers are internal to the bank.<br />
• There can be maximum 3 operations on an account during a day.<br />
• Each customer can have at most 3 accounts on a bank.<br />
• In implementation of money transfer operation, a solution for
deadlock prevention is taken into account.<br />
• Multiple simultaneous view operations (i.e. reads) on an account should be allowed in
synchronization solution. However the other operations, deposit, withdrawal and
transfer (i.e. updates), should be performed in a mutually exclusive way.<br />
• Number of customers, tellers, simulation days, amount of money in each account and
limitation for the money withdrawal during a day would be taken from the input file
(based on the format given at the next page).<br />
• When the simulation is finished for the given number of days, the following should be
available in a log file.<br />