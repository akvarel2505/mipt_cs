import java.util.Scanner;
public class Main {

	public static void main(String[] args) {

		System.out.println("Hello! We'll work with vectors and matrices");
		
		//the first matrix for our experiments - non-square
		System.out.println("The matrix from matrix1.txt:");	
		// AP: здесь и далее скорее всего у меня другой путь к фацлу - и работать не будет - сделайте универсально
		Matrix first=WorkWithMatrices.readFromFile("C:\\Users\\������������\\Documents\\Eclipse Projects\\Matrices\\bin\\matrix1.txt");
		first.println();
		
		Scanner currIn=new Scanner(System.in);
		
		int k;
		System.out.println("Enter the number of line");
		k=currIn.nextInt();
		first.getLine(k).println();
		
		System.out.println("Transposed:");
		first.transpose();
		first.println();
		System.out.println("Line with the same number in transposed matrix:");
		first.getLine(k).println();
		
		first.transpose();
		first.toDiagonal();
		first.println();
		
		//the second matrix for our experiments - square
				
		
		System.out.println("The matrix from matrix2.txt:");		
		Matrix second=WorkWithMatrices.readFromFile("C:\\Users\\������������\\Documents\\Eclipse Projects\\Matrices\\bin\\matrix2.txt");
		second.println();
	
		System.out.println("Diagonal (if possible):");
		second.toDiagonal();
		second.println();
		
		System.out.println("Determinant: "+WorkWithMatrices.determinant(second));
		
		
		System.out.println("The matrices for multiplication:");		
		Matrix A=WorkWithMatrices.readFromFile("C:\\Users\\������������\\Documents\\Eclipse Projects\\Matrices\\bin\\matrix3.txt");
		Matrix B=WorkWithMatrices.readFromFile("C:\\Users\\������������\\Documents\\Eclipse Projects\\Matrices\\bin\\matrix4.txt");
		A.println();
		System.out.println();
		B.println();
		System.out.println("Result:");
		
		Matrix C=WorkWithMatrices.multiplication(B,A);
		C.println();		
		
		currIn.close();
		System.out.println("The work of the program is over");
	}

}
