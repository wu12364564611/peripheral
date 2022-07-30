#include "Vector_algorithm.h"
/*ÂóÂÖÊ¸Á¿Ëã·¨*/

void mlyd(int x,int y,int f,int maxsdu)   
{ 
	
	int me1=0,me2=0,me3=0,me4=0;
	int c=0;
	int c2=0;
	me1+= x;              
	me2+= x;
	me3+= x;
	me4+= x;
	me1+= y;               
	me2-= y;
	me3+= y;
	me4-= y;
	if(c<myabs(me1))        
	{
		c=myabs(me1);
	}
	if(c<myabs(me2))
	{
		c=myabs(me2);
	}
	if(c<myabs(me3))
	{
		c=myabs(me3) ;
	}
		if(c<myabs(me4))
	{
		c=myabs(me4) ;
	}
		 
	if(c+myabs(f)<=maxsdu)      
	{
			me1+=f;
			me2-=f;
			me3-=f;
			me4+=f;
	}
	else    
	{
		if(myabs(f)<=maxsdu)      
		{	                       
			
			c2=maxsdu-myabs(f);		
			me1= me1 *c2/c;
			me2= me2 *c2/c;
			me3= me3 *c2/c;
			me4= me4 *c2/c;		
			 me1+=f;
			 me2-=f;
			 me3-=f;
			 me4+=f;		
		}
		else
		{	
	 	 	 me1+=f*maxsdu/myabs(f);
       me2-=f*maxsdu/myabs(f);
 	     me3-=f*maxsdu/myabs(f);
       me4+=f*maxsdu/myabs(f);
	  }
	}
 	Target_A = me1;
	Target_B = me2;
	Target_C = me3;
	Target_D = me4;

}


