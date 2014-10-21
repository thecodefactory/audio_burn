/*
  libaudioburn - an audio cd burning library
  Copyright (C) 2003  Neill Miller
  This file is part of libaudioburn.

  libaudioburn is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  libaudioburn is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with libaudioburn; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA  02111-1307, USA.
*/
#include "libaudioburn.h"

int cdrecord_initialize(settings_t *settings)
{
    int ret = 1;
    struct stat statbuf;

    if (settings)
    {
        if (which("cdrecord",settings->cdrecord_path,
                  MAX_PATH_LEN) == 0)
        {
            ret = 0;
        }
        else
        {
            memset(&statbuf,0,sizeof(statbuf));

            if (stat(CDRECORD_PATH,&statbuf) ||
                (!(statbuf.st_mode & S_IXUSR)))
            {
                fprintf(stderr,"Cannot find required program "
                        "cdrecord.\nPlease install this program "
                        "either from your vendor, or visit:\n\n%s\n",
                        CDRECORD_DOWNLOAD_SITE);
            }
            else
            {
                memcpy(settings->cdrecord_path,CDRECORD_PATH,
                       MIN(strlen(CDRECORD_PATH),MAX_PATH_LEN-1));
                ret = 0;
            }
        }
    }
    return ret;
}

int cdrecord_get_recorder_info(settings_t *settings,
                               cdrecorder_t *recorders)
{
    int i = 0;
    int ret = 1;
    int infd = 0;
    int outfd = 0;
    int scsibus = 0;
    int current = 0;
    int is_null_device = 0;
    int probe_complete = 0;
    ssize_t nread = 0;
    pid_t child_pid = 0;
    char buf[MAX_BUF_LEN];
    char *ptr = (char *)0;
    char *start = (char *)0;
    char *args[4] = {(char *)0,(char *)0,(char *)0,(char *)0};
    int arg_index = 0;

    if (settings && recorders &&
        (strlen(settings->atapi_device) == 0) &&
        (elevate_privileges() == 0))
    {
        memset(recorders,0,sizeof(recorders));

        args[arg_index++] = settings->cdrecord_path;
        if (settings->atapi)
        {
            args[arg_index++] = "dev=ATAPI";
        }
        args[arg_index++] = "-scanbus";
        args[arg_index++] = NULL;

        if (fork_execute(args[0],args,&infd,
                         &outfd,NULL,CHILD_WAIT,&child_pid) == 0)
        {
            drop_privileges();

            memset(buf,0,MAX_BUF_LEN);
            nread = read(outfd,buf,MAX_BUF_LEN);
            if (nread > 0)
            {
                /* check if we got valid output */
                if ((strstr(buf,"For possible targets try") == 0) &&
                    (strstr(buf,"cdrecord dev=help") == 0))
                {
                    ptr = strstr(buf,"scsibus");

                    if (ptr)
                    {
                        ptr += strlen("scsibus");
                        start = ptr;
                        ptr++;
                        *ptr = '\0';
                        ptr++;
                        scsibus = atoi(start);
                    }
                    else
                    {
                        goto probing_complete;
                    }

                    /* extract information about all recorder devices */
                    while(!probe_complete && (current < MAX_NUM_DEVICES))
                    {
                        is_null_device = 0;

                        /* advance to the next bus,target,lun field */
                        while(*ptr && (!isdigit((int)*ptr)))
                        {
                          advance_to_next_bus_target_lun:
                            ptr++;
                            if ((ssize_t)(ptr - buf) > nread)
                            {
                                probe_complete = 1;
                                break;
                            }
                        }

                        if (*ptr == '\0')
                        {
                            probe_complete = 1;
                        }

                        if (probe_complete)
                        {
                            break;
                        }

                        if (ptr[1] == ':')
                        {
                            goto advance_to_next_bus_target_lun;
                        }

                        start = ptr;
                        ptr++;
                        *ptr = '\0';
                        ptr++;

                        recorders->scsibuses[current] = atoi(start);
                        assert(recorders->scsibuses[current] == scsibus);

                        start = ptr;
                        ptr++;
                        *ptr = '\0';
                        ptr++;
                        recorders->targets[current] = atoi(start);

                        start = ptr;
                        ptr++;
                        *ptr = '\0';
                        ptr++;
                        recorders->luns[current] = atoi(start);

                      advance_to_label:
                        /* advance to the device label field */
                        while(*ptr && (!isdigit((int)*ptr)))
                        {
                            ptr++;
                        }

                        /* check if we've wrapped to the next scsibus */
                        if (ptr && (strlen(ptr) > 4) &&
                            (ptr[1] == ',') && (ptr[3] == ','))
                        {
                            /* and keep advancing if so */
                            ptr += 5;
                            goto advance_to_label;
                        }

                        start = ptr;
                        while(*ptr && (*ptr != ')'))
                        {
                            ptr++;
                        }
                        *ptr = '\0';
                        ptr += 2;

                        recorders->labels[current] = atoi(start);

                        /* blank and copy in the entire device name */
                        memset(recorders->devices[current],0,
                               MAX_DEVICENAME_LEN);

                        i = 0;
                        while(*ptr && (*ptr != '\n') &&
                              (i < MAX_DEVICENAME_LEN))
                        {
                            if ((*ptr == '*') ||
                                (strstr(ptr,"Removable not present")))
                            {
                                is_null_device = 1;
                                break;
                            }
                            recorders->devices[current][i++] = *ptr;
                            ptr++;
                        }

                        /* make sure ptr is advanced to next line */
                        while(ptr && (*ptr != '\n')) ptr++;
                        ptr++;

                        /*
                          greedily increment scsibus if necessary
                          for next iteration
                        */
                        if (ptr && (strlen(ptr) > 7) &&
                            (memcmp(ptr,"scsibus",7) == 0))
                        {
                            scsibus++;
                        }
                        if (!probe_complete && !is_null_device)
                        {
                            recorders->num_devices++;
                        }
                        if (!is_null_device)
                        {
                            /*
                              here's our last chance to weed out the
                              device in the case that it's not a
                              CD-ROM device.

                              For now, if the device description does
                              NOT contain the word, "CD-ROM", we're
                              going to ignore it.
                            */
                            if (strstr(recorders->devices[current],
                                       "CD-ROM"))
                            {
                                current++;
                            }
                            else
                            {
                                /*
                                  bump back down the number of
                                  recorder devices since this one was
                                  retracted
                                 */
                                recorders->num_devices--;
                            }
                        }
                    }
                }
            }

          probing_complete:

            close(infd);
            close(outfd);

            ret = ((recorders->num_devices) ? 0 : 1);
        }
        else
        {
            drop_privileges();
        }
    }
    else if (strlen(settings->atapi_device))
    {
        /*
          if an ATAPI device is specified explicitly, do not probe
          at all, just make sure that the device file exists
        */
        struct stat statbuf;
        memset(&statbuf, 0, sizeof(struct stat));

        ret = stat(settings->atapi_device, &statbuf);
        if (ret)
        {
            fprintf(stderr,"\nError:\n");
            fprintf(stderr,"Specified device file %s does not "
                    "exist!\n", settings->atapi_device);
            fprintf(stderr,"Please verify that this device is "
                    "correct\n\n");
        }
    }
    return ret;
}

