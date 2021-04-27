# centralBody, refFrame, timeSystem: Strings containing metadata
# numStates, numControls, numIntegrals: Ints containing metadata
# data: List of lists. Each internal list contains a row of file data.
# colNames: Names for each of the columns in data, in order
class segment:
	def __init__(self, centralBody, refFrame, timeSystem, numStates, numControls, numIntegrals, data, colNames):
		# NOTE: Error checks not yet in place. Following is a commented out example.
		#validBodies = ['Sun', 'Earth', 'PLACEHOLDER']
		#if centralBody not in validBodies:
		#	raise ValueError('Invalid Central Body: ' + centralBody)
		self.centralBody = centralBody
		self.refFrame = refFrame
		self.timeSystem = timeSystem
		self.numStates = numStates
		self.numControls = numControls
		self.numIntegrals = numIntegrals
		self.data = data
		self.colNames = colNames

	def subtractFromColumn(self, column, value):
		# Column may be passed in as a numeric index or a column name. If it is a column name, convert to a numeric index.
		index = -1
		if isinstance(column, int):
			index = column
		elif isinstance(column, str):
			index = self.colNames.index(column)

		for row in self.data:
			#Ensure that the column holds numeric values
			try:
				float(row[index])
			except ValueError:
				print("Error: Values in column " + str(index) + " are not numeric.")
				return

			#Subtract value
			row[index] = str(row[index] - value)

	def getDataToKeep(self, columnsToKeep):
		indexesToKeep = []

		# Convert column names to keep to numeric indices
		for column in columnsToKeep:
			if column in self.colNames:
				indexesToKeep.append(self.colNames.index(column))
			else:
				print('Error: Column ' + column + ' not found in segment.')
				return

		dataToKeep = []

		# Populate dataToKeep with rows containing only the relevant columns.
		for row in self.data:
			newRow = []
			for i in indexesToKeep:
				newRow.append(row[i])
			dataToKeep.append(newRow)

		return dataToKeep