#!/usr/bin/env python

import re
import numpy.linalg

def load_data(fname):
  f = open(fname)
  data=[]
  for line in f:
    fields = re.split("\s+",line)
    data.append([float(fields[0]), float(fields[1])])
  f.close()
  return data

def train(data, bottom_power = 0):
  size = len(data)
  model = [[],[]]
  for i in xrange(size):
    power = 0
    value = 1.0
    row = []
    while power < size - bottom_power:
      row.append(value)
      value *= data[i][0] # x_i
      power += 1

    power = 1
    value = -data[i][1] * data[i][0]
    while power <= bottom_power:
      row.append(value)
      value *= data[i][0] # x_i
      power += 1

    model[0].append(row)
    model[1].append(data[i][1])
  coefs = numpy.linalg.solve(model[0], model[1])
  model.append(coefs)
  model.append(bottom_power)
  return model

def calc(model, x):
  bottom_power = model[3]
  size = len(model[2])
  
  power = 0
  value = 1.0
  result = 0.0
  while power < size - bottom_power:
    result += value * model[2][power]
    value *= x
    power += 1
  
  power = 1
  value = x
  devider =1.0
  while power <= bottom_power:
    devider += value * model[2][size - bottom_power - 1 + power]
    value *= x
    power += 1

  return result/devider

if __name__ == '__main__':
  print "Hello world!"
  data = load_data("infile.txt")
  print data
  m = train(data, len(data) / 2)
  print m
  
  for i in xrange(len(data)):
    print calc(m, data[i][0])
