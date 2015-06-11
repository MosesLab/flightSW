/*
 * controlFuncs.c
 * 
 * Author: Roy Smart
 * Created: May 15, 2014
 * 
 * contains the functions that direct the flight computer what to do in the 
 * event of telemetry input, e.g. timers, uplinks and hlp packets.
 */
#include "controlFuncs.h"

/*
 * Determines control string for uplink packets and finds the corresponding 
 * function by looking up the control string in the hash table
 */
int execPacket(packet_t* p) {
    node_t* np = lookup(hlpHashTable, p->control, hlp_hash_size); //Lookup corresponding function in table
    if (np == NULL) {
        return BAD_PACKET; //bad acknowlege if unsuccessful in finding control string in hash table
    }
    int status = (*((hlpControl) np->def))(p); //Call control function, cast from void pointer
    return status;

}

/**
 * Determines what control functions have been asserted via gpio pins. Then
 * executes control function
 * 
 * @param g
 * @return status of executed control functions
 */
void exec_gpio(gpio_in_uni * g) {

    /*loop through output to find asserted pins*/
    unsigned int i;
    for (i = 0; i < NUM_CONTROL_GPIO; i++) {

        U32 control_enable = g->val & gpio_control_mask[i];
        if (control_enable) {
            (*functionTable[i])();
        }
    }
}

/**
 * Control wait blocks until there is input available on either the file
 * descriptor or the queue
 * 
 * @param int, LockingQueue
 * @return 0 if failure, 1 if input is available on file descriptor,
 * 2 if input is available on the queue
 */
int control_wait(int fd, LockingQueue * queue) {
    int ret_val = FALSE;
    int rc;

    while (!ret_val) {
        /*wait for input on file desciptor*/
        rc = input_timeout(fd, 0, 50000); // wait for 5ms for input  

        if (rc == 0) { //timeout  

            /*check queue for input*/
            if (occupied(queue)) {
                ret_val = 2; //queue has input
            }
        } else if (rc == 1) { // fd has input
            ret_val = rc;
        } else {
            break;
        }
    }

    return ret_val;
}

/*
 * Power control functions
 */

/*Command the payload subsystem to power on*/
int enablePower(packet_t* p) {
    char msg[256];

    record("Command to enable subsystem power received\n");

    //Insert control code here  
    int subsystem = strtol(p->data, NULL, 16);
    if (subsystem <= NUM_SUBSYSTEM) {
        set_power(subsystem, ON);
    }
    sprintf(msg, "Enabled power subsystem: %d\n", subsystem);
    record(msg);

    packet_t* r = constructPacket(PWR_S, STATUS_ON, p->data);
    enqueue(&lqueue[hkdown], r);

    return GOOD_PACKET;
}

/*Command the payload subsystem to power off*/
int disablePower(packet_t* p) {
    char msg[256];

    record("Command to disable subsystem power received\n");
    //Insert control code here
    unsigned int subsystem = strtol(p->data, NULL, 16);
    if (subsystem <= NUM_SUBSYSTEM) {
        set_power(subsystem, OFF);
    }

    sprintf(msg, "Disabled power subsystem: %d\n", subsystem);
    record(msg);

    packet_t* r = constructPacket(PWR_S, STATUS_OFF, p->data);
    enqueue(&lqueue[hkdown], r);

    return GOOD_PACKET;
}

/*sets the provided subsystem to a new state*/
void set_power(U32 sys, U32 state) {
    /*off by one since sys starts at 1*/
    U32 mask = power_subsystem_mask[sys - 1];

    /*bitwise AND state with pin mask*/
    U32 masked_state = mask & state;

    /*apply new state to global memory*/
    gpio_out_state.val = (gpio_out_state.val & ~mask) | masked_state;

    /*dynamically allocate and copy new value*/
    gpio_out_uni * new_state = malloc(sizeof (U32));
    *new_state = gpio_out_state;

    /*enqueue new state to fpga server for assertion*/
    enqueue(&lqueue[gpio_out], new_state);
}

/*Query the power status of the payload subsystem*/
int queryPower(packet_t* p) {
    char msg[256];

    record("Command to query subsystem power received\n");

    unsigned int subsystem = strtol(p->data, NULL, 16);
    U32 power_state = OFF;

    /*dynamically allocate request for power state*/
    gpio_out_uni * new_state = malloc(sizeof (U32));
    new_state->val = REQ_POWER;

    /*enqueue new state to fpga server for assertion*/
    enqueue(&lqueue[gpio_out], new_state);

    /*wait for reply for request*/
    gpio_out_uni * req_state = dequeue(&lqueue[gpio_req]);

    /*off by one since sys starts at 1*/
    U32 mask = power_subsystem_mask[subsystem - 1];

    /*AND with mask to find state of requested power pin*/
    power_state = mask & req_state->val;

    sprintf(msg, "Querying subsystem %d\n", subsystem);
    record(msg);

    /*send response packet*/
    packet_t* r;
    if (power_state) {
        r = constructPacket(PWR_S, STATUS_ON, p->data);
    } else {
        r = constructPacket(PWR_S, STATUS_OFF, p->data);
    }
    enqueue(&lqueue[hkdown], r);

    return GOOD_PACKET;
}

int hlp_shell(int pipe_fd, packet_t * p) {
    if (strcmp(p->subtype, INPUT) == 0) {
        char msg[255];

        int rc = write(pipe_fd, p->data, p->dataSize + 1);

        if (rc < 0) {
            record("Shell failed to write to stdin\n");
            return BAD_PACKET;
        } else {
            sprintf(msg, "Executed command: %s\n", p->data);
            record(msg);

            /*send shell acknowledge*/
            packet_t * r = constructPacket(SHELL_S, ACK, p->data);
            enqueue(&lqueue[hkdown], r);

            return GOOD_PACKET;
        }
    } else if (strcmp(p->subtype, INTERRUPT) == 0) {
        kill(vshell_pid, SIGINT);
        return GOOD_PACKET;
    } else {
        return BAD_PACKET;
    }
}

