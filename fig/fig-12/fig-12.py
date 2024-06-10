#! /usr/bin/env python

from zplot import *
from pathlib import Path
sys.path[0] = str(Path(sys.path[0]).parent) 
from zplot_config import *
import math

# populate zplot table from data file
ctype = 'pdf' if len(sys.argv) < 2 else sys.argv[1]

######################################large_data#################################

c = canvas(ctype, title='fig-12-b', dimensions=[400, 200])
t = table(file='fig-12-b.data')
d = drawable(canvas=c, xrange=[-0.6, t.getmax('rownumber')+0.6],
             yrange=[0, 260], dimensions=[pictureWidthMini, pictureHeightMini], coord=[200,30])

axis(drawable=d,style='y',ticstyle='in',
     doxmajortics=False,doymajortics=True,
     ymanual=[['0', 0], ['50', 50],['100', 100],['150', 150],['200', 200],['250', 250]],
     ticmajorsize = 3,         
     ylabelfontsize=ylabelTextSize,         
     ytitlesize=ytitleTextSize,)

axis(drawable=d, style='box', ticstyle='in',
         doxmajortics=False, doymajortics=True,
         xminorticcnt=0, doxminortics=False, #yminorticcnt=0,
         #xtitle='(b) Large datasets.', 
         ytitle='Running time (s)', doylabels=True, ytitleshift=[ytitleShiftX,ytitleShiftY],
         linewidth=0.8,
         #yaxisposition=1,
         #xaxisposition=0, yauto=['','',3],
#          xlabelrotate=20,
        xaxisposition=0, #yauto=['','',0.5], 
        xlabelshift=[0, 0],
         xlabelfontsize=xlabelTextSize,
         ylabelfontsize=ylabelTextSize,
         xtitlesize=xtitleTextSize,
         ytitlesize=ytitleTextSize,
         ymanual=[['0', 0], ['50', 50],['100', 100],['150', 150],['200', 200],['250', 250]],
        xmanual = t.getaxislabels('Serie'),
     )
p = plotter()
L = legend()
L_line = legend()

# grid(drawable=d, x=False, y=True, ystep=0.4,  yrange=[0.2, 1.5],
#     linedash=[1,1], linewidth=0.3, linecolor='black')

bartypes = [('solid', 1, 4),
            ('solid', 1, 4),
            ('solid', 1, 4),
            ('dline2', 1, 4),
            ('hline', 1, 2),
            ('dline1', 1, 4),
            ('dline2', 1, 4),
            ('solid', 1, 4),
            ('solid', 1, 1),]

series_list = ['AdaMBEFinder', 'AdaMBEFinderRand', 'AdaMBEFinderUC']
series_name = ['AdaMBE-ASC','AdaMBE-RAND','AdaMBE-UC']
bgcolors    = ['white', 'white', 'white', 'white', 'white','lightgrey', 'lightgrey', 'black']
fillcolors  = fig_colors_light # ['darkgreen', 'lightcyan', 'lightyellow', 'black', 'black','black', 'black', 'black',]

for i in range(len(series_list)):
    p.verticalbars(drawable=d, table=t, xfield='rownumber', yfield=series_list[i],
               barwidth=0.7, 
               linewidth=0.7, cluster =[i,len(series_list)], legend=L, legendtext=series_name[i],
               labelformat='%s samples/ms',labelrotate=0,labelsize=8,
               fill=True, fillcolor=fillcolors[i], bgcolor=bgcolors[i],
               fillstyle=bartypes[i][0], fillsize=0.4, fillskip=bartypes[i][2])
c.render()

######################################medium_data#################################

c = canvas(ctype, title='fig-12-a', dimensions=[400, 200])
t = table(file='fig-12-a.data')
d = drawable(canvas=c, xrange=[-0.6, t.getmax('rownumber')+0.6],
             yrange=[0, 12.4], dimensions=[pictureWidthMini, pictureHeightMini], coord=[40,30])

axis(drawable=d,style='y',ticstyle='in',
     doxmajortics=False,doymajortics=True,
     ymanual=[['0', 0], ['2', 2],['4', 4],['6', 6],['8', 8],['10', 10],['12', 12],],
     ticmajorsize = 3,         
     ylabelfontsize=ylabelTextSize,         
     ytitlesize=ytitleTextSize,)

axis(drawable=d, style='box', ticstyle='in',
         doxmajortics=False, doymajortics=True,
         xminorticcnt=0, doxminortics=False, #yminorticcnt=0,
         #xtitle='(a) Medium datasets.', 
         ytitle='Running time (s)', doylabels=True, ytitleshift=[ytitleShiftX,ytitleShiftY],
         linewidth=0.8,
         #yaxisposition=1,
         #xaxisposition=0, yauto=['','',3],
#          xlabelrotate=20,
        xaxisposition=0, #yauto=['','',0.5], 
        xlabelshift=[0, 0],
         xlabelfontsize=xlabelTextSize,
         ylabelfontsize=ylabelTextSize,
         xtitlesize=xtitleTextSize,
         ytitlesize=ytitleTextSize,
         ymanual=[['0', 0], ['2', 2],['4', 4],['6', 6],['8', 8],['10', 10],['12', 12],],
        xmanual = t.getaxislabels('Serie'),
     )
p = plotter()
L = legend()
L_line = legend()

# grid(drawable=d, x=False, y=True, ystep=0.4,  yrange=[0.2, 1.5],
#     linedash=[1,1], linewidth=0.3, linecolor='black')

bartypes = [('solid', 1, 4),
            ('solid', 1, 4),
            ('solid', 1, 4),
            ('dline2', 1, 4),
            ('hline', 1, 2),
            ('dline1', 1, 4),
            ('dline2', 1, 4),
            ('solid', 1, 4),
            ('solid', 1, 1),]

series_list = ['AdaMBEFinder', 'AdaMBEFinderRand', 'AdaMBEFinderUC']
series_name = ['AdaMBE-ASC','AdaMBE-RAND','AdaMBE-UC']
bgcolors    = ['white', 'white', 'white', 'white', 'white','lightgrey', 'lightgrey', 'black']
fillcolors  = fig_colors_light # ['darkgreen', 'lightcyan', 'lightyellow', 'black', 'black','black', 'black', 'black',]

for i in range(len(series_list)):
    p.verticalbars(drawable=d, table=t, xfield='rownumber', yfield=series_list[i],
               barwidth=0.7, 
               linewidth=0.7, cluster =[i,len(series_list)], legend=L, legendtext=series_name[i],
               labelformat='%s samples/ms',labelrotate=0,labelsize=8,
               fill=True, fillcolor=fillcolors[i], bgcolor=bgcolors[i],
               fillstyle=bartypes[i][0], fillsize=0.4, fillskip=bartypes[i][2])
c.render()


