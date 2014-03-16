package ru.mipt.cs.linearAlgebra;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Scanner;


public class Vector {
	
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
	
	public static Vector readFromFile(String path) throws FileNotFoundException, IOException{

			Scanner scanner = new Scanner(new File(path));
			
			int n,j,value;
			n=scanner.nextInt();
			
			Vector res=new Vector(n);
			
			for (j=0; j<n; j++){
				value=scanner.nextInt();
				res.x[j]=value;
			}
			
			scanner.close();
			return res;
		
	}
}


