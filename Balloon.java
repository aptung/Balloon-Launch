// Program for converting raw data from the Arduino into a useable format for data analysis.
// Note that the program does not conduct the main analysis (which was done in a spreadsheet),
// except for calculations of ground speed.

import java.util.ArrayList; // For ArrayLists
import java.util.Scanner; // Used for reading in input

public class Balloon {
	public static Scanner CONSOLE = new Scanner(System.in); // For reading in input

	public static void main(String args[]) {
		arduino1(); // Replace this with "arduino2();" if reading in Arduino 2's output
	}
	
	// Reads in data in the form of the output of Arduino 1 (make sure to delete the last, incomplete line in the file)
	// Prints out data in a useable format
	public static void arduino1() {
		// time in 1st position, data in 2nd position of double[]
		ArrayList<double[]> pressureData = new ArrayList<double[]>();
		ArrayList<double[]> tempData = new ArrayList<double[]>();
		ArrayList<double[]> altitudeData = new ArrayList<double[]>();
		ArrayList<double[]> positionData = new ArrayList<double[]>(); // time, lat, long
		
		ArrayList<double[]> speedData = new ArrayList<double[]>(); // time, speed (m/s)

		// These are outside the loop and get updated periodically
		double time = 0; // in sec
		double pressure = 0; // in mbar
		double temp = 0; // in deg C
		double altitude = 0; // in m
		double lat = 0; // in degrees (decimal, not minutes/seconds)
		double lon = 0; // in degrees (same as above, can be negative)
		
		// Step through each line of the input
		while (CONSOLE.hasNextLine()) {
			String input = CONSOLE.nextLine();
			if (input.length() == 0) {
				break;
			} // Output being over is indicated with a blank line, in which case we say bye

			time = Double.parseDouble(input.substring(input.indexOf("(") + 1, input.indexOf(")")))/1000;

			// Find the lines that give pressure data
			if (input.substring(input.indexOf(")") + 1, input.indexOf(")") + 2).equals("P")) {
				pressure = Double.parseDouble(input.substring(input.indexOf("=") + 1));
				if (pressure <= 0 || pressure > 1500) { // Filters out weird values
					pressure = pressureData.get(pressureData.size() - 1)[1]; // then just take the previous reading

				}
				pressureData.add(new double[] { time, pressure });
			}

			// Find the lines that give the temp data
			if (input.substring(input.indexOf(")") + 1, input.indexOf(")") + 2).equals("T")) {
				temp = Double.parseDouble(input.substring(input.indexOf("=") + 1));
				if (temp <= -200 || temp > 100) { // Filters out weird values
					temp = tempData.get(tempData.size() - 1)[1]; // then just take the previous reading

				}
				tempData.add(new double[]{ time, temp });

			}

			// Find the lines with GPS data
			if (input.substring(input.indexOf(")") + 1, input.indexOf(")") + 2).equals("L")) {
				altitude = Double.parseDouble(input.substring(input.indexOf("Alt") + 5, input.indexOf("(mm)")))/1000;
				altitudeData.add(new double[]{ time, altitude });

				lat = Double.parseDouble(input.substring(input.indexOf("Lat") + 5, input.indexOf("Long")))/10000000;
				lon = Double.parseDouble(input.substring(input.indexOf("Long") + 6, input.indexOf("(deg")))/10000000;
				positionData.add(new double[]{ time, lat, lon });
			}

		}

		// Stuff to calculate the distance between consecutive GPS readings
		for (int i=0; i<positionData.size()-1; i++) {
			double lat1 = positionData.get(i)[1];
			double lat2 = positionData.get(i+1)[1];
			double lon1 = positionData.get(i)[2];
			double lon2 = positionData.get(i+1)[2];
			double el1 = altitudeData.get(i)[1];
			double el2 = altitudeData.get(i+1)[1];
			double distance = distance(lat1, lat2, lon1, lon2, el1, el2);
			double timeElapsed = positionData.get(i+1)[0]-positionData.get(i)[0];
			// The speed is calculated based on the position at two points in time, 
			// so we use the halfway point between those two times for the "time" coordinate of the speed
			double timeMid = (positionData.get(i+1)[0]+positionData.get(i)[0])*0.5;
			if (distance/timeElapsed>1000) {
				speedData.add(new double[] {timeMid, speedData.get(i-1)[1]});
			}
			else {
				speedData.add(new double[] {timeMid, distance/timeElapsed});
			}
		}
		
		// Print statements can be modified as necessary to extract the relevant data
		System.out.println("Time (s)");
		for (int i = 0; i < speedData.size(); i++){
			System.out.println(speedData.get(i)[0]);
		}
		System.out.println("Speed (m/s)");
		for (int i = 0; i < speedData.size(); i++){
			System.out.println(speedData.get(i)[1]);
		}
	}
	
	
	// Most upvoted answer here https://stackoverflow.com/questions/3694380/calculating-distance-between-two-points-using-latitude-longitude
	// Uses Haversine method
	// Returns distance in meters between points (lat1, lon1, el1) and (lat2, lon2, el2)
	public static double distance(double lat1, double lat2, double lon1,
	        double lon2, double el1, double el2) {

	    final int R = 6371; // Radius of the earth

	    double latDistance = Math.toRadians(lat2 - lat1);
	    double lonDistance = Math.toRadians(lon2 - lon1);
	    double a = Math.sin(latDistance / 2) * Math.sin(latDistance / 2)
	            + Math.cos(Math.toRadians(lat1)) * Math.cos(Math.toRadians(lat2))
	            * Math.sin(lonDistance / 2) * Math.sin(lonDistance / 2);
	    double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
	    double distance = R * c * 1000; // convert to meters

	    double height = el1 - el2;

	    distance = Math.pow(distance, 2) + Math.pow(height, 2);

	    return Math.sqrt(distance);
	}
	
