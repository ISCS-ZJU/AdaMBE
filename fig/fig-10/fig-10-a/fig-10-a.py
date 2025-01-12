#! /usr/bin/env python

from zplot import *
from pathlib import Path
sys.path[0] = str(Path(sys.path[0]).parent.parent) 
from zplot_config import *
import math

# populate zplot table from data file
ctype = 'pdf' if len(sys.argv) < 2 else sys.argv[1]

c = canvas(ctype, title='fig-10-a', dimensions=[400, 200])


######################################real_time#################################

t = table(file='fig-10-a.data')
d = drawable(canvas=c, xrange=[-0.6, t.getmax('rownumber')+0.6],
             yrange=[0.8, 15000], dimensions=[pictureWidthShort, pictureHeight], coord=[40,30], yscale='log10')

axis(drawable=d,style='y',ticstyle='in',
     doxmajortics=False,doymajortics=True,
     ymanual=[['1', 1],['10', 10],['100', 100], ['1k', 1000], ['10k', 10000]],
     ticmajorsize = 3,         
     ylabelfontsize=ylabelTextSize,         
     ytitlesize=ytitleTextSize,)

axis(drawable=d, style='box', ticstyle='in',
         doxmajortics=False, doymajortics=True,
         xminorticcnt=0, doxminortics=False, #yminorticcnt=0,
         #xtitle='(a) Real workloads.', 
         ytitle='Running time (s)', doylabels=True, ytitleshift=[ytitleShiftX,ytitleShiftY],
         linewidth=0.8,
         #yaxisposition=1,
         #xaxisposition=0, yauto=['','',3],
#          xlabelrotate=20,
        xaxisposition=1.5, #yauto=['','',0.5], 
        xlabelshift=[0, -8],
         xlabelfontsize=xlabelTextSize,
         ylabelfontsize=ylabelTextSize,
         xtitlesize=xtitleTextSize,
         ytitlesize=ytitleTextSize,
         ymanual=[['1', 1],['10', 10],['100', 100], ['1k', 1000], ['10k', 10000]],
        xmanual = t.getaxislabels('Serie'),
        #xmanual=[['Unicode', 0], ['UCforum', 1], ['Writers', 2],
        #        ['YouTube', 3], 
        #         ['Teams', 4], ['ActorMovies', 5],
        #        ['IMDB', 6] , ['Wikipedia', 7], ['DBLP', 8], ['Wikinews', 9],
        #        ['MovieLens', 10], ['Amazon', 11],]
     )
p = plotter()
L = legend()
L_line = legend()

# grid(drawable=d, x=False, y=True, ystep=0.4,  yrange=[0.2, 1.5],
#     linedash=[1,1], linewidth=0.3, linecolor='black')

bartypes = [('solid', 1, 4),
            ('solid', 1, 4),
            ('solid', 1, 4),
            ('solid', 1, 4),
            ('solid', 1, 2),
            ('dline1', 1, 4),
            ('dline2', 1, 4),
            ('solid', 1, 4),
            ('solid', 1, 1),]

#series_list = ['MBEA','iMBEA','PMBE','MineLMBC','FMBE','MMBEA_FAST','MMBEA_MIN','MMBEA']
#series_name = ['MBEA','iMBEA','PMBE','MineLMBC','FMBE','MMBEA_FAST','MMBEA_MIN','MMBEA']
series_list = ['Baseline', 'LNC', 'BDS','AdaMBE']
series_name = ['Baseline','AdaMBE-LN','AdaMBE-BIT','AdaMBE']
bgcolors    = ['white', 'white', 'white', 'white', 'white','lightgrey', 'lightgrey', 'black']
fillcolors  = ['whitesmoke',fig_colors[1],fig_colors[2],fig_colors[0],'whitesmoke']
# ['darkgreen', 'lightcyan', 'lightyellow', 'black', 'black','black', 'black', 'black',]

