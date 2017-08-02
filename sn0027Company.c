//CS-687 Project 1
//By: Suhas Suresh and Vaidyanath Areyur Shanthakumar

#include <stdio.h>
#include <string.h>
#include <libpq-fe.h>

EXEC SQL INCLUDE sqlca;

EXEC SQL WHENEVER SQLERROR sqlprint;

int main(int argc, char *argv[])
{
	
//Declaration of variables which will be required in the sql query to retrieve the data.
EXEC SQL BEGIN DECLARE SECTION;
     char      			ssn[9];
     unsigned int       pno;
	 unsigned int       projno;
	 int 				employeecount;
     float     			hrs,hours,totalemphours;
     int       			sal;
     char*      		mssn=NULL;
     char*				fname=NULL;
     char*				lname=NULL;
     char* 				minit=NULL;
	 char*				mgrname=NULL;
	 char*				mgrfname=NULL;
     char*				mgrlname=NULL;
     char* 				mgrminit=NULL;
     char*				dname=NULL;
	 char*				pname=NULL;
	 int 				updateflag,flag;
     float 				salary;
     int 				no_of_dep;	
     char      			date[12];
	 int 				i;
EXEC SQL END DECLARE SECTION;

	//Establishing connection to the database	
     EXEC SQL CONNECT TO unix:postgresql://localhost /cs687 USER sn0027 USING "f16687"; 

     printf("CONNECTED\n");
	 
	//Check on number of command line arguments and reading the command line arguments if its correctly passed.
	 if(argc==7)
	 {
		for (i=0;i<argc;i++)
		{
			if (strcmp(argv[i],"-ssn")==0)
			{
				strcpy(ssn,argv[++i]);				
			}
			if (strcmp(argv[i],"-pno")==0)
			{	
				pno=atoi(argv[++i]);
			}
			if (strcmp(argv[i],"-hours")==0)
			{
				sscanf(argv[++i],"%f",&hrs);
			}
		}
		flag=0;
		no_of_dep = 0;
		EXEC SQL DECLARE c_empdept CURSOR FOR 
			 SELECT e.fname, e.minit, e.lname, d.dname, e.salary
			 FROM sn0027_vs0027.employee e, sn0027_vs0027.department d, sn0027_vs0027.dependent t 
			 WHERE e.dno=d.dnumber
			 AND e.ssn = :ssn;
		EXEC SQL OPEN c_empdept;

		EXEC SQL WHENEVER NOT FOUND DO BREAK;

		while (SQLCODE==0)
		{	 
			EXEC SQL FETCH IN c_empdept INTO :fname, :minit, :lname, :dname, :salary;
		}
		EXEC SQL CLOSE c_empdept;
		//Query 1: To retrieve employee’s name, employee’s department name, salary, and the number of dependents	
		EXEC SQL DECLARE c_empdep CURSOR FOR 
			 SELECT e.fname, e.minit, e.lname, d.dname, e.salary, count(t.dependent_name) Number_of_Dependents 
			 FROM sn0027_vs0027.employee e, sn0027_vs0027.department d, sn0027_vs0027.dependent t 
			 WHERE e.dno=d.dnumber 
			 AND e.ssn = t.essn 
			 AND e.ssn = :ssn			 
			 GROUP BY t.essn,e.fname,e.minit,e.lname,d.dname,e.salary;

		EXEC SQL OPEN c_empdep;

		EXEC SQL WHENEVER NOT FOUND DO BREAK;

		while (SQLCODE==0)
		{
			
		   EXEC SQL FETCH IN c_empdep INTO :fname, :minit, :lname, :dname, :salary, :no_of_dep;
		   printf("\n");
		   printf("|\tFirst Name\t |\tMiddle initial\t |\tLast Name\t |\t Department\t |\t  Salary\t  |\t Number of Dependents\t \n");
		   printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		   printf("|\t  %s  \t |\t    %s    \t |\t  %s  \t |\t %s  \t |\t  %.2f     \t  |\t          %d    \t \n",fname,minit,lname,dname,salary,no_of_dep);
		   printf("\n");
		   
		   
		}
		if(no_of_dep==0)
		{
			while (SQLCODE==100)
			{
			   if (flag==1)
					break;
			   printf("\n");
			   printf("|\tFirst Name\t |\tMiddle initial\t |\tLast Name\t |\t Department\t |\t  Salary\t  |\t Number of Dependents\t \n");
			   printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			   printf("|\t  %s  \t |\t    %s    \t |\t  %s  \t |\t %s  \t |\t  %.2f     \t  |\t          %d    \t \n",fname,minit,lname,dname,salary,no_of_dep);
			   printf("\n");
			   flag = 1;
			}
		}
		EXEC SQL CLOSE c_empdep;
		
	
		
		//Query 2: To retrieve the project number, the project name, the name of the department manager controlling the project, the number of hours the employee is working on the project, the number of employees working on the project, the total number of hours for the project.
		EXEC SQL DECLARE c_empproj CURSOR FOR  
		SELECT w.pno, p.pname, m.fname||' '||m.minit||' '||m.lname Manager, w.hours, employeecount, totalemphours from 
		(select pno as pnum,count(essn) as employeecount,sum(hours) as totalemphours from sn0027_vs0027.works_on 
		group by pno) as empworkson,sn0027_vs0027.works_on w, sn0027_vs0027.employee m, sn0027_vs0027.department d,sn0027_vs0027.project p
		where essn=:ssn and 
		w.pno=p.pnumber AND
		empworkson.pnum = w.pno AND
		m.dno=d.dnumber AND
		p.dnum=d.dnumber AND
		d.mgr_ssn = m.ssn;
		EXEC SQL OPEN c_empproj;

		EXEC SQL WHENEVER NOT FOUND DO BREAK;

		printf("|\t ProjectNo\t |\t  ProjectName\t    |\t       Manager\t          |\t    Hours\t   |\t  NoOfEmployee\t    |\t TotalEmpHours\n");
		printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  
		while (SQLCODE==0)
		{
			  EXEC SQL FETCH IN c_empproj INTO :projno, :pname, :mgrfname, :hours, :employeecount, :totalemphours;  
			  printf("|\t     %d\t         |\t    %s\t    |\t   %s\t  |\t    %.2f\t   |\t       %d\t    |\t      %.2f\n",projno,pname,mgrfname,hours,employeecount,totalemphours);
			  printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		}

		printf("\n");

		EXEC SQL CLOSE c_empproj;

		//To retrieve name of project corresponding to the project number provided by the user.
		EXEC SQL DECLARE c_proj CURSOR FOR
		SELECT pname
		FROM sn0027_vs0027.PROJECT
		WHERE pnumber = :pno;
		EXEC SQL OPEN c_proj;
		EXEC SQL WHENEVER NOT FOUND DO BREAK;
		while (SQLCODE==0)
		{
			EXEC SQL FETCH IN c_proj INTO :pname;
		}
		EXEC SQL CLOSE c_proj;
		
		//To retrieve the details of the employee and the details of the project that he/she is working on. 
		EXEC SQL DECLARE c_empwkonproj CURSOR FOR
		SELECT e.fname, e.minit, e.lname, p.pname,w.hours 
		FROM sn0027_vs0027.WORKS_ON w, sn0027_vs0027.EMPLOYEE e, sn0027_vs0027.PROJECT p
		WHERE w.essn = :ssn AND
		w.essn = e.ssn AND
		p.pnumber = w.Pno AND
		pno= :pno;
		 
		EXEC SQL OPEN c_empwkonproj;

		EXEC SQL WHENEVER NOT FOUND DO BREAK;
		updateflag=0;		
		while (SQLCODE==0)
		{
			EXEC SQL FETCH IN c_empwkonproj INTO :fname, :minit, :lname, :pname,:hours; 
			if (updateflag==1)
				break;
			
			//if the hours input by the user is greater than 0, then working hours for the corresponding project is updated in table WORKS_ON.
			if(hrs>0)
			{
				printf("Updating\n");
				EXEC SQL UPDATE sn0027_vs0027.WORKS_ON
				SET hours = :hrs
				WHERE essn = :ssn AND
				pno= :pno;
				printf("The number of hours for employee %s %s %s on project %s is updated from %f to %f.\n",fname,minit,lname,pname,hours,hrs);
				updateflag=1;
			}
			
			//if the hours input by the user is 0, then user stops working on the project and hence the corresponding details will be deleted from table WORKS_ON.
			if(hrs==0)
			{
				printf("Deleting\n");
				EXEC SQL DELETE
				FROM sn0027_vs0027.WORKS_ON
				WHERE essn = :ssn AND
				pno= :pno;
				updateflag=1;
				printf("Employee %s %s %s who was working for %f hours on project %s stopped working on this project.\n",fname,minit,lname,hours,pname);
			}
		}
		//If the user is not working on the particular project then insert a new row in the table WORKS_ON.
		while (SQLCODE==100)
		{
			if (updateflag==1)
			{
				break;
			}
			printf("Inserting\n");
			EXEC SQL INSERT INTO sn0027_vs0027.WORKS_ON VALUES(:ssn,:pno,:hrs);
			updateflag=1;
			printf("Employee %s %s %s started to work for %f hours on project %s.\n",fname,minit,lname,hrs,pname);
		}
		EXEC SQL CLOSE c_empwkonproj;
		EXEC SQL COMMIT;
		
		//To retrieve the details after Updation/Deletion/Insertion
		printf ("After Updation/Deletion/Insertion:\n\n");
		
		EXEC SQL OPEN c_empproj;

		EXEC SQL WHENEVER NOT FOUND DO BREAK;

		printf("|\t ProjectNo\t |\t  ProjectName\t    |\t       Manager\t          |\t    Hours\t   |\t  NoOfEmployee\t    |\t TotalEmpHours\n");
		printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  
		while (SQLCODE==0)
		{
			  EXEC SQL FETCH IN c_empproj INTO :projno, :pname, :mgrfname, :hours, :employeecount, :totalemphours;  
			  printf("|\t     %d\t         |\t    %s\t    |\t   %s\t  |\t    %.2f\t   |\t       %d\t    |\t      %.2f\n",projno,pname,mgrfname,hours,employeecount,totalemphours);
			  printf("---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		}

		printf("\n");

		EXEC SQL CLOSE c_empproj;

		EXEC SQL DISCONNECT;

	}
	else 
	{
		printf("Incorrect method of passing command line arguments to the function\n");
		printf("Correct Usage: ./mycompany -ssn 123456789 -pno 1 -hours 32.5\n");
	}
return 0;

}
