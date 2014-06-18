#include "control_sim.h"


void* science(void* arg){
    int notify_fd, watch_fd;
    struct inotify_event *event;
    char* imagepath = "/students/jackson.remington/esus/testFiles/imageFiles/";
    struct dirent *dPath;                           //For browsing directories
    char *tmpPath = NULL;
    int input_len;
    char buffer2[EVENT_BUFFER_LENGTH];
    char *bufPtr;
    
    imgPtr_t * currentImg = malloc(sizeof(imgPtr_t));
    tm_queue_init(&roeQueue);
    
    notify_fd = inotify_init();                 //initialize directory monitor
    if (notify_fd < 0) {
        perror("cannot initialize i_notify");
        exit(EXIT_FAILURE);
    }
    
    watch_fd = inotify_add_watch(notify_fd, imagepath, IN_CREATE);
    if (watch_fd < 0) {
        perror("cannot watch image directory");
        exit(EXIT_FAILURE);
    }
    while (ts_alive){
        
        DIR *dir = opendir(imagepath);          //Looping Open/Close to simulate infinite images in folder 
        
        while ((dPath = readdir(dir)) != NULL) {
            char *buffer = malloc(strlen(imagepath) + strlen(dPath->d_name) + 1);
            if (buffer == NULL) {
                printf("Out of memory.");
            } else {
                strcpy(buffer, imagepath);
                strcat(buffer, dPath->d_name);
                tmpPath = buffer;
            }
            char *dot = strrchr(tmpPath, '.');
            if (dot && !strcmp(dot, ".roe")) {  //Check if file is *.roe
            
                currentImg->filePath = tmpPath;
                tm_enqueue(&roeQueue, currentImg); //Add image to queue
            
            }
        }
        
        closedir(dir);                          //Looping Open/Close to simulate infinite images in folder  
        
    }
    return NULL;
}

void* telem(void* arg){
    //fdown = init_hkdown_serial_connection();                  //Open housekeeping downlink
    FILE *fp;
    int rc;
    
    while (ts_alive){
        if (roeQueue.count != 0) {
            /*dequeue*/
            fp = fopen(roeQueue.first->filePath, "r");         //Open file data
            if (fp == NULL) {
                printf("fopen(%s) error=%d %s\n", roeQueue.first->filePath, errno, strerror(errno));
            }
           
            int i, j;                                               
            char * tempName = strrchr(roeQueue.first->filePath, '/') + 1;
            int l = 2048, h = 1024;        
//            int tempData[h][l];
            int **  tempData = malloc(sizeof(int*) * l);
            for(i = 0; i < l; i++){
                tempData[i] = malloc(sizeof(int) * h * 3);
            }
            
//            int **tempData2 = malloc(h * sizeof(int *));
//            for (i = 0; i < h; i++) { 
//                tempData2[i] = malloc(l * sizeof(int)); 
//            }
            
            int index = 0;                                      //Create data structure
            
//            while (rc != EOF) {                                //Open image data
                for (i=0; i < l; i++){
                    for (j=0; j < (h*3) ; j++){                      //Read image data
                        tempData[i][j] = 1;
                        rc = fread((&tempData[i][j]), (size_t) 2, 1, fp); 
                    }
//                    index++;
//                    if (index > ((h * 3) - 1)) { break; }
                 }
                 //rc = fread((tempData2[index]), sizeof(int), (l), fp);
                 //index++;
                 //if (index > 1023){ break; }
            
//             }

            if (!ts_alive) break;                               //If the program has terminated, break out of the loop

            if ((&roeQueue)->first != NULL){  
                
                fseek(fp, 0, SEEK_END); // seek to end of file
                int size = ftell(fp); // get current file pointer
                fseek(fp, 0, SEEK_SET);
                roeImage_t* r = constructImage(tempName, tempData, size);

                /*enqueue onto hkdown queue here*/
//                if (r->status) {
//                    sendImage(r, fdown);
//                    //recordPacket(r);                        //save packet to logfile for debugging
//                    //record(asprintf("Sent:       %s%s%s%s%s%s\n", p->timeStamp, p->type, p->subtype, p->dataLength, p->data, p->checksum));            
//                    free(r);                                    //Clean up after packet is sent
//                } else {
//                    record("Bad send Packet\n");
//                }

                tm_dequeue(&roeQueue);                             //dequeue the next packet once it becomes available
            }
        }
        else {
            struct timespec timeToWait;
            struct timeval now;

            gettimeofday(&now, NULL);
            timeToWait.tv_sec = now.tv_sec + 1;
            timeToWait.tv_nsec = 0;

            /*jackson what are you doing here? the dequeue operation should do this for you*/
                                                                /*The thread must be locked for pthread_cond_timedwait() to work*/
//            pthread_mutex_lock(&roeQueue.lock);
//            while (roeQueue.count == 0){                       //unlocks the mutex and waits on the conditional variable
//                pthread_cond_timedwait(&roeQueue.cond, &roeQueue.lock, &timeToWait);        
//                //pthread_cond_wait(&queue->cond, &queue->lock);
//            }
        }
    }
    return NULL;
}

