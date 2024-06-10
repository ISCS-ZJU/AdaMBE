#!/usr/bin/env python
# coding=utf-8
#! /usr/bin/env python


from zplot import *
import math
from pathlib import Path
sys.path[0] = str(Path(sys.path[0]).parent) 
from zplot_config import *


styles = [
    ['lightcoral',        'circle',False],
    ['darkseagreen',      'square',False],
    ['lightblue',         'triangle',False],
    ['thistle',        'plusline',False],
    ['blue',         'square',True],
    ['blue',         'square',False],
    ['red',         'circle',True],
    ['red',         'circle',False],
]

# xlabelTextSize = 8.5
# ylabelTextSize = 10
# xtitleTextSize = 10
# ytitleTextSize = 10
# legendTextSize = 10
# NormalizedTextSize = 4

ctype = 'pdf' if len(sys.argv) < 2 else sys.argv[1]
c = canvas(ctype, 'fig-13', dimensions=[400, 300])

t = table(file='fig-13.data')

# print(min_value)

d = drawable(canvas=c, xrange=[-0.6, t.getmax('rownumber')+0.6],
             yrange=[8, 500000], dimensions=[pictureWidthShort*0.8, pictureHeight], coord=[40,30], yscale='log10')


axis(drawable=d, style='box', ticstyle='in', dominortics=False,
     xminorticcnt=0, doxminortics=False, yminorticcnt=0,doxmajortics=True,
     # xtitle='Dataset', 
     ytitle='Running time (s)',
     xlabelshift=[0,0],
     xlabelfontsize=xlabelTextSize,
     ylabelfontsize=ylabelTextSize,
     xtitlesize=xtitleTextSize,
     xaxisposition=8,
     xlabelrotate=0,
     ytitlesize=ytitleTextSize,
     linewidth=0.8, #yauto=['', '', 4],
     ymanual=[['10',10],['100', 100],['1k',1000],['10k',10000],['100k',100000]],
     xmanual=[['LJ10', 0],['LJ20', 1], ['LJ30', 2],['LJ40', 3],['LJ50', 4]]
     )



p = plotter()

p.function(drawable=d, function=lambda x: 172800,xrange=[-0.6, t.getmax('rownumber')+0.6], legend="", legendtext="", linedash=0, linecolor='red', step=0.1, linewidth=2)



L = legend()
L_line = legend()

series_list = ['AdaMBEFinder',	'FmbeFinder',	'PmbeFinder',	'ooMBEAFinder']
series_name = ['AdaMBE','FMBE','PMBE','ooMBEA']

idx = 0

t0 =table('fig-13-0.data')

p.line(drawable=d, table=t0, xfield='rownumber', yfield=series_list[idx], linecolor=styles[idx][0],
            linewidth=1.2,legend=L_line)

p.points(drawable=d, table=t0, xfield='rownumber', yfield=series_list[idx], linecolor=styles[idx][0],
                linewidth=1.2, style=styles[idx][1], fill=styles[idx][2], fillcolor='black', size=2.5,
            legend=L, legendtext=series_name[idx])

idx = 1

t1 =table('fig-13-1.data')

p.line(drawable=d, table=t1, xfield='rownumber', yfield=series_list[idx], linecolor=styles[idx][0],
            linewidth=1.2,legend=L_line)

p.points(drawable=d, table=t1, xfield='rownumber', yfield=series_list[idx], linecolor=styles[idx][0],
                linewidth=1.2, style=styles[idx][1], fill=styles[idx][2], fillcolor='black', size=2.5,
            legend=L, legendtext=series_name[idx])

idx = 2

t2 =table('fig-13-2.data')

p.line(drawable=d, table=t2, xfield='rownumber', yfield=series_list[idx], linecolor=styles[idx][0],
            linewidth=1.2,legend=L_line)

p.points(drawable=d, table=t2, xfield='rownumber', yfield=series_list[idx], linecolor=styles[idx][0],
                linewidth=1.2, style=styles[idx][1], fill=styles[idx][2], fillcolor='black', size=2.5,
            legend=L, legendtext=series_name[idx])

idx = 3

t3 =table('fig-13-3.data')

p.line(drawable=d, table=t3, xfield='rownumber', yfield=series_list[idx], linecolor=styles[idx][0],
            linewidth=1.2,legend=L_line)

p.points(drawable=d, table=t3, xfield='rownumber', yfield=series_list[idx], linecolor=styles[idx][0],
                linewidth=1.2, style=styles[idx][1], fill=styles[idx][2], fillcolor='black', size=2.5,
            legend=L, legendtext=series_name[idx])

# for idx in range(len(series_list)):

#     p.line(drawable=d, table=t, xfield='rownumber', yfield=series_list[idx], linecolor=styles[idx][0],
#                linewidth=1.2,legend=L_line)

#     p.points(drawable=d, table=t, xfield='rownumber', yfield=series_list[idx], linecolor=styles[idx][0],
#                  linewidth=1.2, style=styles[idx][1], fill=styles[idx][2], fillcolor='black', size=2.5,
#                 legend=L, legendtext=series_name[idx])


L.draw(canvas=c, coord=[d.left()+193, d.top()-35], width=5, height=5, vskip=5, fontsize=legendTextSize, skipnext=4, order=[2, 1, 3, 0])
L_line.draw(canvas=c, coord=[d.left()+185, d.top()-35], width=20, height=10, vskip=0, fontsize=20, skipnext=4, order=[2, 1, 3, 0]) 

c.text(coord=[30, 100],text="TLE", size=ylabelTextSize, color="red", font="Times-Bold")

# ----- drawing circle


c.render()
