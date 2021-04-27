import segment
import csv

class ochWriter:
	def __init__(self, segments, outputFile, columnsToKeep):
		self.segments = segments
		self.outputFile = outputFile
		self.columnsToKeep = columnsToKeep


	def writeSegment(self, segment, writer):
		# Write out metadata.
		writer.write('META_START\n')
		writer.write('	CENTRAL_BODY	= ' + segment.centralBody + '\n')
		writer.write('	REF_FRAME		= ' + segment.refFrame + '\n')
		writer.write('	TIME_SYSTEM		= ' + segment.timeSystem + '\n')
		writer.write('	NUM_STATES		= ' + str(segment.numStates) + '\n')
		writer.write('	NUM_CONTROLS	= ' + str(segment.numControls) + '\n')
		writer.write('	NUM_INTEGRALS	= ' + str(segment.numIntegrals) + '\n')
		writer.write('META_STOP\n')
		writer.write('\n')
		writer.write('DATA_START\n')

		# Filter data down to the relevant columns.
		# NOTE: the ColumnsToKeep filtering is currently redundant due to missionInterface 
		# 	already narrowing down EMTG input, but has been left in in case we need that 
		# 	functionality later.
		data = segment.getDataToKeep(self.columnsToKeep)

		# Concatenate data lists into output strings and write them out.
		for row in data:
			outputLine = "\t"
			for value in row:
				if not isinstance(value, str):
					value = str(value)
				value = value.ljust(27)
				outputLine = outputLine + value
			outputLine = outputLine + '\n'
			writer.write(outputLine)

		writer.write('DATA_STOP\n')
		writer.write('\n')

	def writeFile(self):
		writer = open(self.outputFile, 'w')
		writer.write('Written by PLACEHOLDER\n')
		writer.write('\n')
		for segment in self.segments:
			self.writeSegment(segment, writer)

	def subtractFromColumn(self, column, value):
		for seg in self.segments:
			seg.subtractFromColumn(column, value)







