# MakeFile for PDL language

all: pdl.exe

OBJS= main.obj parse.obj symtab.obj built.obj ogg.obj matrix.obj \
draw.obj hard.obj g_ps.obj g_bgi.obj

CFLAGS=

gram.c: gram.y

gram.obj: lex.c types.h

lex.c: lex.l

$(OBJS): types.h
# main.obj: err.h
# built.obj hard.obj g_bgi.obj: prim.h

pdl.exe: gram.obj $(OBJS) 
	$(CC) $(CFLAGS) -epdl.exe @&&!
gram.obj $(OBJS)
! graphics.lib

clean:
	-rm $(OBJS)
	-rm gram.obj gram.c lex.c
	-rm pdl.exe
wc:
	wc main.c parse.c symtab.c built.c ogg.c matrix.c draw.c hard.c \
g_bgi.c g_ps.c gram.y lex.l proto.h types.h err.h prim.h

# Automatic .ps compilation

# .SUFFIXES: .p .ps

.p.ps:
	pdl -o $*.ps arc.pi diag.pi gate.pi $<


ps: pdl.exe \
	4reti.ps cnff.ps fsm.ps gctp.ps hwpm.ps pm.ps svc.ps \
	fc41.ps fc42.ps \
	s27.ps

4reti.ps cnff.ps fsm.ps gctp.ps hwpm.ps pm.ps svc.ps: arc.pi diag.pi
fc41.ps fc42.ps: arc.pi diag.pi
s27.ps: arc.pi gate.pi

# Automatic BGI preview

.p.view:
	pdl -q arc.pi greco.pi diag.pi gate.pi $<

view: pdl.exe \
4reti.view e121prod.view e221dfa.view fc41.view gctp.view pm.view cnff.view \
e12prod.view e221prod.view fc42.view hwpm.view s27.view e0.view e1g.view \
e2c.view fc43.view lfsr.view sa0sa1.view e1.view e2.view e2g.view fsm.view \
svc.view e121dfa.view e21g.view fc31.view

# Automatic .ARJ creation

arj:
	arj a pdl main.c parse.c symtab.c built.c ogg.c matrix.c draw.c hard.c \
g_bgi.c g_ps.c gram.y lex.l proto.h types.h err.h prim.h
