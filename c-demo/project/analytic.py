import csv
import sys

if len(sys.argv) < 1:
    print("You must pass the path to the file name in the arguments")
file = sys.argv[1]
with open(file, "r") as fr:
    reader = csv.reader(fr, delimiter=',')
    print('Latitude,Longitude99')
    for i, row in enumerate(reader):
        # print('entry[{}] = {}'.format(i, row))
        latDeg = row[0][:-7]
        latMin = row[0][-7:]
        latHem = row[1]
        lonDeg = row[2][:-7]
        lonMin = row[2][-7:]
        lonHem = row[3]

        # latDec = float(latDeg) + float(latMin) / 60.0
        # lonDec = float(lonDeg) + float(lonMin) / 60.0
        # if latHem == 'S':
        #     latDec = -1 * latDec
        # if lonHem == 'W':
        #     lonDec = -1 * lonDec
        # print('{},{}'.format(latDec, lonDec))
        if latHem == 'S':
            latDeg = -1 * int(latDeg)
        if lonHem == 'W':
            lonDeg = -1 * int(lonDeg)
        print(latDeg, latMin, lonDeg, lonMin)
        # print('entry[{}] = {}'.format(i, row))
