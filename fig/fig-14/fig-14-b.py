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
    ['black',         'plusline',False],
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
c = canvas(ctype, 'fig-14-b', dimensions=[400, 300])

t = table(file='fig-14-b.data')

# print(min_value)

d = drawable(canvas=c, xrange=[3, 120],
             yrange=[20, 20000], dimensions=[pictureWidthMini, pictureHeightMini], coord=[40,30], yscale='log10', xscale='log2')


axis(drawable=d, style='box', ticstyle='in', dominortics=False,
     xminorticcnt=0, doxminortics=False, yminorticcnt=0,doxmajortics=True,
     # xtitle='Dataset', 
     ytitle='Running time (s)',
     xtitle='The number of threads',
     xlabelshift=[0,0],
     xlabelfontsize=xlabelTextSize,
     ylabelfontsize=ylabelTextSize,
     xtitlesize=xtitleTextSize,
     xaxisposition=20,
     xlabelrotate=0,
     ytitlesize=ytitleTextSize,
     linewidth=0.8, #yauto=['', '', 4],
     ymanual=[['100', 100],['1k',1000],['10k',10000]],
     xmanual=[['4', 4],['8', 8], ['16', 16],['32', 32],['64', 64],['96',96]]
     )

p = plotter()

L = legend()
L_line = legend()

series_list = ['ParAdaMBEFinder','ParMBE']
series_name = ['ParAdaMBE','ParMBE']

# print 32 line


# print 64 line
for idx in range(len(series_list)):

    p.line(drawable=d, table=t, xfield='thread_num', yfield=series_list[idx], linecolor=styles[idx][0],
               linewidth=1.2,legend=L_line)

    p.points(drawable=d, table=t, xfield='thread_num', yfield=series_list[idx], linecolor=styles[idx][0],
                 linewidth=1.2, style=styles[idx][1], fill=styles[idx][2], fillcolor='black', size=2.5,
                legend=L, legendtext=series_name[idx])


#c.box(coord=[[d.left()+47, d.top()-50], [d.left()+115, d.top()-30]], fill=True, fillcolor="white", linewidth=0.5)
L.draw(canvas=c, coord=[d.left()+53, d.top()-35], width=5, height=5, vskip=5, fontsize=legendTextSize, skipnext=2, skipspace=74, order=[1,0])
L_line.draw(canvas=c, coord=[d.left()+50, d.top()-35], width=10, height=10, vskip=0, fontsize=legendTextSize, skipnext=2, skipspace=74, order=[1,0])


# ----- drawing circle


c.render()
