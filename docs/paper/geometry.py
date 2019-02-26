from math import sin, cos, radians
from pyx import *

R = 1
L = 1/2

text.set(text.LatexRunner)

attr = [deco.barrow.small, deco.earrow.small]

c = canvas.canvas()
fillstyle = [style.linewidth.Thin, color.rgb.black, deco.filled([color.grey(0.8)])]
mypainter=graph.axis.painter.regular(labeldist=0.15*unit.v_cm)

# sphere
c.stroke(path.circle(0, 0, R), fillstyle)

# plane
c.stroke(path.rect(-1.3, -R-L, 2.6, -0.1), fillstyle)

# z-axis
c.insert(text.text(0.17, 1.35, r"$z$"))
c.stroke(path.line(0, -R-L, 0, 1.5), [ deco.earrow([deco.stroked([deco.stroked.clear])])])

# line and label for radius R (45°)
c.insert(text.text(0.45, -0.05, r"$R$", [text.halign.center]))
phi = radians(-25)
c.stroke(path.line(0, 0, cos(phi), sin(phi)), [deco.earrow.small, style.linewidth.thick])

# line with arrows and label L
c.insert(text.text(-0.34, -R-L/2, r"$L$", [text.halign.flushcenter, text.valign.middle]))
c.stroke(path.line(0, -R-L, 0, -1), [deco.barrow.small, deco.earrow.small, style.linewidth.thick])

c.writePDFfile()
