
public class Matrix {
	
	private boolean ifTransposed;
	final public int n,m; // size: n lines, m columns
	private Vector[] line;
	
	public Matrix(int a, int b){
		
		ifTransposed=false; // non-transposed by default
		n=a;
		m=b;
		line=new Vector[n];
		
		int i;
		for (i=0; i<n; i++){
			line[i]=new Vector(m);
		}
		
	}
	
	//�������� ������� �������
	public int get(int i, int j){
		//errors!!!
		if (!ifTransposed) return this.line[i].x[j];
		else return this.line[j].x[i];
	}
	
	//set an element of matrix
	public void set(int i, int j, int value){
		//errors!!!
		if (!ifTransposed) this.line[i].x[j]=value;
		else this.line[j].x[i]=value;
	}
	
	public void println(){
		int i,max;
		Vector curr;
		if (ifTransposed) max=m; else max=n;
		for (i=0; i<max; i++){
			curr=getLine(i);
			curr.println();
		}
	}
	
	public void transpose(){
		ifTransposed=!ifTransposed;
	}
	
	public boolean ifTransposed(){
		return ifTransposed;
	}
	
	//�������� ���� �� ������������ ��������
	public Vector getMainLine(int i){
		Vector res=new Vector(this.n);
		try {
			res=this.line[i];
		}
		catch (ArrayIndexOutOfBoundsException e){
			ErrorHandler.error("Demand of absent line"); //error
		}
		finally{
			return res;
		}
	}
	
	//���������� ������������� ������
	public void setMainLine(int i, Vector a){
		try{
			this.line[i]=a;
		}
		catch (ArrayIndexOutOfBoundsException e1){
			ErrorHandler.error("Demand of absent line"); //error
		}
		catch (ArrayStoreException e2){
			ErrorHandler.error("Wrong vector dimension"); //error
		}
	}
	
	//�������� ������; ���� ������� �� ���������������, �� ��� - ���� � �� �� � getMainLine()
	public Vector getLine(int n){
		
		Vector res;
		if (!ifTransposed) {			
			res=new Vector(this.m);
		}
		else{
			res = new Vector(this.n); 
		}
		
		try{
			if (ifTransposed) {
				int i;
				for (i=0; i<this.n; i++) res.x[i]=this.line[i].x[n];
			}
			else{
			res = this.line[n]; 
			}
		}
		catch (ArrayIndexOutOfBoundsException e){
			ErrorHandler.error("Demand of absent line"); //error
		} 
		finally {
			return res;
		}
	}
	
	//get a column
	public Vector getColumn(int n){
		Vector res;
		if (ifTransposed) {			
			res=new Vector(this.n);
		}
		else{
			res = new Vector(this.m); 
		}
		
		try{
			if (!ifTransposed) {
				int i;
				for (i=0; i<this.n; i++) res.x[i]=this.line[i].x[n];
			}
			else res=this.line[n];
		}
		catch (ArrayIndexOutOfBoundsException e){
			ErrorHandler.error("Demand of absent column"); //error
		} 
		finally {
			return res;
		}
	}
	
	public void swapLines(int i, int j){
		
		try{
			if (ifTransposed) {
				int k,buf;
				for (k=0; k<this.n; k++) {
					buf=this.line[k].x[i];
					this.line[k].x[i]=this.line[k].x[j];
					this.line[k].x[j]=buf;
				}
		
		}
			else{
				Vector buffer=new Vector(this.m);
				buffer=this.line[i];
				this.line[i]=this.line[j];
				this.line[j]=buffer;
			}
		}
		catch (ArrayIndexOutOfBoundsException e){
			ErrorHandler.error("Demand of absent line"); //error
		} 
	}
	
	public void swapColumns(int i, int j){
		try{
			if (!ifTransposed) {
			
				int k,buf;
				for (k=0; k<this.n; k++) {
					buf=this.line[k].x[i];
					this.line[k].x[i]=this.line[k].x[j];
					this.line[k].x[j]=buf;
				}
			
			}
			else{
			
				Vector buffer=new Vector(this.m);
				buffer=this.line[i];
				this.line[i]=this.line[j];
				this.line[j]=buffer;
			}
		}
		catch (ArrayIndexOutOfBoundsException e){
			ErrorHandler.error("Demand of absent line"); //error
		} 
	}
	
	//�������� ������� � ������������� ����; ���������� ��, �� ��� ��� �������� ������������
	public int toDiagonal(){
			
			boolean haveWeTransposed=false;
			if (ifTransposed){
				this.transpose();
				haveWeTransposed=true;
			}
			
			int i=0; //������� ������� �� ������� ���������
			int j=0;
			int nNull=0; //������� ���������� ������� �����
			int determinantFactor=1;
			int k;
			byte flag;
			
			for (i=0; i<(this.n-nNull); i++){    

				flag=0;
				//���� ������ ��������� ������� � ������
				while ((nNull<(this.n-i))&&(flag==0)){                
					j=i;
							
					while ((j<this.m)&&(flag==0)){
						if ((this.get(i,j)==0)) j++; else flag=1;
					}
					
					if (j==this.m){
					//null line, sending it to the bottom
						this.swapLines(i,this.n-nNull-1);
						nNull++;
					}
					else{
						//swap columns
						if (i!=j) this.swapColumns(i,j);
						determinantFactor*=(-1);
					}
				}
				// ���� ��������� �� �� ��������� ������� ������
				if (nNull<this.n-i){
				
					// "�������" ��� �������� ���� � ���� ���� (current coordinates (i,i))
					int factor, factorKiller;
					int killer=this.get(i,i);
					for (j=0; j<this.n; j++){
						int killed=this.get(j,i);
						if ((j!=i)&&(killed!=0)){							
							factor=NOK(killer,killed)/killed;
							factorKiller=NOK(killer,killed)/killer;
							determinantFactor*=factor;
							for (k=i; k<this.m; k++) {
								int value=factor*this.get(j,k)-factorKiller*this.get(i,k);
								this.set(j,k,value);
							}
						}
					}
					//��������� �������� �������
				}
				
			}
			
			if (haveWeTransposed) this.transpose();
			return determinantFactor;
		}
	
	private int NOD(int a0, int b0){
		int c;
		int a=Math.max(Math.abs(a0),Math.abs(b0));
		int b=Math.min(Math.abs(b0),Math.abs(a0));
		while (b!=0){
			c=a%b;
			a=b;
			b=c;
		}
		return a;
	}
	
	private int NOK(int a, int b){
		return a*b/NOD(a,b);
	}
}