	// Reads in the output from Arduino 2 (delete the first line from the raw output)
	public static void arduino2() {
		ArrayList<double[]> pressureData = new ArrayList<double[]>(); // time, pressure
		ArrayList<double[]> solarData = new ArrayList<double[]>(); // time, without, with
		ArrayList<double[]> humidityData = new ArrayList<double[]>(); // time, humidity
		ArrayList<String[]> CO2Data = new ArrayList<String[]>(); // time, CO2 (is a string of "<400ppm")
		
		double time = 0; // s
		double pressure = 0; // mbar
		double solarWithout = 0;
		double solarWith = 0;
		double humidity = 0; // in %
		String CO2; // string indicating the CO2 level (all of which happen to be "<400ppm")
		
		// Same structure as the arduino1() method
		while (CONSOLE.hasNextLine()) {
			String input = CONSOLE.nextLine();
			if (input.length()==0) {
				break;
			}
			time = Double.parseDouble(input.substring(input.indexOf("(") + 1, input.indexOf(")")))/1000;
			
			// Find the lines that give pressure data
			if (input.substring(input.indexOf(")") + 1, input.indexOf(")") + 2).equals("P")) {
				pressure = Double.parseDouble(input.substring(input.indexOf("=") + 1));
				pressure = pressure*1.00249 - 9.96392; // uncalibrate the pressure >:(
				if (pressure <= 0 || pressure > 1500) { // Filters out weird values
					pressure = pressureData.get(pressureData.size() - 1)[1]; // then just take the previous reading
				}
				pressureData.add(new double[] { time, pressure });
			}
			
			// Find lines with solar data
			if (input.substring(input.indexOf(")") + 1, input.indexOf(")") + 6).equals("Witho")) {
				solarWithout = Double.parseDouble(input.substring(input.indexOf(":") + 1));
			}
			if (input.substring(input.indexOf(")") + 1, input.indexOf(")") + 6).equals("With ")) {
				solarWith = Double.parseDouble(input.substring(input.indexOf(":") + 1));
				solarData.add(new double[] { time, solarWithout, solarWith });
			}
			
			// Humidity data
			if (input.substring(input.indexOf(")") + 1, input.indexOf(")") + 2).equals("H")) {
				humidity = Double.parseDouble(input.substring(input.indexOf(":") + 1));
				humidityData.add(new double[] { time, humidity });
			}
			
			// CO2 data
			if (input.substring(input.indexOf(")") + 1, input.indexOf(")") + 2).equals("C")) {
				CO2 = input.substring(input.indexOf(":") + 1);
				CO2Data.add(new String[] { String.valueOf(time), CO2 });
			}
		}
		
		// Output can be modified as necessary
		System.out.println("Time");
		for (int i = 0; i < pressureData.size(); i++){
			System.out.println(pressureData.get(i)[0]);
		}
		System.out.println("Pressure");
		for (int i = 0; i < pressureData.size(); i++){
			System.out.println(pressureData.get(i)[1]);
		}
		
	}
	
	
}
