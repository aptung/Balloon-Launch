import java.util.Scanner;

public class Balloon {
	public static Scanner CONSOLE = new Scanner(System.in);
	
	// Stores time in first position, pressure in second position
	public static double[][] pressureData = new double[1000000][2]; 
	

	public static void main(String args[]) {
		int counter = 0;
		while (CONSOLE.hasNextLine()) {
			String input = CONSOLE.nextLine();
			// Find the lines that give pressure data
			if (input.substring(input.indexOf(")")+1, input.indexOf(")")+2).equals("P")){
				double time = Double.parseDouble(input.substring(input.indexOf("(")+1, input.indexOf(")")))/1000;
				double pressure = Double.parseDouble(input.substring(input.indexOf("=")+1));
				pressureData[counter][0] = time;
				pressureData[counter][1] = pressure;
				System.out.println(time);
				System.out.println(pressure);
				counter++;
				
			}
		}
	}
}
