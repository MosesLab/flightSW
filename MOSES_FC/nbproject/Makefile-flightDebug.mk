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
CND_CONF=flightDebug
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
	${OBJECTDIR}/dma.o \
	${OBJECTDIR}/fpga.o \
	${OBJECTDIR}/gpio.o \
	${OBJECTDIR}/hashTable.o \
	${OBJECTDIR}/helper.o \
	${OBJECTDIR}/lockingQueue.o \
	${OBJECTDIR}/logger.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/packet.o \
	${OBJECTDIR}/plx/Aardvark.o \
	${OBJECTDIR}/plx/ConsFunc.o \
	${OBJECTDIR}/plx/Eep_8000.o \
	${OBJECTDIR}/plx/PlxApi.o \
	${OBJECTDIR}/plx/PlxApiI2cAa.o \
	${OBJECTDIR}/plx/PlxInit.o \
	${OBJECTDIR}/roe.o \
	${OBJECTDIR}/roe_image.o \
	${OBJECTDIR}/sci_timeline_funcs.o \
	${OBJECTDIR}/science_timeline.o \
	${OBJECTDIR}/send_TM.o \
	${OBJECTDIR}/sequence.o \
	${OBJECTDIR}/v_shell.o


# C Compiler Flags
CFLAGS=-march=i486 -O2

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread -lm

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moses_fc

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moses_fc: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moses_fc ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/control.o: control.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/control.o control.c

${OBJECTDIR}/controlFuncs.o: controlFuncs.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/controlFuncs.o controlFuncs.c

${OBJECTDIR}/dma.o: dma.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/dma.o dma.c

${OBJECTDIR}/fpga.o: fpga.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/fpga.o fpga.c

${OBJECTDIR}/gpio.o: gpio.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/gpio.o gpio.c

${OBJECTDIR}/hashTable.o: hashTable.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/hashTable.o hashTable.c

${OBJECTDIR}/helper.o: helper.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/helper.o helper.c

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
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/packet.o: packet.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/packet.o packet.c

${OBJECTDIR}/plx/Aardvark.o: plx/Aardvark.c 
	${MKDIR} -p ${OBJECTDIR}/plx
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/plx/Aardvark.o plx/Aardvark.c

${OBJECTDIR}/plx/ConsFunc.o: plx/ConsFunc.c 
	${MKDIR} -p ${OBJECTDIR}/plx
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/plx/ConsFunc.o plx/ConsFunc.c

${OBJECTDIR}/plx/Eep_8000.o: plx/Eep_8000.c 
	${MKDIR} -p ${OBJECTDIR}/plx
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/plx/Eep_8000.o plx/Eep_8000.c

${OBJECTDIR}/plx/PlxApi.o: plx/PlxApi.c 
	${MKDIR} -p ${OBJECTDIR}/plx
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/plx/PlxApi.o plx/PlxApi.c

${OBJECTDIR}/plx/PlxApiI2cAa.o: plx/PlxApiI2cAa.c 
	${MKDIR} -p ${OBJECTDIR}/plx
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/plx/PlxApiI2cAa.o plx/PlxApiI2cAa.c

${OBJECTDIR}/plx/PlxInit.o: plx/PlxInit.c 
	${MKDIR} -p ${OBJECTDIR}/plx
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/plx/PlxInit.o plx/PlxInit.c

${OBJECTDIR}/roe.o: roe.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/roe.o roe.c

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

${OBJECTDIR}/v_shell.o: v_shell.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/v_shell.o v_shell.c

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
