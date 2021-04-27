import Mission
import segment

def missionToSegments(inputFile):
	miss = Mission.Mission(inputFile)
	segments = []
	for journey in miss.Journeys:
		data = []
		for event in journey.missionevents:
			line = [event.JulianDate, event.SpacecraftState[0], event.SpacecraftState[1], event.SpacecraftState[2], 
				event.SpacecraftState[3], event.SpacecraftState[4], event.SpacecraftState[5], event.Mass, event.Thrust[0], 
				event.Thrust[1], event.Thrust[2]]
			data.append(line)

		colNames = 'JulianDate', 'x', 'y', 'z', 'dotx', 'doty', 'dotz', 'mass', 'T_x', 'T_y', 'T_z'
		seg = segment.segment(journey.central_body, "EME2000", "UTCModJulian", 7, 3, 0, data, colNames)
		segments.append(seg)

	return segments