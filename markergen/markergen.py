#coding=utf-8

import sys

try:
    print("引数:" +  sys.argv[1])
    a = int(sys.argv[1], 16)
except IndexError:
    print("16進数4桁の引数が必要です")
    sys.exit(1)
except ValueError:
    print("引数は4桁の16進数で指定してください")
    sys.exit(1)

with open("marker.svg", "w") as f:
    f.write("<?xml version=\"1.0\"?>\n")
    f.write("<svg xmlns=\"http://www.w3.org/2000/svg\" x=\"0\" y=\"0\" width=\"840\" height=\"1187\" style=\"background-color: white\">\n")
    f.write("<rect x=\"0\" y=\"0\" width=\"840\" height=\"1187\" stroke=\"none\" fill=\"white\" stroke-width=\"1\" />\n")
    f.write("<rect x=\"300\" y=\"474\" width=\"240\" height=\"240\" stroke=\"none\" fill=\"red\" />\n")

    for i in range(4):
        for j in range(4):
            if a % 2 == 0:
                f.write("<rect x=\"{}\" y=\"{}\" width=\"40.5\" height=\"40.5\" \
                        stroke=\"none\" fill=\"white\" />\n".format(460 - 40 * j, 637 - 40 * i))
            a //= 2

    f.write("</svg>\n")
