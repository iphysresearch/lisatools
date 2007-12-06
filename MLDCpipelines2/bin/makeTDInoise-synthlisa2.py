#!/usr/bin/env python

__version__='$Id: $'

import lisaxml2 as lisaxml
import random

import synthlisa
import PseudoRandomNoise

import numpy

import sys
import math

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id: makeTDInoise-synthlisa.py 181 2006-12-01 00:37:11Z vallisneri $")

parser.add_option("-t", "--initialTime",
                  type="float", dest="inittime", default=0.0,
                  help="initial time for TDI observables (s) [default 0.0]")
                                    
parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=31457280.0,
                  help="total time for TDI observables (s) [default 31457280 = 2^21 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-r", "--rawMeasurements",
                  action="store_true", dest="rawMeasurements", default=False,
                  help="output raw phase measurements (y's and z's) instead of TDI X, Y, Z")

parser.add_option("-R", "--randomizeNoise",
                  type="float", dest="randomizeNoise", default=0.0,
                  help="randomize level of noises (e.g., 0.2 for 20% randomization)")

parser.add_option("-l", "--laserNoise",
                  type="string", dest="laserNoise", default='None',
                  help="laser noise level: None, Standard, <numerical value> (e.g., 0.2 for 20% of pm noise at 1 mHz)")

parser.add_option("-k", "--keyOnly",
                  action="store_true", dest="keyOnly", default=False,
                  help="produce key only, don't compute noise [off by default]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# for the moment, support a single input barycentric file

if len(args) < 1:
    parser.error("You must specify an output file!")

outputfile = args[0]

if options.seed == None:
    parser.error("You must specify the seed!")

# set the noise seed
random.seed(options.seed)

# hardcode LISA and Noise to the MLDC standards
lisa = synthlisa.EccentricInclined(0.0,1.5*math.pi,-1)

def randnoise():
    return random.uniform(1.00 - options.randomizeNoise, 1.0 + options.randomizeNoise)

proofnoises = []
for ind in ['pm1', 'pm1s', 'pm2', 'pm2s', 'pm3', 'pm3s']:
    noise = PseudoRandomNoise.PseudoRandomNoise(ind)
    
    noise.SpectralType = 'PinkAcceleration'
    noise.PowerSpectralDensity = 2.5e-48 * randnoise()
    noise.Fknee = 1e-4
    noise.GenerationTimeStep = 100
    noise.InterpolationOrder = 2
    noise.PseudoRandomSeed = random.randint(0,2**30)
    
    proofnoises.append(noise)

if options.laserNoise == 'None':
    laserpsd = 0.0
elif options.laserNoise == 'Standard':
    laserpsd = 1.1e-26
else:
    laserpsd = float(options.laserNoise) * 2.5e-48 * (1e-3)**-2

lasernoises = []
for ind in ['c1', 'c1s', 'c2', 'c2s', 'c3', 'c3s']:
    noise = PseudoRandomNoise.PseudoRandomNoise(ind)

    noise.SpectralType = 'WhiteFrequency'
    noise.PowerSpectralDensity = laserpsd * randnoise()
    noise.GenerationTimeStep = 15
    noise.InterpolationOrder = 2
    noise.PseudoRandomSeed = random.randint(0,2**30)

    lasernoises.append(noise)

shotnoises = []
for ind in ['pd1', 'pd1s', 'pd2', 'pd2s', 'pd3', 'pd3s']:
    noise = PseudoRandomNoise.PseudoRandomNoise(ind)

    noise.SpectralType = 'WhitePhase'
    noise.PowerSpectralDensity = 1.8e-37 * randnoise()
    noise.GenerationTimeStep = 15
    noise.InterpolationOrder = 4
    noise.PseudoRandomSeed = random.randint(0,2**30)

    shotnoises.append(noise)

# pm1, pm2, pm3, pm1s, pm2s, pm3s for the two proof masses on each bench
# pd1, pd2, pd3, pd1s, pd2s, pd3s for photodetector (i.e., shot) noise
# c1, c1s, c2, c2s, c3, c3s for the laser noises

# the synthlisa ordering is the same, except for shot noises, where it is
# {132,231,213,312,321,123} = {pd2s, pd1, pd3s, pd2, pd3, pd1s}

tdiobs = None

if not options.keyOnly:
    proofnoise  = [n.synthesize() for n in proofnoises]
    shotnoise   = [shotnoises[i].synthesize() for i in (3,0,5,2,1,4)]
    lasernoise  = [n.synthesize() for n in lasernoises]

    tdi = synthlisa.TDInoise(lisa,proofnoise,shotnoise,lasernoise)

    samples = int( options.duration / options.timestep + 0.1 )

    if options.rawMeasurements:    
        [t,y123,y231,y312,y321,y132,y213,
           z123,z231,z312,z321,z132,z213] = numpy.transpose(synthlisa.getobsc(samples,options.timestep,
                                                                              [tdi.t,tdi.y123,tdi.y231,tdi.y312,tdi.y321,tdi.y132,tdi.y213,
                                                                                     tdi.z123,tdi.z231,tdi.z312,tdi.z321,tdi.z132,tdi.z213],
                                                                              options.inittime))    

        tdiobs = lisaxml.Observable('t,y123f,y231f,y312f,y321f,y132f,y213f,z123f,z231f,z312f,z321f,z132f,z213f')     
        tdiobs.TimeSeries = lisaxml.TimeSeries([t,y123,y231,y312,y321,y132,y213,
                                                  z123,z231,z312,z321,z132,z213],'t,y123f,y231f,y312f,y321f,y132f,y213f,z123f,z231f,z312f,z321f,z132f,z213f')    

    else:
        [t,X,Y,Z] = numpy.transpose(synthlisa.getobsc(samples,options.timestep,
                                                      [tdi.t,tdi.Xm,tdi.Ym,tdi.Zm],
                                                      options.inittime))
        tdiobs = lisaxml.Observable('t,Xf,Yf,Zf')
        tdiobs.TimeSeries = lisaxml.TimeSeries([t,X,Y,Z],'t,Xf,Yf,Zf')

    tdiobs.DataType = 'FractionalFrequency'

    tdiobs.TimeSeries.Cadence = options.timestep
    tdiobs.TimeSeries.TimeOffset = options.inittime

outputXML = lisaxml.lisaXML(outputfile,'w')

# save the standard LISA...
lisa = lisaxml.LISA('Standard MLDC PseudoLISA')
lisa.TimeOffset      = 0; lisa.TimeOffset_Unit      = 'Second'
lisa.InitialPosition = 0; lisa.InitialPosition_Unit = 'Radian'
lisa.InitialRotation = 0; lisa.InitialRotation_Unit = 'Radian'
lisa.Armlength = 16.6782; lisa.Armlength_Unit       = 'Second'

outputXML.LISAData.append(lisa)

for noise in proofnoises:
    outputXML.NoiseData.append(noise)
for noise in shotnoises:
    outputXML.NoiseData.append(noise)
for noise in lasernoises:
    outputXML.NoiseData.append(noise)

if tdiobs:
    outputXML.TDIData(tdiobs)

outputXML.close()

sys.exit(0)