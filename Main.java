public class Main{

	public static void main(String args[]){
	
		final int nTms=5;

		Team allTeams[] = new Team[nTms];
		int i,j;
		float scores[]=new float[nTms];
		byte awards[]=new byte[nTms];
		
		allTeams[0]= new Team("Germany",0);
		allTeams[1]= new Team("Russia",1);
		allTeams[2]= new Team("China",2);
		allTeams[3]= new Team("China", 3);
		allTeams[4]= new Team("USA",4);

		awards[0]=0;
		for (i=0; i<nTms; i++)
		{
			allTeams[i].printInfo();
			scores[i]=allTeams[i].start();
			System.out.println();
			j=0;
			while ((j<i)&&(scores[i]>scores[awards[j]])) j++;
			rotateAwards(awards,i,j);
		}
	
	// print result
		System.out.println();
		System.out.println();
		String s;
		for (i=0; i<nTms; i++){
			s=String.format("%d. %.2f ",i+1,scores[awards[i]])+allTeams[awards[i]].getCountry();
			System.out.println(s);
		}
	
		System.out.println("The competition is over");	
	}

	private static void rotateAwards(byte ar[], int ins, int begin){
		int i;
		for (i=ins-1; i>=begin; i--) ar[i+1]=ar[i];
		ar[begin]=(byte)ins;
	}
	

}
