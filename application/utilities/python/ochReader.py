import segment
import csv

class ochReader:
	def __init__(self, inputFile, colNames):
		self.inputFile = inputFile
		self.segments = []

		file = open(inputFile, 'r')

		centralBody = ''
		refFrame = ''
		timeSystem = ''
		numStates = -1
		numControls = -1
		numIntegrals = -1
		data = []
		metaFlag = False
		dataFlag = False

		for line in file:
			line = line.strip()
			if line == "META_START":
				metaFlag = True
				continue

			if line == "META_STOP":
				metaFlag = False
				continue

			if line == "DATA_START":
				dataFlag = True
				continue

			if line == "DATA_STOP":
				dataFlag = False
				seg = segment.segment(centralBody, refFrame, timeSystem, numStates, numControls, numIntegrals, data, colNames)
				self.segments.append(seg)
				centralBody = ''
				refFrame = ''
				timeSystem = ''
				numStates = -1
				numControls = -1
				numIntegrals = -1
				data = []
				continue

			if metaFlag:
				if line.split()[0] == 'CENTRAL_BODY':
					centralBody = line.split()[2]
				elif line.split()[0] == 'REF_FRAME':
					refFrame = line.split()[2]
				elif line.split()[0] == 'TIME_SYSTEM':
					timeSystem = line.split()[2]
				elif line.split()[0] == 'NUM_STATES':
					numStates = int(line.split()[2])
				elif line.split()[0] == 'NUM_CONTROLS':
					numControls = int(line.split()[2])
				elif line.split()[0] == 'NUM_INTEGRALS':
					numIntegrals = int(line.split()[2])
				else:
					continue

			if dataFlag:
				data.append(line.split())

	def getSegments(self):
		return self.segments