void cdrecord_print_recorder_info(cdrecorder_t *recorders)
{
    int i = 0;

    if (recorders)
    {
        if (recorders->num_devices > 1)
        {
            printf("*** Detected %d CD-ROM Devices ***\n\n",
                   recorders->num_devices);
        }
        else
        {
            printf("*** Detected %d CD-ROM Device ***\n\n",
                   recorders->num_devices);
        }

        for(i = 0; i < recorders->num_devices; i++)
        {
            printf("Device %d: %s\n",i,recorders->devices[i]);
            printf("\tscsibus is %d, target is %d, lun is %d "
                   "(label is %d)\n",
                   recorders->scsibuses[i],
                   recorders->targets[i],
                   recorders->luns[i],
                   recorders->labels[i]);
        }
    }
}

int cdrecord_burn_audio_cd(settings_t *settings,
                           cdrecorder_t *recorders,
                           fn_callback_t fnptr)
{
    int i = 0;
    int ret = 1;
    int arg_index = 0;
    char *args[MAX_NUM_CDRECORD_ARGS] = {(char*)0};
    char device[MAX_DEVICE_OPTION_LEN] = {0};
    char speed[MAX_SPEED_OPTION_LEN] = {0};
    char gracetime_str[MAX_GRACETIME_STR_LEN] = {0};

    if (settings && recorders)
    {
        args[arg_index++] = settings->cdrecord_path;

        if (settings->verbose)
        {
            args[arg_index++] = "-v";
        }
        if (settings->dummy_run)
        {
            args[arg_index++] = "-dummy";
        }
        if (settings->gracetime)
        {
            snprintf(gracetime_str,MAX_GRACETIME_STR_LEN,"gracetime=%d",
                     settings->gracetime);
            args[arg_index++] = gracetime_str;
        }
        if (settings->burn_free)
        {
            args[arg_index++] = "driveropts=burnfree";
        }

        if (strlen(settings->atapi_device))
        {
            snprintf(device,MAX_DEVICE_OPTION_LEN,"dev=%s",
                     settings->atapi_device);
        }
        else if (settings->atapi)
        {
            snprintf(
                device,MAX_DEVICE_OPTION_LEN,"dev=ATAPI:%d,%d,%d",
                recorders->scsibuses[settings->preferred_device_number],
                recorders->targets[settings->preferred_device_number],
                recorders->luns[settings->preferred_device_number]);
        }
        else
        {
            snprintf(
                device,MAX_DEVICE_OPTION_LEN,"dev=%d,%d,%d",
                recorders->scsibuses[settings->preferred_device_number],
                recorders->targets[settings->preferred_device_number],
                recorders->luns[settings->preferred_device_number]);
        }
        args[arg_index++] = device;

        if (settings->dao)
        {
            args[arg_index++] = "-dao";
        }
        else
        {
            args[arg_index++] = "-tao";
        }

        if (settings->eject)
        {
            args[arg_index++] = "-eject";
        }

        snprintf(speed,MAX_SPEED_OPTION_LEN,"speed=%d",
                 settings->speed);
        args[arg_index++] = speed;

        args[arg_index++] = "-pad";
        args[arg_index++] = "-audio";

        for(i = 0; i < settings->num_output_files; i++)
        {
            if (arg_index > (MAX_NUM_CDRECORD_ARGS - 1))
            {
                fprintf(stderr,"Cannot write any more tracks!\n");
                return ret;
            }
            args[arg_index++] = settings->output_files[i];
        }

        if (settings->verbose)
        {
            printf("cdrecord command is:\n");
            for(i = 0; i < arg_index; i++)
            {
                printf("%s ",args[i]);
            }
            printf("\n");
        }

        if (elevate_privileges() == 0)
        {
            ret = fork_execute_with_io_callback(
                settings->cdrecord_path, args,fnptr, CHILD_IO_ERR);
        }
        drop_privileges();
    }
    return ret;
}
