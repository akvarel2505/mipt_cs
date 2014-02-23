public class Member extends Biathlon{

	private float accuracy;
	private float speed;
	private float changeWait; //медлительность спортсмена при смене пуль 
	private String name;

	Member(String str){
		super();

		//случайным образом задаем меткость и скорость спортсмена
		accuracy=(float)Math.random();
		if (accuracy<0.5) accuracy+=0.5;// чтобы не промазывал постоянно =)
		speed=(float)(7*(Math.random()+0.75));
		changeWait=(float)(8+3*(Math.random()+0.75));

		name=str;
	}
	
	public String getName(){
		return name;
	}

	public float go(){
		float totalTime=0;

		System.out.println();

		System.out.println(String.format("I, %s, am starting",name));
		totalTime+=(distance/speed);

		int i;
		int nMiss=0;
		byte nPos=nPositions;
		for (i=0; i<nPos; i++) totalTime+=positionResult();

		return totalTime;
	}

	private float positionResult(){
		byte nAim=nAims;
		byte nAdd=nAdditional;
		float pun=punishment;

		float time=changeWait; //minimal time for the shooting

		int i;
		float fortune;
		for (i=0; i<nAim; i++)
		{
			nAim-=shot(); //determination the result of main shot
		}

		// additional shots
		while ((nAim>0)&&(nAdd>0))
		{
			System.out.println(String.format("Additional shot..."));
			time+=changeWait;
			nAim-=shot();
			nAdd--;
		}

		time+=(nAim*pun)/speed;  //спортсмен бежит столько дополнительных кругов, сколько промахов он сделал в конечном итоге позиции
		
		System.out.println(String.format("%d missings and equal number of additional rounds",nAim));

		return time;
	}

	private byte shot(){
		float fortune=(float)Math.random();
		return (byte)Math.floor(fortune+accuracy); //determine the result of the shot
	}

}
