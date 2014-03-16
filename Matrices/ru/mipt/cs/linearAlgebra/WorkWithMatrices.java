package ru.mipt.cs.linearAlgebra;

import java.io.File;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.lang.ArrayIndexOutOfBoundsException;
import java.util.Scanner;
import java.lang.IllegalArgumentException;

public class WorkWithMatrices{

	//��������� ��� ������� �� �����������
	public static Matrix multiplyFactor(Matrix A, byte k){
		int i;
		Matrix res=new Matrix(A.n, A.m);
		for (i=0; i<A.n; i++) res.setMainLine(i, WorkWithVectors.multiplyFactor(A.getMainLine(i),k));
		return res;
	}
	
	//���������� ������������ ���������� �������
	public static int determinant(final Matrix A){
		if (A.n==A.m){
			Matrix B=new Matrix(A.n, A.n);
			B=A;
			int factor=B.toDiagonal();
			
			int i;
			int res=1;
			for (i=0; i<A.n; i++) res*=B.get(i,i);
			return (res/factor);
		}
		else {
			System.out.println("The matrix is not square");
			return -1;
		} //error
	}
	
	//���������� ����� ���������� �������
		public static int trace(Matrix A){
			if (A.n==A.m){
				int i;
				int res=0;
				for (i=0; i<A.n; i++) res+=A.get(i,i);
				return res;
			}
			else {
				ErrorHandler.error("Non-square matrix"); //error
				return -1;
			}
		}
	
	private static int lineNum, colNum;
	private static void checkSizes(Matrix A, Matrix B) throws IllegalArgumentException{
		
		IllegalArgumentException e = new IllegalArgumentException();
		// (1) A,B (2) At,B 
		// (3) A,Bt (4) At,Bt
		
		byte flag;
		if (B.ifTransposed()){
			if (A.ifTransposed()) flag=4;
			else flag=3;
		}
		else{
			if (A.ifTransposed()) flag=2;
			else flag=1;			
		}
		
		// ��������� �������
		
		lineNum=A.n;
		colNum=B.m;   //by default
		
		if (flag==1){
			if (A.m!=B.n) throw e;
		}
		
		if (flag==2){
			if (A.n!=B.n) throw e;
			else lineNum=A.m;		
		}
		
		if (flag==3){
			if (A.m!=B.m) throw e;
			else colNum=B.n;
		}
		
		if (flag==4){
			if (A.n!=B.m) throw e;
			else{
				lineNum=A.m;
				colNum=B.n;
			}
		}
		
	}
		
	public static Matrix multiplication(Matrix A, Matrix B) throws IllegalArgumentException{
		
		checkSizes(A,B);
		
		int i;
		Matrix res=new Matrix(lineNum,colNum);
		
		for (i=0; i<lineNum; i++)
			res.setMainLine(i,(Vector)multiplication(A.getLine(i),B));
		
		return res;
	}
	
	//������ ����� �������
	public static Vector multiplication(Matrix A, Vector a) throws ArrayIndexOutOfBoundsException{
		if ((a.n!=A.n)&&(a.n!=A.m)){ //error
				ArrayIndexOutOfBoundsException e;
				e = new ArrayIndexOutOfBoundsException();
				throw e;
			}
		if (A.n==A.m){ //����� ������, ��������������� �������, ��� ���
			Vector res=new Vector(A.n);
			for (int i=0; i<A.n; i++) res.x[i]=WorkWithVectors.scalarProduct(A.getLine(i),a);
			return res;
		}
		//�� ���������� �������
		else return multiplicationNonSquare(A,a);
	}
	
	//������ ����� ��������
	public static Vector multiplication(Vector a, Matrix A) throws ArrayIndexOutOfBoundsException{
		if ((a.n!=A.n)&&(a.n!=A.m)){ //error
				ArrayIndexOutOfBoundsException e;
				e = new ArrayIndexOutOfBoundsException();
				throw e;
			}
		if (A.n==A.m){ //����� ������, ��������������� �������, ��� ���
			Vector res=new Vector(A.n);
			for (int i=0; i<A.n; i++) res.x[i]=WorkWithVectors.scalarProduct(A.getColumn(i),a);
			return res;
		}
		//�� ���������� �������
		else return multiplicationNonSquare(A,a);
	}
	
	private static Vector multiplicationNonSquare(Matrix A, Vector a){
		//����� ��� �������������, ��� ������� ������ ���������
		// �� �������� ������������� ���������, � ����� ������� ��������
		
		Vector res;
		int i;
		if (a.n==A.m){
			res=new Vector(A.n);
			for (i=0; i<A.n; i++) res.x[i]=WorkWithVectors.scalarProduct(A.getMainLine(i),a);
		}
		else{
			res=new Vector(A.m);
			if (!A.ifTransposed()){
				for (i=0; i<A.m; i++) res.x[i]=WorkWithVectors.scalarProduct(A.getColumn(i),a);
			}
			else{
				for (i=0; i<A.m; i++) res.x[i]=WorkWithVectors.scalarProduct(A.getLine(i),a);
			}
		}
		return res;
	}
	
	
	public static Matrix readFromFile(String path) throws FileNotFoundException, IOException, ArrayIndexOutOfBoundsException{

			Scanner scanner = new Scanner(new File(path));
			
			int n,m,i,j,value;
			n=scanner.nextInt();
			m=scanner.nextInt();
			
			Matrix res=new Matrix(n,m);
			
			for (i=0; i<n; i++){
				for (j=0; j<m; j++){
					value=scanner.nextInt();
					res.set(i, j, value);
				}
			}
			
			scanner.close();
		
			return res;
	}
}
