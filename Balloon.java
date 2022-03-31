import java.util.ArrayList;
import java.util.Scanner;

public class Balloon {
	public static Scanner CONSOLE = new Scanner(System.in);

	public static void main(String args[]) {
		arduino1();
	}
	
	public static void arduino2() {
		ArrayList<double[]> pressureData = new ArrayList<double[]>(); // time, pressure
		ArrayList<double[]> solarData = new ArrayList<double[]>(); // time, without, with
		ArrayList<double[]> humidityData = new ArrayList<double[]>(); // time, humidity
		ArrayList<String[]> CO2Data = new ArrayList<String[]>(); // time, CO2 (is a string of "<400ppm")
		
		double time = 0;
		double pressure = 0;
		double solarWithout = 0;
		double solarWith = 0;
		double humidity = 0;
		String CO2;
		
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
		
		System.out.println("Time");
		for (int i = 0; i < pressureData.size(); i++){
			System.out.println(pressureData.get(i)[0]);
		}
		System.out.println("Pressure");
		for (int i = 0; i < pressureData.size(); i++){
			System.out.println(pressureData.get(i)[1]);
		}
		
	}
	
	public static void arduino1() {
		// time in 1st pos, data in 2nd pos
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
		double lat = 0; // in degrees
		double lon = 0; // in degrees

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
//				if (altitude<=-200 || altitude>100) { // Filters out weird values
//					altitude = altitudeData.get(altitudeData.size()-1)[1]; // then just take the previous reading
//				}
				altitudeData.add(new double[]{ time, altitude });

				lat = Double.parseDouble(input.substring(input.indexOf("Lat") + 5, input.indexOf("Long")))/10000000;
				lon = Double.parseDouble(input.substring(input.indexOf("Long") + 6, input.indexOf("(deg")))/10000000;
				positionData.add(new double[]{ time, lat, lon });
			}

		}

		
		for (int i=0; i<positionData.size()-1; i++) {
			double lat1 = positionData.get(i)[1];
			double lat2 = positionData.get(i+1)[1];
			double lon1 = positionData.get(i)[2];
			double lon2 = positionData.get(i+1)[2];
			double el1 = altitudeData.get(i)[1];
			double el2 = altitudeData.get(i+1)[1];
			double distance = distance(lat1, lat2, lon1, lon2, el1, el2);
			double timeElapsed = positionData.get(i+1)[0]-positionData.get(i)[0];
			double timeMid = (positionData.get(i+1)[0]+positionData.get(i)[0])*0.5;
			if (distance/timeElapsed>1000) {
				speedData.add(new double[] {timeMid, speedData.get(i-1)[1]});
			}
			else {
				speedData.add(new double[] {timeMid, distance/timeElapsed});
			}
		}
		
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
	// Returns distance in meters
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
}
