import missionInterface
import ochWriter
import ochReader

# Set the path to your input file here:
inputData = missionInterface.missionToSegments('/home/crc/Desktop/CAESAR_HighFidelityTest_standard_nothrottlelogic.emtg')
columnsToKeep = ['JulianDate', 'x', 'y', 'z', 'dotx', 'doty', 'dotz', 'mass', 'T_x', 'T_y', 'T_z']

# Set up the output file here:
writer = ochWriter.ochWriter(inputData, "output.och", columnsToKeep)
writer.subtractFromColumn("JulianDate", 2430000.0)
writer.writeFile()

# # This is code for reading in an OCH file; we read it and write a new one.  THe outputs match.
# ochReader = ochReader.ochReader("output.och", ['JulianDate', 'x', 'y', 'z', 'dotx', 'doty', 'dotz', 'mass', 'T_x', 'T_y', 'T_z'])
# writer2 = ochWriter.ochWriter(ochReader.getSegments(), "output2.och", columnsToKeep)
# writer2.writeFile()
