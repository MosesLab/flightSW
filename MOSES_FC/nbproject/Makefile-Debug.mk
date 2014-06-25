#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/control.o \
	${OBJECTDIR}/controlFuncs.o \
	${OBJECTDIR}/hashTable.o \
	${OBJECTDIR}/lockingQueue.o \
	${OBJECTDIR}/logger.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/packet.o \
	${OBJECTDIR}/roe_image.o \
	${OBJECTDIR}/sci_timeline_funcs.o \
	${OBJECTDIR}/science_timeline.o \
	${OBJECTDIR}/send_TM.o \
	${OBJECTDIR}/sequence.o \
	${OBJECTDIR}/tm_queue.o


# C Compiler Flags
CFLAGS=-Werror

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lm -lpthread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moses_fc

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moses_fc: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moses_fc ${OBJECTFILES} ${LDLIBSOPTIONS} -static

${OBJECTDIR}/control.o: control.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/control.o control.c

${OBJECTDIR}/controlFuncs.o: controlFuncs.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/controlFuncs.o controlFuncs.c

${OBJECTDIR}/hashTable.o: hashTable.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/hashTable.o hashTable.c

${OBJECTDIR}/lockingQueue.o: lockingQueue.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/lockingQueue.o lockingQueue.c

${OBJECTDIR}/logger.o: logger.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/logger.o logger.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -lm -pthread -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/packet.o: packet.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/packet.o packet.c

${OBJECTDIR}/roe_image.o: roe_image.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/roe_image.o roe_image.c

${OBJECTDIR}/sci_timeline_funcs.o: sci_timeline_funcs.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/sci_timeline_funcs.o sci_timeline_funcs.c

${OBJECTDIR}/science_timeline.o: science_timeline.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/science_timeline.o science_timeline.c

${OBJECTDIR}/send_TM.o: send_TM.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/send_TM.o send_TM.c

${OBJECTDIR}/sequence.o: sequence.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/sequence.o sequence.c

${OBJECTDIR}/tm_queue.o: tm_queue.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/tm_queue.o tm_queue.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moses_fc

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