/*Uplink control functions*/
int uDataStart() {
    record("Received data start Uplink\n");

    /*Check for updated sequences*/
    loadSequences();

    unsigned int i;
    for (i = 0; i < SEQ_MAP_SIZE; i++) {
        if (strstr(sequenceMap[i].sequenceName, "data") != NULL) {
            ops.sequence = i;
            break;
        }
    }

    /*enqueue sequence to science timeline*/
    enqueue(&lqueue[sequence], &sequenceMap[i]);

    packet_t* r = constructPacket(UPLINK_S, DATASTART, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

int uDataStop() {
    record("Received data stop Uplink\n");

    ops.seq_run = FALSE;

    packet_t* r = constructPacket(UPLINK_S, DATASTOP, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

int uDark1() {
    record("Received Dark1 Uplink\n");

    /*Check for updated sequences*/
    loadSequences();

    int i;
    for (i = 0; i < SEQ_MAP_SIZE; i++) {
        if (strstr(sequenceMap[i].sequenceName, "dark1") != NULL) {
            ops.sequence = i;
            break;
        }
    }


    /*enqueue sequence to science timeline*/
    enqueue(&lqueue[sequence], &sequenceMap[i]);

    packet_t* r = constructPacket(UPLINK_S, DARK1, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

int uDark2() {
    record("Received Dark2 Uplink\n");

    /*Check for updated sequences*/
    loadSequences();

    int i;
    for (i = 0; i < SEQ_MAP_SIZE; i++) {
        if (strstr(sequenceMap[i].sequenceName, "dark2") != NULL) {
            ops.sequence = i;
            break;
        }
    }


    /*enqueue sequence to science timeline*/
    enqueue(&lqueue[sequence], &sequenceMap[i]);

    packet_t* r = constructPacket(UPLINK_S, DARK2, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

int uDark3() {
    record("Received Dark3 Uplink\n");

    /*Check for updated sequences*/
    loadSequences();

    int i;
    for (i = 0; i < SEQ_MAP_SIZE; i++) {
        if (strstr(sequenceMap[i].sequenceName, "dark3") != NULL) {
            ops.sequence = i;
            break;
        }
    }

    /*enqueue sequence to science timeline*/
    enqueue(&lqueue[sequence], &sequenceMap[i]);

    packet_t* r = constructPacket(UPLINK_S, DARK3, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

int uDark4() {
    record("Received Dark4 Uplink\n");

    /*Check for updated sequences*/
    loadSequences();

    int i;
    for (i = 0; i < SEQ_MAP_SIZE; i++) {
        if (strstr(sequenceMap[i].sequenceName, "dark4") != NULL) {
            ops.sequence = i;
            break;
        }
    }

    /*enqueue sequence to science timeline*/
    enqueue(&lqueue[sequence], &sequenceMap[i]);

    packet_t* r = constructPacket(UPLINK_S, DARK4, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/* DJK 2/24/15 - This function still needs to be tested*/
int uSleep() {
    record("Received shutdown Uplink\n");

    /* Indicate that its MOSES should go to sleep*/
    ops.sleep = 1;

    /* Tell main we want to quit */
    kill(main_pid, SIGINT);


    packet_t* r = constructPacket(UPLINK_S, SLEEP, NULL);
    enqueue(&lqueue[hkdown], r);

    return GOOD_PACKET;
}

/*With how the Sleep function is implemented there isn't really much use for this function*/
int uWake() {
    record("Received shutdown pending Uplink\n");
    //Insert uplink handling code here
    packet_t* newp = constructPacket(UPLINK_S, WAKE, NULL);
    enqueue(&lqueue[hkdown], newp);
    return GOOD_PACKET;
}

int uTest() {
    record("Received test Uplink\n");

    /*Only thing needed done here is send a packet*/
    packet_t* r = constructPacket(UPLINK_S, TEST, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Timer control functions*/
int tDataStart() {
    record("Received data start Timer\n");

    /*Check for updated sequences*/
    loadSequences();

    unsigned int i;
    for (i = 0; i < SEQ_MAP_SIZE; i++) {
        if (strstr(sequenceMap[i].sequenceName, "data") != NULL) {
            ops.sequence = i;
            break;
        }
    }

    /*enqueue sequence to science timeline*/
    enqueue(&lqueue[sequence], &sequenceMap[i]);


    packet_t* r = constructPacket(TIMER_S, DATASTART, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

int tDataStop() {
    record("Received data stop Timer\n");

    ops.seq_run = FALSE;

    packet_t* r = constructPacket(TIMER_S, DATASTOP, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

int tDark2() {
    record("Received Dark2 Timer\n");

    /*Check for updated sequences*/
    loadSequences();

    int i;
    for (i = 0; i < SEQ_MAP_SIZE; i++) {
        if (strstr(sequenceMap[i].sequenceName, "dark2") != NULL) {
            ops.sequence = i;
            break;
        }
    }

    /*enqueue sequence to science timeline*/
    enqueue(&lqueue[sequence], &sequenceMap[i]);

    packet_t* r = constructPacket(TIMER_S, DARK2, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

int tDark4() {
    record("Received Dark4 Timer\n");

    /*Check for updated sequences*/
    loadSequences();

    int i;
    for (i = 0; i < SEQ_MAP_SIZE; i++) {
        if (strstr(sequenceMap[i].sequenceName, "dark4") != NULL) {
            ops.sequence = i;
            break;
        }
    }

    /*enqueue sequence to science timeline*/
    enqueue(&lqueue[sequence], &sequenceMap[i]);

    packet_t* r = constructPacket(TIMER_S, DARK4, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

int tSleep() {
    record("Received shutdown Timer\n");

    /* Indicate that its MOSES should go to sleep*/
    ops.sleep = 1;

    /* Tell main we want to quit */
    kill(main_pid, SIGUSR2);

    packet_t* r = constructPacket(TIMER_S, SLEEP, NULL);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}
/*
 *Mission Data Acquisition (MDAQ) control functions 
 */

/*Associates a sequence name with a signal number*/
int setSequence(packet_t* p) {
    record("Set sequence command received\n");

    //		char data[32];
    //	strcpy(data,p.getDataStr().c_str());
    //	char temp = data[2];
    //	data[2] = '\0';
    //	int num = atoi(data);
    //	data[2] = temp;
    //	sequenceMap[num]->reload(data+2);
    //	sequenceMap[num]->setNum(num);

    char data[32];
    strcpy(data, p->data);
    char temp = data[2];
    data[2] = '\0';
    int num = atoi(data);
    data[2] = temp;

    //reload(data + 2);
    sequenceMap[num].num = num; /*flightSW_2005 code never used num -DJK*/
    return GOOD_PACKET;
}

/*Uses stem for an output file.*/
// When is this used??? -djk

int setOutputName(packet_t* p) {
    record("Set output filename command received\n");
    strcpy(dataStub, p->data);
    ops.unique_stub = TRUE;
    return GOOD_PACKET;
}

/*Get sequence name that corresponds to the signal number in the data field*/
int getSeqName(packet_t* p) {
    record("Get sequence name command received\n");
    char numstr[16];
    char name[128];
    strcpy(numstr, p->data);
    strcpy(name, sequenceMap[atoi(numstr)].sequenceName);

    packet_t* r = constructPacket(MDAQ_RSP, GT_SEQ_NM, name);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Generates a string representation of the sequence file*/
int getSeqInfo(packet_t* p) {
    record("Get sequence info command received\n");
    char numstr[16];
    char info[256];
    strcpy(numstr, p->data);

    strcpy(info, toString(atoh(p->data)));
    packet_t* r = constructPacket(MDAQ_RSP, GT_SEQ_INFO, info);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Commands the flight software to return the currently running sequence*/
int getCurSeqName(packet_t* p) {
    record("Get current sequence name command received\n");
    char* name = currentSequence->sequenceName;
    packet_t* r = constructPacket(MDAQ_RSP, GT_CUR_SEQ, name);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Commands the flight software to return the current frame's exposure length*/
int getCurFrameLen(packet_t* p) {
    record("Get current frame length command received\n");
    char response[256];
    sprintf(response, "%6.3f", getCurrentExposure());
    packet_t* r = constructPacket(MDAQ_RSP, GT_CUR_FRML, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Command the flight software to return the current frame's index number*/
int getCurFrameIndex(packet_t* p) {
    record("Get current frame index command received\n");
    char response[4];
    sprintf(response, "%d", currentSequence->currentFrame);
    packet_t* r = constructPacket(MDAQ_RSP, GT_CUR_FRMI, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Commands the flight software to return the filename of the output file*/
int getOutputName(packet_t* p) {
    record("Get output filename command received\n");
    packet_t* r = constructPacket(MDAQ_RSP, GT_OFN, dataStub);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Commands the flight software to determine if he ROE is in self-test mode*/
int getSelftestStatus(packet_t* p) {
    record("Get self-test mode status command received\n");
    char response[5];
    sprintf(response, "%s", (ops.read_block == STMBLK) ? "ON" : "OFF");
    packet_t* r = constructPacket(MDAQ_RSP, GT_SLFT_STS, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Commands the flight software to determine if the ROE is in STIMS mode*/
int getStimsStatus(packet_t* p) {
    record("Get STIMS mode status command received\n");
    char response[5];
    sprintf(response, "%s", (ops.read_block == STBLK) ? "ON" : "OFF");
    packet_t* r = constructPacket(MDAQ_RSP, GT_STM_STS, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Commands the flight software to determine if Telemetry is enabled*/
int getTelemStatus(packet_t* p) {
    record("Get Telemetry mode status command received\n");
    char response[5];
    sprintf(response, "%s", (ops.tm_write == ON) ? "ON" : "OFF");
    packet_t* r = constructPacket(MDAQ_RSP, GT_TLM_STS, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Commands the flight software to determine if Channel 0 data will be recorded*/
int getCh0Status(packet_t* p) {
    record("Get status of channel 0 record command received\n");
    char response[5];
    sprintf(response, "%s", ((ops.channels & CH0) == CH0) ? "ON" : "OFF");
    packet_t* r = constructPacket(MDAQ_RSP, GT_CH0_STS, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Commands the flight software to determine if positive channel data will be recorded*/
int getPosOnlyStatus(packet_t* p) {
    record("Get status of positive channel record command received\n");
    char response[5];
    sprintf(response, "%s", (ops.channels == CH3) ? "ON" : "OFF");
    packet_t* r = constructPacket(MDAQ_RSP, GT_POS_STS, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Command the flight software to scale the current sequence, multiplying each frame by the
 floating point value contained within the Data field*/
int scaleSequence(packet_t* p) {
    record("Command to scale sequence received\n");
    char numstr[16];
    char *ptr;
    //char name[128];
    strcpy(numstr, p->data);

    scale(strtod(numstr, &ptr));
    return GOOD_PACKET;
}

/*Commands the flight software to translate the current sequence, adding the 
 floating point value in the Data field to each frame in the sequence*/
int translateSeq(packet_t* p) {
    char numstr[16];
    //char name[128];
    strcpy(numstr, p->data);
    char *ptr;
    record("Command to translate sequence received\n");
    translate(strtod(numstr, &ptr));
    return GOOD_PACKET;
}

/*Commands the flight software to interrupt the current exposure, find an exposure
 of the length given in the Data field, and jump to the first exposure matching
 this criterion*/
int findAndJump(packet_t* p) {
    record("Command to Find and Jump received\n");
    //Insert control code here
    char numstr[16];
    char response[100];
    char *ptr;
    //char name[128];
    strcpy(numstr, p->data);
    sprintf(response, "%2d", findAndJump_seq(strtod(numstr, &ptr)));
    packet_t* r = constructPacket(MDAQ_RSP, FIND_N_JUMP, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Commands the flight software to jump to the exposure with index number given 
 in the data field*/
int jumpToExp(packet_t* p) {
    record("Command to Jump to exposure received\n");
    //Insert control code here  
    char numstr[16];
    char response[100];
    strcpy(numstr, p->data);
    sprintf(response, "%2d", jump(atoi(numstr)));
    packet_t* r = constructPacket(MDAQ_RSP, JUMP, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}
/*CONFLICTING INTERFACES*/
/*HLP Document: Commands the flight software to save the current sequence to the 
 filename defined in the Data field (possibly "SVC") */

/*flightSW_2005: Saves the sequence defined in the Data field*/
int saveSequence(packet_t* p) {
    record("Command to save sequence received\n");
    /* User enters a sequence name, this will save the current sequence to that name*/
    char numstr[100];
    strcpy(numstr, p->data);
    saveAs(numstr);
    return GOOD_PACKET;
}

/*Commands the flight software to find exposures with lengths given by the first
 floating point value in the data field and replace them with the second value 
 the Data field*/
int findAndReplace(packet_t* p) {
    record("Command to find and replace received\n");
    //Insert control code here  
    //char* response = "0,0"; //test EGSE
    char response[128];
    char input[128];
    strcpy(input, p->data);
    double num1 = strtod(strtok(input, ","), NULL);
    double num2 = strtod(strtok(NULL, ","), NULL);
    strcpy(response, findAndReplace_seq(num1, num2));
    packet_t* r = constructPacket(MDAQ_RSP, FIND_N_RPLC, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Commands the flight software to start the currently loaded sequence*/
int beginSequence(packet_t* p) {
    record("Command to begin sequence received\n");
    ops.seq_run = TRUE;
    return GOOD_PACKET;
}

/*Commands the flight software to stop the currently running sequence*/
int endSequence(packet_t* p) {
    record("Command to stop sequence received\n");
    ops.seq_run = FALSE;
    return GOOD_PACKET;
}

/*Commands the flight software to terminate*/
int exitSW(packet_t* p) {
    record("Command to terminate received\n");

    kill(main_pid, SIGINT);

    return GOOD_PACKET;
}

/*commands the flight software to turn telemetry ON*/
int telemEnable(packet_t* p) {
    record("Command to enable telemetry received\n");
    ops.tm_write = ON;
    return GOOD_PACKET;
}

/*Commands the flight software to turn telemetry OFF*/
int telemDisable(packet_t* p) {
    record("Command to disable telemetry received\n");
    ops.tm_write = OFF;
    return GOOD_PACKET;
}

/*Commands the flight software to turn channel 0 ON*/
int ch0Enable(packet_t* p) {
    record("Command to enable Channel 0 received\n");
    ops.channels |= CH0;
    return GOOD_PACKET;
}

/*Commands the flight software to turn Channel 0 OFF*/
int ch0Disable(packet_t* p) {
    record("Command to disable Channel 0 received\n");
    ops.channels &= ~CH0;
    return GOOD_PACKET;
}

/* Commands the flight software to enable only positive channel */
int posOnlyEnable(packet_t* p) {
    record("Command to enable only positive channel received\n");
    ops.channels = CH3;
    return GOOD_PACKET;
}

/* Commands the flight software to disable only positive channel */
int posOnlyDisable(packet_t* p) {
    record("Command to disable only positive channel received\n");
    ops.channels = CH1 | CH2 | CH3;
    return GOOD_PACKET;
}

/*Commands the flight software to turn STIMS ON*/
int stimsEnable(packet_t* p) {
    record("Command to enable STIMS mode received.\n");

    if (roe_struct.active) {
        int var = stimOn();
        if (var == -1) {
            record("Did not go into STIMS mode.\n");
            packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_BACK, (char *) NULL);
            enqueue(&lqueue[hkdown], a);
        } else {
            ops.read_block = STMBLK;
            packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_GACK, (char *) NULL);
            enqueue(&lqueue[hkdown], a);
        }
    } else {
        record("STIM-ON ERROR: ROE INACTIVE!\n");
        packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_BACK, (char *) NULL);
        enqueue(&lqueue[hkdown], a);
    }
    return GOOD_PACKET;
}

/*Commands the flight software to turn STIMS OFF*/
int stimsDisable(packet_t* p) {
    record("Command to disable STIMS mode received\n");
    if (roe_struct.active) {
        int var = stimOff();
        if (var == -1) {
            record("ROE did not turn STIMS off.\n");
            packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_BACK, (char *) NULL);
            enqueue(&lqueue[hkdown], a);
        } else {
            ops.read_block =
                    (ops.roe_custom_read == TRUE) ? READBLK_CUSTOM : READBLK_DEFAULT;
            packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_GACK, (char *) NULL);
            enqueue(&lqueue[hkdown], a);
        }
    } else {
        record("STIM-OFF ERROR: ROE INACTIVE!\n");
        packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_BACK, (char *) NULL);
        enqueue(&lqueue[hkdown], a);
    }
    return GOOD_PACKET;
}

/*Commands the flight software to reset the ROE*/
int resetROE(packet_t* p) {
    record("Command to reset ROE received\n");
    if (roe_struct.active) {
        reset();
        ops.read_block =
                (ops.roe_custom_read == TRUE) ? READBLK_CUSTOM : READBLK_DEFAULT;
    } else {
        record("RESET-ROE ERROR: ROE INACTIVE!\n");
        packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_BACK, (char *) NULL);
        enqueue(&lqueue[hkdown], a);
    }
    return GOOD_PACKET;
}

/*Exit default mode*/

int disableDefaultROE(packet_t* p) {
    record("Command to exit to default mode received.\n");
    if (roe_struct.active) {
        int var = exitDefault();
        if (var == -1) {
            record("ROE did not exit default mode.\n");
            packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_GACK, (char *) NULL);
            enqueue(&lqueue[hkdown], a);
        } else {
            packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_BACK, (char *) NULL);
            enqueue(&lqueue[hkdown], a);
        }
    } else {
        record("EXIT-DEFAULT ERROR: ROE INACTIVE!\n");
        packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_BACK, (char *) NULL);
        enqueue(&lqueue[hkdown], a);
    }
    return GOOD_PACKET;
}

/*Commands the flight software to set the ROE to self-test mode*/
int enableSelftestROE(packet_t* p) {
    record("Command to set ROE to self-test mode received.\n");
    if (roe_struct.active) {
        int var = selftestMode();
        if (var == -1) {
            record("Did not enter self test mode\n");
            packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_BACK, (char *) NULL);
            enqueue(&lqueue[hkdown], a);
        } else {
            ops.read_block = STBLK;
            packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_GACK, (char *) NULL);
            enqueue(&lqueue[hkdown], a);
        }
    } else {
        record("SELF-TEST ERROR: ROE INACTIVE!\n");
        packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, ROE_BACK, (char *) NULL);
        enqueue(&lqueue[hkdown], a);
    }
    return GOOD_PACKET;
}

/*Command to reset the flight software*/
int resetSW(packet_t* p) {

    record("Command to reset flight software received\n");

    kill(main_pid, SIGUSR2); // used to be SIGHUP

    return GOOD_PACKET;
}



/*
 * Housekeeping control functions
 */

/*Request for flight computer +2.0V voltage*/

/* Not used, VDX doesn't have voltage monitoring*/
int FC_2V_V(packet_t* p) {
    record("Request for FC +2.0V voltage received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    packet_t* r = constructPacket(HK_RSP, POS2_0V, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Request for flight computer +2.5V voltage*/

/* Not used, VDX doesn't have voltage monitoring */
int FC_2_5V_V(packet_t* p) {
    record("Request for FC +2.5V voltage received\n");
    //Insert control code here  
    char* response = FC_P2_5V_V "0.0"; //test EGSE
    packet_t* r = constructPacket(HK_RSP, POS2_5V, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Request for ROE +2.5V voltage*/
int ROE_2_5V_V(packet_t* p) {
    record("Request for ROE +2.5V voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P2_5VD_V);
    char* response = getHK(VPOS2_5VD);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS2_5V, response);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +2.5V current*/
int ROE_2_5V_I(packet_t* p) {
    record("Request for ROE +2.5V current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P2_5VD_I);
    char* response = getHK(CPOS2_5VD);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS2_5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for flight computer +3.3V voltage*/

/* Not used, VDX doesn't have voltage monitoring */
int FC_3_3V_V(packet_t* p) {
    record("Request for FC +3.3V voltage received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    packet_t* r = constructPacket(HK_RSP, POS3_3V, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Request for flight computer +5.0V voltage*/

/* Not used, VDX doesn't have voltage monitoring */
int FC_POS_5V_V(packet_t* p) {
    record("Request for FC +5.0V voltage received\n");
    //Insert control code here  
    char* response = FC_P5V_V "0.0"; //test EGSE
    packet_t* r = constructPacket(HK_RSP, POS5V, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V A Channel voltage*/
int ROE_POS_5V_VA(packet_t* p) {
    record("Request for ROE +5.0V A Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P5VAA_V);
    char* response = getHK(VPOS5VA_A);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V B Channel voltage*/
int ROE_POS_5V_VB(packet_t* p) {
    record("Request for ROE +5.0V B Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P5VAB_V);
    char* response = getHK(VPOS5VA_B);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V D Channel voltage*/
int ROE_POS_5V_VD(packet_t* p) {
    record("Request for ROE +5.0V D Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P5VD_V);
    char* response = getHK(VPOS5VD);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V A Channel current*/
int ROE_POS_5V_IA(packet_t* p) {
    record("Request for ROE +5.0V A Channel current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P5VD_I);
    char* response = getHK(CPOS5VD);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V B Channel current*/
int ROE_POS_5V_IB(packet_t* p) {
    record("Request for ROE +5.0V B Channel current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P5VAB_I);
    char* response = getHK(CPOS5VA_B);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V D Channel current*/
int ROE_POS_5V_ID(packet_t* p) {
    record("Request for ROE +5.0V D Channel current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P5VD_I);
    char* response = getHK(CPOS5VD);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE -5.0V A Channel voltage*/
int ROE_NEG_5V_VA(packet_t* p) {
    record("Request for ROE -5.0V A Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_N5VAA_V);
    char* response = getHK(VNEG5VA_A);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, NEG5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE -5.0V B Channel voltage*/
int ROE_NEG_5V_VB(packet_t* p) {
    record("Request for ROE -5.0V B Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_N5VAB_V);
    char* response = getHK(VNEG5VA_B);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, NEG5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE -10.0V A Channel voltage*/
int ROE_NEG_10V_VA(packet_t* p) {
    record("Request for ROE -10.0V A Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_N10VA_V);
    char* response = getHK(VNEG10V_A);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, NEG10V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE -10.0V B Channel voltage*/
int ROE_NEG_10V_VB(packet_t* p) {
    record("Request for ROE -10.0V B Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_N10VB_V);
    char* response = getHK(VNEG10V_B);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, NEG10V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE -5.0V A Channel current*/
int ROE_NEG_5V_IA(packet_t* p) {
    record("Request for ROE -5.0V A Channel current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_N5VAA_I);
    char* response = getHK(CNEG5VA_A);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, NEG5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE -5.0V B Channel current*/
int ROE_NEG_5V_IB(packet_t* p) {
    record("Request for ROE -5.0V B Channel current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_N5VAB_I);
    char* response = getHK(CNEG5VA_B);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, NEG5V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for flight computer +12V voltage*/

/* Not used, VDX doesn't have voltage monitoring */
int FC_12V_V(packet_t* p) {
    record("Request for FC +12V voltage received\n");
    //Insert control code here
    char* response = FC_P12V_V "0.0"; //test EGSE
    packet_t* r = constructPacket(HK_RSP, POS12V, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Request for ROE +12V A Channel voltage*/
int ROE_12V_VA(packet_t* p) {
    record("Request for ROE +12V A Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P12VA_V);
    char* response = getHK(VPOS12V_A);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS12V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +12V B Channel voltage*/
int ROE_12V_VB(packet_t* p) {
    record("Request for ROE +12V B Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P12VB_V);
    char* response = getHK(VPOS12V_B);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS12V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +12V A Channel current*/
int ROE_12V_IA(packet_t* p) {
    record("Request for ROE +12V A Channel current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P12VA_I);
    char* response = getHK(CPOS12V_A);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS12V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +12V B Channel current*/
int ROE_12V_IB(packet_t* p) {
    record("Request for ROE +12V B Channel current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P12VB_I);
    char* response = getHK(CPOS12V_B);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS12V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +36V A Channel voltage*/
int ROE_36V_VA(packet_t* p) {
    record("Request for ROE +36V A Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P36VA_V);
    char* response = getHK(VPOS36V_A);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS36V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +36V B Channel voltage*/
int ROE_36V_VB(packet_t* p) {
    record("Request for ROE +36V B Channel voltage received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P36VB_V);
    char* response = getHK(VPOS36V_B);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS36V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +36V A Channel current*/
int ROE_36V_IA(packet_t* p) {
    record("Request for ROE +36V A Channel current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P36VA_I);
    char* response = getHK(CPOS36V_A);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS36V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE +36V B Channel current*/
int ROE_36V_IB(packet_t* p) {
    record("Request for ROE +36V B Channel current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, ROE_P36VB_I);
    char* response = getHK(CPOS36V_B);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, POS36V, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for flight computer temperature 1*/
int FC_TEMP_1(packet_t* p) {
    record("Request for FC temp 1 received\n");
    //Insert control code here  
    char* response = TEMP1 "0.0"; //test EGSE
    packet_t* r = constructPacket(HK_RSP, TEMP, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Request for flight computer temperature 2*/
int FC_TEMP_2(packet_t * p) {
    record("Request for FC temp 2 received\n");
    //Insert control code here  
    char* response = TEMP2 "0.0"; //test EGSE
    packet_t* r = constructPacket(HK_RSP, TEMP, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Request for flight computer temperature 3*/
int FC_TEMP_3(packet_t* p) {
    record("Request for FC temp 3 received\n");
    //Insert control code here  
    char* response = TEMP3 "0.0"; //test EGSE
    packet_t* r = constructPacket(HK_RSP, TEMP, response);
    enqueue(&lqueue[hkdown], r);
    return GOOD_PACKET;
}

/*Request for ROE upper temperature*/
int ROE_TEMP_UPPER(packet_t* p) {
    record("Request for ROE upper temperature received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, UPPER);
    char* response = getHK(UPPER_TEMP);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, TEMP, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for ROE lower temperature*/
int ROE_TEMP_LOWER(packet_t* p) {
    record("Request for ROE lower temperature received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, LOWER);
    char* response = getHK(LOWER_TEMP);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, TEMP, temp);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for CCDA_VOD current from the ROE*/
int ROE_CCDA_VOD(packet_t* p) {
    record("Request for ROE CCDA_VOD current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, CCDA_VOD);
    char* response = getHK(CCDA_VODC);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, CCDA_VOD, response);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for CCDA_VRD current from the ROE*/
int ROE_CCDA_VRD(packet_t* p) {
    record("Request for ROE CCDA_VRD current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, CCDA_VRD);
    char* response = getHK(CCDA_VRDC);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, CCDA_VRD, response);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for CCDA_VSS current from the ROE*/
int ROE_CCDA_VSS(packet_t* p) {
    record("Request for ROE CCDA_VSS current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, CCDA_VSS);
    char* response = getHK(CCDA_VSSC);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, CCDA_VSS, response);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for CCDS_VOD current from the ROE*/
int ROE_CCDS_VOD(packet_t* p) {
    record("Request for ROE CCDS_VOD current received\n");
    char * temp = malloc(sizeof (char) * 6);
    //strcpy(temp, CCDB_VOD);
    char* response = getHK(CCDB_VODC);
    //strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, CCDB_VOD, response);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for CCDS_VRD current from the ROE*/
int ROE_CCDS_VRD(packet_t* p) {
    record("Request for ROE CCDS_VRD current received\n");
    char * temp = malloc(sizeof (char) * 6);
    //strcpy(temp, CCDB_VRD);
    char* response = getHK(CCDB_VRDC);
    //strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, CCDB_VRD, response);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Request for CCDS_VSS current from the ROE*/
int ROE_CCDS_VSS(packet_t* p) {
    record("Request for ROE CCDS_VSS current received\n");
    char * temp = malloc(sizeof (char) * 6);
    strcpy(temp, CCDB_VSS);
    char* response = getHK(CCDB_VSSC);
    strcat(temp, response);
    packet_t* r = constructPacket(HK_RSP, CCDB_VSS, response);
    enqueue(&lqueue[hkdown], r);
    free(response);
    free(temp);
    return GOOD_PACKET;
}

/*Uses a hash table to match packet strings to function pointers*/
void hlpHashInit() {
    funcNumber = 87; //This number must be set and must be correct
    hlp_hash_size = funcNumber * 2;

    /*allocate space for control strings*/
    char** stringTable;
    if ((stringTable = (char**) malloc(sizeof (char*) * funcNumber)) == NULL) {
        record("malloc failed to allocate control string array\n");
    }

    /*Uplink control string-function matching*/
    stringTable[UDataStart] = UPLINK_S DATASTART;
    stringTable[UDataStop] = UPLINK_S DATASTOP;
    stringTable[UDark1] = UPLINK_S DARK1;
    stringTable[UDark2] = UPLINK_S DARK2;
    stringTable[UDark3] = UPLINK_S DARK3;
    stringTable[UDark4] = UPLINK_S DARK4;
    stringTable[USleep] = UPLINK_S SLEEP;
    stringTable[UWake] = UPLINK_S WAKE;
    stringTable[UTest] = UPLINK_S TEST;
    stringTable[TDataStart] = "NOTUSED1"; // Use useless strings so a zero hash
    stringTable[TDataStop] = "NOTUSED2"; // doesn't start timers
    stringTable[TDark2] = "NOTUSED3";
    stringTable[TDark4] = "NOTUSED4";
    stringTable[TSleep] = "NOTUSED5";
    stringTable[SetSeq] = MDAQ_RQS_S ST_SEQUENCE;
    stringTable[SetOut] = MDAQ_RQS_S ST_OFN;
    stringTable[GetSeqName] = MDAQ_RQS_S GT_SEQ_NM;
    stringTable[GetSeqInfo] = MDAQ_RQS_S GT_SEQ_INFO;
    stringTable[GetCurSeq] = MDAQ_RQS_S GT_CUR_SEQ;
    stringTable[GetFrameLen] = MDAQ_RQS_S GT_CUR_FRML;
    stringTable[GetFrameInd] = MDAQ_RQS_S GT_CUR_FRMI;
    stringTable[GetOut] = MDAQ_RQS_S GT_OFN;
    stringTable[GetSelf] = MDAQ_RQS_S GT_SLFT_STS;
    stringTable[GetStims] = MDAQ_RQS_S GT_STM_STS;
    stringTable[GetTelem] = MDAQ_RQS_S GT_TLM_STS;
    stringTable[GetCh0] = MDAQ_RQS_S GT_CH0_STS;
    stringTable[GetPosOnly] = MDAQ_RQS_S GT_POS_STS;
    stringTable[ScaleSeq] = MDAQ_RQS_S SCALE_SEQ;
    stringTable[TransSeq] = MDAQ_RQS_S TRANS_SEQ;
    stringTable[FindJump] = MDAQ_RQS_S FIND_N_JUMP;
    stringTable[JumpSeq] = MDAQ_RQS_S JUMP;
    stringTable[SaveSeq] = MDAQ_RQS_S SAV_SEQ;
    //stringTable[SaveCurSeq] = SAV_CUR_SEQ;
    stringTable[FindReplace] = MDAQ_RQS_S FIND_N_RPLC;
    stringTable[BeginSeq] = MDAQ_RQS_S BEGIN_SEQ;
    stringTable[EndSeq] = MDAQ_RQS_S END_SEQ;
    stringTable[ExitSW] = MDAQ_RQS_S EXT;
    stringTable[TelemOn] = MDAQ_RQS_S TELEM_ON;
    stringTable[TelemOff] = MDAQ_RQS_S TELEM_OFF;
    stringTable[ZeroOn] = MDAQ_RQS_S CHAN0_ON;
    stringTable[ZeroOff] = MDAQ_RQS_S CHAN0_OFF;
    stringTable[PosOnlyOn] = MDAQ_RQS_S POSONLY_ON;
    stringTable[PosOnlyOff] = MDAQ_RQS_S POSONLY_OFF;
    stringTable[StimsOn] = MDAQ_RQS_S STIM_ON;
    stringTable[StimsOff] = MDAQ_RQS_S STIM_OFF;
    stringTable[ResetROE] = MDAQ_RQS_S RST_ROE;
    stringTable[RoeDefaultOff] = MDAQ_RQS_S EXIT_DFLT;
    stringTable[RoeSelfOn] = MDAQ_RQS_S SET_SLFTST;
    stringTable[ResetSW] = MDAQ_RQS_S RESET_MDAQ;
    stringTable[PowerOn] = PWR_S TURN_ON;
    stringTable[PowerOff] = PWR_S TURN_OFF;
    stringTable[PowerQuery] = PWR_S QUERY;
    stringTable[FC_2V] = HK_RQS_S POS2_0V;
    stringTable[FC_2_5V] = HK_RQS_S POS2_5V FC_P2_5V_V;
    stringTable[ROE_2_5V] = HK_RQS_S POS2_5V ROE_P2_5VD_V;
    stringTable[ROE_2_5I] = HK_RQS_S POS2_5V ROE_P2_5VD_I;
    stringTable[FC_3_3V] = HK_RQS_S POS3_3V;
    stringTable[FC_P5_5V] = HK_RQS_S POS5V FC_P5V_V;
    stringTable[ROE_P5_5VA] = HK_RQS_S POS5V ROE_P5VAA_V;
    stringTable[ROE_P5_5VB] = HK_RQS_S POS5V ROE_P5VAB_V;
    stringTable[ROE_P5_5VD] = HK_RQS_S POS5V ROE_P5VD_V;
    stringTable[ROE_P5_5IA] = HK_RQS_S POS5V ROE_P5VAA_I;
    stringTable[ROE_P5_5IB] = HK_RQS_S POS5V ROE_P5VAB_I;
    stringTable[ROE_P5_5ID] = HK_RQS_S POS5V ROE_P5VD_I;
    stringTable[ROE_N5_5VA] = HK_RQS_S NEG5V ROE_N5VAA_V;
    stringTable[ROE_N5_5VB] = HK_RQS_S NEG5V ROE_N5VAB_V;
    stringTable[ROE_N5_5IA] = HK_RQS_S NEG5V ROE_N5VAA_I;
    stringTable[ROE_N5_5IB] = HK_RQS_S NEG5V ROE_N5VAB_I;
    stringTable[FC_12V] = HK_RQS_S POS12V FC_P12V_V;
    stringTable[ROE_12VA] = HK_RQS_S POS12V ROE_P12VA_V;
    stringTable[ROE_12VB] = HK_RQS_S POS12V ROE_P12VB_V;
    stringTable[ROE_12IA] = HK_RQS_S POS12V ROE_P12VA_I;
    stringTable[ROE_12IB] = HK_RQS_S POS12V ROE_P12VB_I;
    stringTable[ROE_36VA] = HK_RQS_S POS36V ROE_P36VA_V;
    stringTable[ROE_36VB] = HK_RQS_S POS36V ROE_P36VB_V;
    stringTable[ROE_36IA] = HK_RQS_S POS36V ROE_P36VA_I;
    stringTable[ROE_36IB] = HK_RQS_S POS36V ROE_P36VB_I;
    stringTable[FC_T1] = HK_RQS_S TEMP TEMP1;
    stringTable[FC_T2] = HK_RQS_S TEMP TEMP2;
    stringTable[FC_T3] = HK_RQS_S TEMP TEMP3;
    stringTable[ROE_TU] = HK_RQS_S TEMP UPPER;
    stringTable[ROE_TL] = HK_RQS_S TEMP LOWER;
    stringTable[ROE_CA_VOD] = HK_RQS_S CCDA_VOD;
    stringTable[ROE_CA_VRD] = HK_RQS_S CCDA_VRD;
    stringTable[ROE_CA_VSS] = HK_RQS_S CCDA_VSS;
    stringTable[ROE_CS_VOD] = HK_RQS_S CCDB_VOD;
    stringTable[ROE_CS_VRD] = HK_RQS_S CCDB_VRD;
    stringTable[ROE_CS_VSS] = HK_RQS_S CCDB_VSS;

    /*allocate space for function pointers*/
    if ((functionTable = calloc(funcNumber, sizeof (hlpControl))) == NULL) {
        record("malloc failed to allocate control function array\n");
    }

    /*array of function pointers to match with control strings*/
    functionTable[UDataStart] = &uDataStart;
    functionTable[UDataStop] = &uDataStop;
    functionTable[UDark1] = &uDark1;
    functionTable[UDark2] = &uDark2;
    functionTable[UDark3] = &uDark3;
    functionTable[UDark4] = &uDark4;
    functionTable[USleep] = &uSleep;
    functionTable[UWake] = &uWake;
    functionTable[UTest] = &uTest;
    functionTable[TDataStart] = &tDataStart;
    functionTable[TDataStop] = &tDataStop;
    functionTable[TDark2] = &tDark2;
    functionTable[TDark4] = &tDark4;
    functionTable[TSleep] = &tSleep;
    functionTable[SetSeq] = &setSequence;
    functionTable[SetOut] = &setOutputName;
    functionTable[GetSeqName] = &getSeqName;
    functionTable[GetSeqInfo] = &getSeqInfo;
    functionTable[GetCurSeq] = &getCurSeqName;
    functionTable[GetFrameLen] = &getCurFrameLen;
    functionTable[GetFrameInd] = &getCurFrameIndex;
    functionTable[GetOut] = &getOutputName;
    functionTable[GetSelf] = &getSelftestStatus;
    functionTable[GetStims] = &getStimsStatus;
    functionTable[GetTelem] = &getTelemStatus;
    functionTable[GetCh0] = &getCh0Status;
    functionTable[GetPosOnly] = &getPosOnlyStatus;
    functionTable[ScaleSeq] = &scaleSequence;
    functionTable[TransSeq] = &translateSeq;
    functionTable[FindJump] = &findAndJump;
    functionTable[JumpSeq] = &jumpToExp;
    functionTable[SaveSeq] = &saveSequence;
    //functionTable[SaveCurSeq] = saveCurSeqAs;
    functionTable[FindReplace] = &findAndReplace;
    functionTable[BeginSeq] = &beginSequence;
    functionTable[EndSeq] = &endSequence;
    functionTable[ExitSW] = &exitSW;
    functionTable[TelemOn] = &telemEnable;
    functionTable[TelemOff] = &telemDisable;
    functionTable[ZeroOn] = &ch0Enable;
    functionTable[ZeroOff] = &ch0Disable;
    functionTable[PosOnlyOn] = &posOnlyEnable;
    functionTable[PosOnlyOff] = &posOnlyDisable;
    functionTable[StimsOn] = &stimsEnable;
    functionTable[StimsOff] = &stimsDisable;
    functionTable[ResetROE] = &resetROE;
    functionTable[RoeDefaultOff] = &disableDefaultROE;
    functionTable[RoeSelfOn] = &enableSelftestROE;
    functionTable[ResetSW] = &resetSW;
    functionTable[PowerOn] = &enablePower;
    functionTable[PowerOff] = &disablePower;
    functionTable[PowerQuery] = &queryPower;
    functionTable[FC_2V] = &FC_2V_V;
    functionTable[FC_2_5V] = &FC_2_5V_V;
    functionTable[ROE_2_5V] = &ROE_2_5V_V;
    functionTable[ROE_2_5I] = &ROE_2_5V_I;
    functionTable[FC_3_3V] = &FC_3_3V_V;
    functionTable[FC_P5_5V] = &FC_POS_5V_V;
    functionTable[ROE_P5_5VA] = &ROE_POS_5V_VA;
    functionTable[ROE_P5_5VB] = &ROE_POS_5V_VB;
    functionTable[ROE_P5_5VD] = &ROE_POS_5V_VD;
    functionTable[ROE_P5_5IA] = &ROE_POS_5V_IA;
    functionTable[ROE_P5_5IB] = &ROE_POS_5V_IB;
    functionTable[ROE_P5_5ID] = &ROE_POS_5V_ID;
    functionTable[ROE_N5_5VA] = &ROE_NEG_5V_VA;
    functionTable[ROE_N5_5VB] = &ROE_NEG_5V_VB;
    functionTable[ROE_N5_5IA] = &ROE_NEG_5V_IA;
    functionTable[ROE_N5_5IB] = &ROE_NEG_5V_IB;
    functionTable[FC_12V] = &FC_12V_V;
    functionTable[ROE_12VA] = &ROE_12V_VA;
    functionTable[ROE_12VB] = &ROE_12V_VB;
    functionTable[ROE_12IA] = &ROE_12V_IA;
    functionTable[ROE_12IB] = &ROE_12V_IB;
    functionTable[ROE_36VA] = &ROE_36V_VA;
    functionTable[ROE_36VB] = &ROE_36V_VB;
    functionTable[ROE_36IA] = &ROE_36V_IA;
    functionTable[ROE_36IB] = &ROE_36V_IB;
    functionTable[FC_T1] = &FC_TEMP_1;
    functionTable[FC_T2] = &FC_TEMP_2;
    functionTable[FC_T3] = &FC_TEMP_3;
    functionTable[ROE_TU] = &ROE_TEMP_UPPER;
    functionTable[ROE_TL] = &ROE_TEMP_LOWER;
    functionTable[ROE_CA_VOD] = &ROE_CCDA_VOD;
    functionTable[ROE_CA_VRD] = &ROE_CCDA_VRD;
    functionTable[ROE_CA_VSS] = &ROE_CCDA_VSS;
    functionTable[ROE_CS_VOD] = &ROE_CCDS_VOD;
    functionTable[ROE_CS_VRD] = &ROE_CCDS_VRD;
    functionTable[ROE_CS_VSS] = &ROE_CCDS_VSS;

    /*initialize memory for function hash table*/
    if ((hlpHashTable = calloc(hlp_hash_size, sizeof (node_t))) == NULL) {
        record("malloc failed to allocate hash table\n");
    }

    /*fill hash table with array of strings matching function pointers*/
    int i;
    for (i = 0; i < funcNumber; i++) {
        installNode(hlpHashTable, stringTable[i], functionTable[i], hlp_hash_size);
    }
}

