import java.util.Scanner;

public class Balloon {
	public static Scanner CONSOLE = new Scanner(System.in);
	
	
	// To do: turn the 2D array into an arraylist of (small) arrays

	public static void main(String args[]) {
		
		// Stores time in first position, pressure in second position
		double[][] pressureData = new double[100000][2];
		
		
		// Time in first position, pressure in second, temp in third
		double[][] tempData = new double[100000][3];
		
		
		
		int counterP = 0;
		int counterT = 0;

		double pressure = 0; // Pressure is outside the loop and gets updated periodically
		double time = 0;
		double temp = 0;
		
		while (CONSOLE.hasNextLine()) {
			String input = CONSOLE.nextLine();
			if (input.length()==0) {
				break;
			} // When input is over, break
			
			time = Double.parseDouble(input.substring(input.indexOf("(")+1, input.indexOf(")")))/1000;
			// Find the lines that give pressure data
			if (input.substring(input.indexOf(")")+1, input.indexOf(")")+2).equals("P")){
				double oldPressure=pressure;
				pressure = Double.parseDouble(input.substring(input.indexOf("=")+1));
				if (pressure<=0 || pressure>1500) { // Filters out weird values
					pressure = oldPressure;
				}
				pressureData[counterP][0] = time;
				pressureData[counterP][1] = pressure;
				counterP = counterP+1;
			}
			
			// Find the lines that give the temp data
			if (input.substring(input.indexOf(")")+1, input.indexOf(")")+2).equals("T")){
				temp = Double.parseDouble(input.substring(input.indexOf("=")+1));
				tempData[counterT][0] = time;
				tempData[counterT][1] = pressure;
				tempData[counterT][2] = temp;
				counterT++;
				
			}
			
		}
		
		System.out.println("Time:");
		for (int i=0; i<tempData.length; i++) {
			if (tempData[i][0]!=0) {
				System.out.println(tempData[i][0]);
			}
		}
		
		System.out.println("------");
		System.out.println("Corresponding temperature:");
		for (int i=0; i<tempData.length; i++) {
			if (tempData[i][2]!=0) {
				System.out.println(tempData[i][2]);
			}
		}
		
	}
}
