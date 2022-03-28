import java.util.ArrayList;
import java.util.Scanner;

public class Balloon {
	public static Scanner CONSOLE = new Scanner(System.in);

	public static void main(String args[]) {
		
		// time in 1st pos, data in 2nd pos
		ArrayList<double[]> pressureData = new ArrayList<double[]>(); 
		ArrayList<double[]> tempData = new ArrayList<double[]>();
		ArrayList<double[]> altitudeData = new ArrayList<double[]>();
		ArrayList<double[]> positionData = new ArrayList<double[]>(); // time, lat, long

		// These are outside the loop and get updated periodically
		double time = 0; // in sec
		double pressure = 0; // in mbar
		double temp = 0; // in deg C
		double altitude = 0; // in m
		double lat = 0; // in degrees
		double lon = 0; // in degrees
		
		while (CONSOLE.hasNextLine()) {
			String input = CONSOLE.nextLine();
			if (input.length()==0) {
				break;
			} // Output being over is indicated with a blank line, in which case we say bye
			
			time = Double.parseDouble(input.substring(input.indexOf("(")+1, input.indexOf(")")))/1000;
			
			// Find the lines that give pressure data
			if (input.substring(input.indexOf(")")+1, input.indexOf(")")+2).equals("P")){
				pressure = Double.parseDouble(input.substring(input.indexOf("=")+1));
				if (pressure<=0 || pressure>1500) { // Filters out weird values
					pressure = pressureData.get(pressureData.size()-1)[1]; // then just take the previous reading
				}
				pressureData.add(new double[] {time, pressure});
			}
			
			// Find the lines that give the temp data
			if (input.substring(input.indexOf(")")+1, input.indexOf(")")+2).equals("T")){
				temp = Double.parseDouble(input.substring(input.indexOf("=")+1));
				if (temp<=-200 || temp>100) { // Filters out weird values
					temp = tempData.get(tempData.size()-1)[1]; // then just take the previous reading
				}
				tempData.add(new double[] {time, temp});
				
			}
			
			// Find the lines with GPS data
			if (input.substring(input.indexOf(")")+1, input.indexOf(")")+2).equals("L")){
				altitude = Double.parseDouble(input.substring(input.indexOf("Alt")+5, input.indexOf("(mm)")))/1000;
//				if (altitude<=-200 || altitude>100) { // Filters out weird values
//					altitude = altitudeData.get(altitudeData.size()-1)[1]; // then just take the previous reading
//				}
				altitudeData.add(new double[] {time, altitude});
				
				lat = Double.parseDouble(input.substring(input.indexOf("Lat")+5, input.indexOf("Long")))/10000000;
				lon = Double.parseDouble(input.substring(input.indexOf("Long")+6, input.indexOf("(deg")))/10000000;
				positionData.add(new double[] {time, lat, lon});
			}
			
			
		}
		
		System.out.println("Lats");
		for (int i=0; i<5000; i++) {
			System.out.println(positionData.get(i)[1]);
		}
		System.out.println("Longs");
		for (int i=0; i<5000; i++) {
			System.out.println(positionData.get(i)[2]);
		}
		
	}
}
