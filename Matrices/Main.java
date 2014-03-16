
import java.io.IOException;
import java.io.FileNotFoundException;
import java.lang.ArrayIndexOutOfBoundsException;
import java.util.Scanner;

import ru.mipt.cs.linearAlgebra.ErrorHandler;
import ru.mipt.cs.linearAlgebra.Matrix;
import ru.mipt.cs.linearAlgebra.WorkWithMatrices;

public class Main {

	public static void main(String[] args) {

		System.out.println("Hello! We'll work with vectors and matrices");
		
		//the first matrix for our experiments - non-square
		Matrix first;
		
		System.out.println("The matrix from matrix1.txt:");	
		try{
			first=WorkWithMatrices.readFromFile("matrix1.txt");
		
		
		
		first.println();
		
		Scanner currIn=new Scanner(System.in);
		
		int k;
		System.out.println("Enter the number of line");
		k=currIn.nextInt();
		
		try{
			first.getLine(k).println();
		}
		catch (ArrayIndexOutOfBoundsException e){
			e.printStackTrace();
			ErrorHandler.error("Are you sure that the line really exists? The matrix is above ^_^");
		}
		
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
		Matrix second;
		second=WorkWithMatrices.readFromFile("matrix2.txt");
	
		second.println();
		
		System.out.println("Determinant: "+WorkWithMatrices.determinant(second));
				
		System.out.println("Diagonal:");
		second.println();
		
		System.out.println("The matrices for multiplication:");
		Matrix A,B;
			A=WorkWithMatrices.readFromFile("matrix3.txt");
			B=WorkWithMatrices.readFromFile("matrix4.txt");

		
		A.println();
		System.out.println();
		B.println();
		System.out.println("Result:");
		
		Matrix C=WorkWithMatrices.multiplication(B,A);
		C.println();		
		currIn.close();
		}
		catch (ArrayIndexOutOfBoundsException e){
			e.printStackTrace();
			ErrorHandler.error("Wrong data about sizes of matrices in the file");
		}
		catch (FileNotFoundException e){
			e.printStackTrace();
			ErrorHandler.error("Are you sure that file really exists?");
		}
		catch (IOException e){
			e.printStackTrace();
			ErrorHandler.error("Something is wrong with the file...");
		}
		finally{
			System.out.println("The work of the program is over");
		}
		
		
	}

}
