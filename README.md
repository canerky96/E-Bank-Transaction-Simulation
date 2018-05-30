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