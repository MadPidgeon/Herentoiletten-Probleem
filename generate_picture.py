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
c = ["/.style={fill=red!25}","/.style={fill=green!25}"]
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

with open( "output.csv", "r") as datain:
    with open( "output.tex", "w" ) as dataout:
        n = 0
        data = []

        dataout.write(texA)
        for line in datain:
            data.append([])
            for x in line.split():
                data[n].append(int(x))
            n+=1
        print( data )

        for row in range(1,n+1):
            for cell in range(1,row+1):
                dataout.write( "cell " + str(n-row+cell) + "-" + str(cell) + c[(data[row-1][cell-1]+1)/2] + ",\n" )


        dataout.write(texB)
        for i in range(1,n+1):
            if i != 1:
                dataout.write(",%\n")
            dataout.write("{")
            for j in range(0,i-1):
                dataout.write( str(j) + "," )
            dataout.write( str( i - 1) + "}" )
        dataout.write(texC)