for i in range(len(series_list)):
    p.verticalbars(drawable=d, table=t, xfield='rownumber', yfield=series_list[i],
               barwidth=0.7, 
               linewidth=0.7, cluster =[i,len(series_list)], legend=L, legendtext=series_name[i],
               labelformat='%s samples/ms',labelrotate=0,labelsize=8,
               fill=True, fillcolor=fillcolors[i], bgcolor=bgcolors[i],
               fillstyle=bartypes[i][0], fillsize=0.4, fillskip=bartypes[i][2])

# max line
# p.line(drawable=d, table=t, xfield='rownumber', yfield='CSwap_max', linecolor='black',
#                linewidth=0.6,legend=L_line)
#
# p.points(drawable=d, table=t, xfield='rownumber', yfield='CSwap_max', linecolor='black',
#                  linewidth=0.7, style='circle', fill=False, fillcolor='black', size=0.8,
#                 legend=L, legendtext='CSwap max')


### text for each normalized time
x_start = 5.0
x_step = 28.7
#time = [0.017194,0.18964,126.901452,81.761355,11051.09641,4205.625925,4569.370564,295.365442,1016.563723,7921.560954,4421.418432,204356.4221]
#r2 = [0.005033,0.061496,0.040129,29.19595,4293.674855,1365.841162,147.492321,2.494761,7.70112,53.366439,6.763336,9.389025]

rindex = t.getrindex()
rows  = t.query()

'''
for i in range(len(time)):
    if time[i]>1:
        x_text = "%.1fs" %  (time[i])
    else:
        x_text = "%.3fs" % (time[i])
    rate = 5/(math.log(1000,10)+5)
    c.text(coord=[d.left()+x_start+i*x_step, d.bottom()+0.5+70*rate], rotate=90,
         text=x_text, size=5, anchor='l,h')
    if r2[i]>1:
        x_text = "%.1fs" %  (r2[i])
    else:
        x_text = "%.3fs" % (r2[i])
    #offset = 1 - float(rows[i][rindex['MMBEA']])
    offset = (math.log(float(rows[i][rindex['order2']]), 10) + 5)/(math.log(1000, 10) + 5)
    c.text(coord=[d.left()+x_start+20.8+i*x_step, d.bottom()+0.5+70*offset], rotate=90,
         text=x_text, size=5, anchor='l,h')
'''
#c.text(coord=[d.left()+299, d.bottom()+73], rotate=0,
#         text='2.5x', size=3, anchor='l,h')
#c.text(coord=[d.left()+325, d.bottom()+73], rotate=0,
#         text='2.2x', size=3, anchor='l,h')

#### legend
L.draw(canvas=c, coord=[d.left()+10, d.top()-8], skipnext=4, skipspace=98,
    hspace=3, fontsize=legendTextSize,  width=7, height=7 )
# L_line.draw(canvas=c, coord=[d.left()+118, d.top()-9], width=10, height=15, fontsize=8, skipnext=1, skipspace=55)

##################zoom###################################
'''
zoom_d = drawable(canvas=c,
                 coord=[300,60],
                 xrange=[-0.8, 3],
                 yrange=[0,0.005],
                 dimensions=['1in', '0.5in'])
axis(drawable=zoom_d, style='box',ticstyle='in',
    doxmajortics=False, doymajortics=True,doxminortics=False,
    yminorticcnt = 0, dominortics=True,
     xtitle='', ytitle='',doylabels=True,
    linewidth=0.5,xaxisposition=0,
    xlabelrotate=30,
    xlabelshift=[0,0],
    xlabelfontsize=3,
    ylabelfontsize=3,
    doxlabels=True,
    xmanual=[['DBLP', 0], ['Wikipedia', 1], ['Writers', 2]])

p1 = plotter()
count = 0
for i in [4, 5, 7]:
    p1.verticalbars(drawable=zoom_d, table=t2, xfield='rownumber', yfield=series_list[i],
               barwidth=0.75, yloval=0,
               linewidth=0.7, cluster =[count,3], legend=L, legendtext=series_name[i],
               labelformat='%s samples/ms',labelrotate=0,labelsize=5,
               fill=True, fillcolor=fillcolors[i], bgcolor=bgcolors[i],
               fillstyle=bartypes[i][0], fillsize=0.4, fillskip=bartypes[i][2])
    count += 1
'''


c.render()
