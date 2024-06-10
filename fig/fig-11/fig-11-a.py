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
c = canvas(ctype, 'fig-11-a', dimensions=[400, 300])

t = table(file='fig-11.data')

# print(min_value)

d = drawable(canvas=c, xrange=[0, 270],
             yrange=[0, 2150], dimensions=[pictureWidthMini, pictureHeightMini], coord=[40,30])


axis(drawable=d, style='box', ticstyle='in', dominortics=False,
     xminorticcnt=0, doxminortics=False, yminorticcnt=0,doxmajortics=True,
     # xtitle='Dataset', 
     ytitle='Running time (s)',
     xtitle='Threshold t',
     xlabelshift=[0,0],
     xlabelfontsize=xlabelTextSize,
     ylabelfontsize=ylabelTextSize,
     xtitlesize=xtitleTextSize,
     xaxisposition=20,
     xlabelrotate=0,
     ytitlesize=ytitleTextSize,
     linewidth=0.8, #yauto=['', '', 4],
     ymanual=[['0',0],['500', 500],['1000', 1000],['1500', 1500],['2000',2000]],
     xmanual=[['32',32],['64',64],['96',96],['128',128],['160',160],['192',192],['224',224],['256',256]]
     )

p = plotter()

L = legend()
L_line = legend()

series_list = ['BookCrossing']
series_name = ['AdaMBE-BIT']

# print 32 line


# print 64 line
for idx in range(len(series_list)):

    p.line(drawable=d, table=t, xfield='tau_num', yfield=series_list[idx], linecolor=styles[idx][0],
               linewidth=0.8,legend=L_line)

    p.points(drawable=d, table=t, xfield='tau_num', yfield=series_list[idx], linecolor=styles[idx][0],
                 linewidth=1, style=styles[idx][1], fill=styles[idx][2], fillcolor='black', size=1,
                legend=L, legendtext=series_name[idx])


# L.draw(canvas=c, coord=[d.left()+5, d.top()+10], width=10, height=2.5, fontsize=legendTextSize, skipnext=1, skipspace=120)
# L_line.draw(canvas=c, coord=[d.left()+5, d.top()+10], width=10, height=15, fontsize=legendTextSize, skipnext=1, skipspace=120)


# ----- drawing circle

L.draw(canvas=c, coord=[d.left()+45, d.top()-12], skipnext=1, skipspace=105,
    hspace=3, fontsize=legendTextSize,  width=5, height=2.5 )
L_line.draw(canvas=c, coord=[d.left()+42, d.top()-12], width=10, height=15, fontsize=legendTextSize, skipnext=1, skipspace=120)


c.render()


#c = canvas(ctype, 'sensitivity_threshold_legend', dimensions=[400, 300])

