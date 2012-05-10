# New Makefile that automatically depends itself
#
# $Id: Makefile,v 1.3 1996/12/17 19:52:37 chase Exp $
#

IFLAGS = #-I/usr/local/include -I/home/projects/ee380l/linux/freeglut/include
DFLAGS = -DDEBUG=0 -DNO_WINDOW=0 -DSPECIES_SUMMARY=1 -DALGAE_SPORES=1 -DRANDOM=0 -DSLOWDOWN=0 -DUSE_GC=0
CXX = g++
CC  = $(GCC)
GCC = gcc
LD  = $(CXX)
BISON = bison
BISONFLAGS = -dv
FLEX = flex
FLEXFLAGS = 

LIBS = -L/usr/local/lib -L/usr/X11R6/lib -lX11 -lm -lfl
#LIBS = -L/usr/local/lib -lm -lfl

WFLAGS = -Wall 
SYMFLAGS = -g

PROFILE = #-pg 
OPTFLAGS =#-O
CFLAGS = $(OPTFLAGS) $(PROFILE) $(WFLAGS) $(IFLAGS) $(SYMFLAGS)
CXXFLAGS = $(CFLAGS)
CPPFLAGS = $(IFLAGS) $(DFLAGS)
LDFLAGS = $(PROFILE) -g 

PROGRAM = animals
CXXSRCS = LifeForm.cc animals.cc Window.cc Event.cc Algae.cc \
          Craig.cc LifeForm-Craig.cc Rock.cc Square.cc Bait.cc Praveen.cc 
          

YACCSRCS = 
LEXSRCS = animals-lex.l
CSRCS =
F77SRCS =
ASMSRCS =

SRCS = $(CXXSRCS) $(CSRCS) $(F77SRCS) $(ASMSRCS) 

OBJS = $(CXXSRCS:.cc=.o) $(CSRCS:.c=.o) $(F77SRCS:.F=.o) $(ASMSRCS:.S=.o) \
       $(YACCSRCS:.y=.o) $(LEXSRCS:.l=.o)

all: $(PROGRAM)


animals-lex.o: animals-lex.cc

$(PROGRAM): $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(PROGRAM)

tidy:
	rm -f *.BAK *.bak *.CKP

undepend:
	rm -f $(OBJS:%.o=.%.d) 

spotless: tidy clean undepend
	rm -f animals-lex.cc

.y.cc:
	$(BISON) $(BISONFLAGS) -o $@ $<
	mv $@.h $*.h
	mv $@.output $*.output
.l.cc:
	$(FLEX) ${FLEXFLAGS} -t $< > $@

# auto depend stuff for GNU make only
depend: undepend
	@echo ""
	@echo "Dependences are handled automatically, just \"make\""

ifneq ($(strip $(CSRCS)),)
.%.d: %.c 
	$(SHELL) -ec '$(GCC) -MM $(CPPFLAGS) $< > $@'


include $(CSRCS:%.c=.%.d)
endif 

ifneq ($(strip $(CXXSRCS)),)
.%.d: %.cc
	$(SHELL) -ec '$(GCC) -MM $(CPPFLAGS) $< > $@'

include $(CXXSRCS:%.cc=.%.d) 
endif 

# Not known to work for F77 (.F) or ASM (.S)
#
# ifneq ($(F77SRCS),)
# include $(F77SRCS:%.F=.%.d) 
# endif # F77SRCS

# ifneq ($(ASMSRCS),)
# include $(ASMSRCS:%.S=.%.d)
# endif # ASMSRCS
