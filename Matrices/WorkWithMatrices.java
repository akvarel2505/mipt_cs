import java.io.FileInputStream;
import java.io.IOException;
import java.util.Scanner;

public class WorkWithMatrices{

	//��������� ��� ������� �� �����������
	public static Matrix multiplyFactor(Matrix A, byte k){
		int i;
		Matrix res=new Matrix(A.n, A.m);
		for (i=0; i<A.n; i++) res.setMainLine(i, WorkWithVectors.multiplyFactor(A.getMainLine(i),k));
		return res;
	}
	
	//���������� ������������ ���������� �������
	public static int determinant(Matrix A){
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
		
		// AP: почему имена аргументов с большой буквы?
	public static Matrix multiplication(Matrix A, Matrix B){
		// (1) A,B (2) At,B 
		// (3) A,Bt (4) At,Bt
		
		Matrix warning=new Matrix(0,0);
		int lineNum=0;
		int colNum=0;
		byte flag;
		if (B.ifTransposed()){
			if (A.ifTransposed()) flag=4;
			else flag=3;
		}
		else{
			if (A.ifTransposed()) flag=2;
			else flag=1;			
		}
		
		// AP: столько ifов - это не объектно-ориетнированно - предложите правильное решение
		// ��������� �������
		if (flag==1){
			if (A.m!=B.n) {
				ErrorHandler.error("Incorrect sizes of the matrices");
				return warning;
			}
			else{
				lineNum=A.n;
				colNum=B.m;
			}
		}
		
		if (flag==2){
			if (A.n!=B.n) {
				ErrorHandler.error("Incorrect sizes of the matrices");
				return warning;
			}
			else{
				lineNum=A.m;
				colNum=B.m;
			}
		}
		
		if (flag==3){
			if (A.m!=B.m) {
				ErrorHandler.error("Incorrect sizes of the matrices");
				return warning;
			}
			else{
				lineNum=A.n;
				colNum=B.n;
			}
		}
		
		if (flag==4){
			if (A.n!=B.m) {
				ErrorHandler.error("Incorrect sizes of the matrices");
				return warning;
			}
			else{
				lineNum=A.m;
				colNum=B.n;
			}
		}
		
		int i;
		Matrix res=new Matrix(lineNum,colNum);
		
		for (i=0; i<lineNum; i++)
			res.setMainLine(i,(Vector)multiplication(A.getLine(i),B));
		
		return res;
	}
	
	//������ ����� �������
	public static Vector multiplication(Matrix A, Vector a){
		if ((a.n!=A.n)&&(a.n!=A.m)){ //error
				ErrorHandler.error("Incorrect sizes of the matrix and vector");
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
	public static Vector multiplication(Vector a, Matrix A){
		if ((a.n!=A.n)&&(a.n!=A.m)){ //error
				ErrorHandler.error("Incorrect sizes of the matrix and vector");
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
	
	
	public static Matrix readFromFile(String path){
		Matrix res=new Matrix(0,0);
		
		try {
			FileInputStream fileStream = new FileInputStream(path);
			Scanner scanner = new Scanner(fileStream);
			
			int n,m,i,j,value;
			n=scanner.nextInt();
			m=scanner.nextInt();
			
			res=new Matrix(n,m);
			
			for (i=0; i<n; i++){
				for (j=0; j<m; j++){
					value=scanner.nextInt();
					res.set(i, j, value);
				}
			}
			
			scanner.close();
		} 
		catch (IOException e1) {
			e1.printStackTrace();
			ErrorHandler.error("Something is wrong with the file...");
		}
		finally
		{
			return res;
		}
	}
}
