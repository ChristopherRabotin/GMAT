################################################
#  TEST DRIVER: CLIENT -- gmat-sync-mjd.py     #
#  Author -- Michael Stark                     #
#  September 30, 2019                          #
################################################

# modified JD as time tags
# propagation with 0.025 sec stepsize and 0.001 sec timeout

import AttitudeInterface as AI
from math import degrees

# used to delay until time to process next step in synchronization with propagation stepsize
import time as clock
#import pause
from time import sleep

def Run():
    local = input ("Enter 'y' to use localhost option >")
    if local == 'y':
        serverName = 'localhost'
        print ("Client using 'localhost' as server name")
    else:
        serverName = 'GSSLA17010743'
        print ("Client setting server name to ",serverName)

    port = 5000.0
    # using float for port tests cast to integer in
    #AttitudeInterface.Initialize() call to initialize receiver

    secPerDay = 86400.0

    ################ CHANGE TIMEOUT & STEPSIZE HERE FOR TESTS
    timeout = 0.001
    t  = 0 # need to settle on time representation
    dt = 0.025 # models step size in GMAT propagation script

    print ("Initializing AttitudeInterface")
    AI.Initialize(serverName,port,timeout)
    print ("Initialization complete")
    print()

    # expectation: initialize will get Read() to acquire mutex & block until
    # message is available
    
    AI.Read()
    timeTag = AI.StateT()
    print ("Time of first record = ",timeTag)
    
    # simulate time needed to propagate orbit 1-2 days
    for I in  range(0, 10):
        print (10-I)
        sleep (1.0)
    print ("Orbit has been propagated to initial time ",timeTag)
    
    # acknowledge initialization so that server will end its wait
    AI.Write("Ack Initialization")
    
    # start clock & compute time of next step
    now = clock.time()
    next = now + dt
    print ("Now  =",now)
    print ("Next =",next)
    dtInDays = dt/secPerDay

    while not AI.TimeToQuit():
        print ("Time tag = ",timeTag," et=", t," calling GetRPY" )
        [roll,pitch, yaw] = AI.GetRPY(timeTag)
        [wx,wy,wz] = AI.GetW(timeTag)
        r = degrees(roll)
        p = degrees(pitch)
        y = degrees(yaw)
        wxd = degrees(wx)
        wyd = degrees(wy)
        wzd = degrees(wz)
        if AI.NewData():
            print ("************** UPDATING STATE *************")
            t0 = AI.StateT()
            [roll,pitch,yaw] = AI.StateRPY()
            r0 = degrees(roll)
            p0 = degrees(pitch)
            y0 = degrees(yaw)
            print ("**** Dynamic New data t0=", t0, " et=",t,
                " rpy= [", r0,p0,y0,"]")
    
        line = "{0:13.7f} {1:9.4f} {2:9.4f} {3:9.4f} {4:9.4f} {5:9.4f} {6:9.4f}"
        print (line.format(timeTag,r,p,y,wxd,wyd,wzd))

        now = clock.time()
        sleep(next-now)
        AI.Read()
        timeTag = timeTag + dt / secPerDay
        next = next + dt
        t = t+dt

    # end while

    print()
    print("Ack end of data")
    AI.Write("Ack end of data")
    print ("Finalizing Attitude Interface")
    AI.Finalize()
    print ("Finalization complete")

# end Run()

################ MAIN ######################
if __name__ == "__main__":
   #AI.SetDebug()
   Run()

