# Balloon-Launch
Arduino code for the balloon launch. For ASR 2021-2022.

The files that ran during the space launch are: Balloon_arduino_1.ino, Balloon_arduino_3.ino, and the folder Arduino 2. Below are brief descriptions of each of the files/folders.

Folder "Arduino_2" contains all the files for Arduino 2. There are a lot of these because there are a lot of individual projects, but the main file is actually Arduino_2.ino, which calls all the other ones.

Folder "Individual_files" contains the code for the individual experiments in stand-alone files. These should all work because they came from the people themselves.

File "Balloon.java" was used after the launch to process the raw output into a form that can be copy-pasted into a spreadsheet.

FIle "Balloon_arduino_1.ino" contains the code that ran on Arduino 1. Self-explanatory.

File "Balloon_arduino_3.ino" contains the code that ran on Arduino 3. Note that no SD card was attached to Arduino 3 (since it does not collect any meaningful data) -- the "Serial.print" statements were just for debugging.

File "Balloon_launch_OLD.ino" contains the code which was used for the tethered launch, before we decided to split into different Arduinos. It is believed to be bug-free (although it is pretty complicated).

File "Emory_Ozone.ino" contains Emory's code for her ozone sensor, which ran independently of everything else.
