public class Team extends Biathlon{

	private String country;
	private int id;
	private Member people[];
	private float teamResult;	 

	Team(String str, int i){
		super();
		id=i;
		country=str;

		people=new Member[nMembers];

		for (i=0; i<nMembers; i++){
			people[i] = new Member(String.format("%d_%s_%d",id,country,i+1));			
		}
		teamResult=-1;
	}

	public void printInfo(){
		byte nMemb=nMembers;
		System.out.println("Country: "+country);
		int i;

		for (i=0; i<nMemb; i++) System.out.println(people[i].getName());
	}

	public int getID(){
		return id;
	}
	
	public float getTeamResult(){
		return teamResult;
	}


	public String getCountry(){
		return country;
	}
	
	public float start(){

		int i,nMemb;
		float currTime, totalTime=0;

		nMemb=nMembers;
	
		for (i=0; i<nMemb; i++){
			currTime = people[i].go();
			System.out.println(people[i].getName()+", time (s): "+String.format("%f",currTime));
			totalTime+=currTime;			
		}

		System.out.println(String.format("Team %d from %s finished in %f s",id,country,totalTime));
		teamResult=totalTime;

		return totalTime;
	}

}
