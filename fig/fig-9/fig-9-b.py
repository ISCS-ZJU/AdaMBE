#! /usr/bin/env python

from zplot import *
from pathlib import Path
sys.path[0] = str(Path(sys.path[0]).parent) 
from zplot_config import *
import math

# populate zplot table from data file
ctype = 'pdf' if len(sys.argv) < 2 else sys.argv[1]

######################################large_data#################################

c = canvas(ctype, title='fig-9-b', dimensions=[400, 200])
t = table(file='fig-9-b.data')
d = drawable(canvas=c, xrange=[-0.6, t.getmax('rownumber')+0.6],
             yrange=[0, 28000000000], dimensions=[pictureWidthShort, pictureHeight], coord=[40,30])

c.box(coord=[[210,30],[40+pictureWidthShort,30+pictureHeight]],linecolor="black", linedash="3",linewidth=0.5,fillcolor="",bgcolor="gainsboro",fillstyle='')

c.text(coord=[284,3],text="Parallel algorithms", size=9, color="black", font="Times-Bold")

c.text(coord=[110,3],text="Serial algorithms", size=9, color="black", font="Times-Bold")


axis(drawable=d,style='y',ticstyle='in',
     doxmajortics=False,doymajortics=True,
     ymanual=[['0', 0], ],
     ticmajorsize = 3,         
     ylabelfontsize=ylabelTextSize,         
     ytitlesize=ytitleTextSize,)

axis(drawable=d, style='box', ticstyle='in',
         doxmajortics=False, doymajortics=True,
         xminorticcnt=0, doxminortics=False, #yminorticcnt=0,
         #xtitle='(b) Large datasets.', 
         ytitle='Maximal bicliques', doylabels=True, ytitleshift=[ytitleShiftX,ytitleShiftY],
         linewidth=0.8,
         #yaxisposition=1,
         #xaxisposition=0, yauto=['','',3],
        xlabelrotate=20,
        xaxisposition=0, #yauto=['','',0.5], 
        xlabelshift=[0, -5],
         xlabelfontsize=xlabelTextSize,
         ylabelfontsize=ylabelTextSize,
         xtitlesize=xtitleTextSize,
         ytitlesize=ytitleTextSize,
         ymanual=[['0', 0],['5b',5000000000],['10b',10000000000],['15b',15000000000],['20b',20000000000],['25b',25000000000]],
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
series_name = ['AdaMBE-INC','AdaMBE-RAND','AdaMBE-UC']
bgcolors    = ['white', 'white', 'white', 'white', 'white','lightgrey', 'lightgrey', 'black']
fillcolors  = fig_colors_light # ['darkgreen', 'lightcyan', 'lightyellow', 'black', 'black','black', 'black', 'black',]


p.verticalbars(drawable=d, table=t, xfield='rownumber', yfield='nodes',
               barwidth=0.5, 
               linewidth=0.7, 
               labelformat='%s samples/ms',labelrotate=0,labelsize=8,
               fill=True, fillcolor=fig_colors[2], bgcolor='white',
               fillstyle='solid', fillsize=0.4, fillskip=1)

x_start = 27
x_step = 42 
y_start = 2
y_step = 0.00000000264

values=t.getvalues('nodes')
value0 = float(values[0])


for i in range(len(values)):
  value = float(values[i])
  y_offset = y_start + value * y_step
  text = "%.1fb (all)" % (value0 / 1000000000)
  if value > 1000000000 and value < value0:
    text = "%.1fb" % (value / 1000000000)
  elif value < 1000000000:
    text = "%.0fm" % (value / 1000000)
  c.text(coord=[d.left()+x_start+i*x_step, d.bottom()+y_offset+8],text =text, size=8)

times=t.getvalues('time')
for i in range(len(values)):
  value = float(values[i])
  y_offset = y_start + value * y_step
  text = "(TLE)"
  if float(times[i]) < 3600 * 48:
    text ="(%.1fh)" %(float(times[i])/3600)
  c.text(coord=[d.left()+x_start+i*x_step, d.bottom()+y_offset+1],text =text, size=8, color="dimgray")
c.render()

