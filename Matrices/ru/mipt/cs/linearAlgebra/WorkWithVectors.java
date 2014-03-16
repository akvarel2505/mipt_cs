package ru.mipt.cs.linearAlgebra;

public class WorkWithVectors {

	//скал€рное произведение
	public static int scalarProduct(Vector a, Vector b){
		
		int res=0;
		int i, k=a.n;
		if (k!=b.n) ErrorHandler.error("Incorrect sizes of the vectors");
		else{
			for (i=0; i<k; i++){
				res+=a.x[i]*b.x[i];
			}
		}
		return res;
	}
	
	//сложить два вектора
	public static Vector addVectors(Vector a, Vector b){
		
		int i, k=a.n;
		Vector res=new Vector(0);
		
		if (k!=b.n) ErrorHandler.error("Incorrect sizes of the vectors");
		else{
			res=new Vector(k);
			for (i=0; i<k; i++){
				res.x[i]=a.x[i]+b.x[i];
			}
		}
		return res;
	}
	
	//домножить вектор на коэффициент
	public static Vector multiplyFactor(Vector a, byte factor){
		
		int i;
		Vector res=new Vector(a.n);
		for (i=0; i<a.n; i++) res.x[i]=a.x[i]*factor;
		return res;

	}
	
}
