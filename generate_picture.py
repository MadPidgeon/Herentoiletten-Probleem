import sys

texA = """\
\\documentclass[tikz]{standalone}
\\usetikzlibrary{shapes.geometric}
\\begin{document}
\\begin{tikzpicture}[
    rotate=-45,
    every label/.append style={text depth=+0pt},
    label position=center,
"""
texB = """\
    ]
\\foreach \\jRow[count=\\jCount from 1, remember=\\mCount] in {%
"""
c = ["/.style={fill=red!25}","/.style={fill=blue!25}","/.style={fill=green!25}"]
texC = """%
    } {
    \\foreach \\mCell[count=\\mCount from 1, remember=\\mCount] in \\jRow {
        \\node[
            diamond,
            minimum size=1.414cm+0.4\\pgflinewidth,
            draw,
            every cell/.try,
            row \\jCount/.try,
            column \\mCount/.try,
            cell \\jCount-\\mCount/.try,
            label={\\pgfmathprintnumber{\\mCell}},
            alias=@lastnode,
            alias=@lastrow-\\mCount
        ] at (\\mCount-.5,\\jCount-.5) {};
    }
    }
    \\foreach \\mCountExtra in {1,...,\\mCount}
        \\path [late options={name=@lastrow-\\mCountExtra, label=above right:$\\mCountExtra$}];
\\end{tikzpicture}
\\end{document}
"""

if( len( sys.argv ) != 4 ):
    print "usage: ./generate_picture.py input output (-n|-s)" 
    exit()

with open( sys.argv[1], "r") as datain:
    with open( sys.argv[2], "w" ) as dataout:
        n = 0
        data = []

        dataout.write(texA)

        if( sys.argv[3] == "-n" ):
            for line in datain:
                data.append([])
                for x in line.split():
                    data[n].append(int(x))
                n+=1
            print( data )
        else:
            for line in datain:
                l = line.split()
                while( len(data) < int(l[0]) ):
                    data.append([])
                if( len( data[int(l[0])-1] ) == 0 ):
                    data[int(l[0])-1] = list( 0 for i in range(0,int(l[0])))
                data[int(l[0])-1][int(l[1])] = int(l[2])
            n = len(data)

        for row in range(1,n+1):
            for cell in range(1,row+1):
                dataout.write( "cell " + str(n-row+cell) + "-" + str(cell) + c[data[row-1][cell-1]+1] + ",\n" )


        dataout.write(texB)
        for i in range(1,n+1):
            if i != 1:
                dataout.write(",%\n")
            dataout.write("{")
            for j in range(0,i-1):
                dataout.write( str(j) + "," )
            dataout.write( str( i - 1) + "}" )
        dataout.write(texC)
