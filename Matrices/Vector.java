public class Vector {
	
	// AP: здесьи везде - если final - то это не значит, что модификатор доступа не нужно указывать)
	final public int n;    // dimensions
	public int x[];				//components
	
	public Vector(int dim){
		n=dim;
		x=new int[n];
		
		//initialization
		int i;
		for (i=0; i<n; i++){
			x[i]=0;
		}
		
	}
	
	public void println(){
		for (int i=0; i<this.n; i++){
			System.out.print(String.format("%d ",this.x[i]));
		}
		System.out.println();
	}

}
