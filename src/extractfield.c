/*
 * \file    extractfield.c
 * \author  Scott Wales (scott.wales@unimelb.edu.au)
 * \brief   Extract a single STASH field into a netcdf file
 * 
 * Copyright 2013 ARC Centre of Excellence for Climate System Science
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */ 

#include "fieldsfile.h"
#include "list.h"
#include <argp.h>
#include <assert.h>
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>

const char * doc = "Extracts a single STASH variable into a netcdf file";

const char * argp_program_version     = "0";
const char * argp_program_bug_address = "scott.wales@unimelb.edu.au";

struct args {
    const char * filename;
    const char * output;
    int stash;
};

const char * args_doc = "FILENAME STASHCODE OUTPUT";
error_t parse_opt(int key, char * arg, struct argp_state * state){
    struct args * args = state->input;
    switch (key){
        case ARGP_KEY_ARG:
            // Unnamed argument
            switch (state->arg_num){
                case 0:
                    args->filename = arg;
                    break;
                case 1: 
                    {
                        int match = sscanf(arg,"%d",&(args->stash));
                        if (!match) argp_usage(state);
                        break;
                    }
                case 2:
                    args->output = arg;
                    break;
                default:
                    argp_usage(state);
                    break;
            }
            break;
        case ARGP_KEY_END:
            // End of arguments
            if (state->arg_num < 2) argp_usage(state);
            break;
        default:
            // Unknown argument
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}


int main(int argc, char ** argv){
    struct args args;
    struct argp argp = {
        .doc = doc,
        .args_doc = args_doc,
        .parser = parse_opt,
    };
    error_t err = argp_parse(&argp, argc, argv, 0, NULL, &args);

    struct FieldsFile * ff = OpenFieldsFile(args.filename);

    // We need a list of unique values for each dimension. This is done using a
    // list that stores unique values (it's really an ordered set).
    struct list * timelist = NULL;
    struct list * heightlist = NULL;
    struct list * pseudolist = NULL;
    int size[2];
    double origin[2];
    double step[2];

    // Get the dimensions of the field
    size_t found = 0;
    for (size_t i=0;i<ff->header->field_count;++i){
        if (ff->lookup[i].stash_code == args.stash){
            // Each value will only be added once
            ListAdd(&timelist,FFDateToUnixTime(ff->lookup[i].valid_time)); 
            ListAdd(&heightlist,ff->lookup[i].heightlevel);
            ListAdd(&pseudolist,ff->lookup[i].pseudo_dimension);

            // Horizontal dimensions
            size[0] = ff->lookup[i].rows;
            size[1] = ff->lookup[i].columns;
            origin[0] = ff->lookup[i].origin_latitude;
            origin[1] = ff->lookup[i].origin_longitude;
            step[0] = ff->lookup[i].latitude_interval;
            step[1] = ff->lookup[i].longitude_interval;

            ++found;
        }
    }
    if (!found){
        fprintf(stderr, "STASH %d not present in file\n",args.stash);
        exit(1);
    }

    // Now to write the field out as Netcdf. Firstly we need to write out the
    // dimensions. At the moment metadata is ignored, units &c will need to be
    // added elsewhere. The fieldsfile format also allows for alternate grid
    // types, we assume a regular grid here.

    // Set the dimension values
    double * lats = malloc(size[0]*sizeof(*lats));
    double * lons = malloc(size[1]*sizeof(*lats));
    double * times = NULL;
    double * heights = NULL;
    double * pseudos = NULL;

    for (int i=0;i<size[0];++i) lats[i] = origin[0]+step[0]*(i+1);
    for (int i=0;i<size[1];++i) lons[i] = origin[1]+step[1]*(i+1);
    ListToArray(&times,timelist);
    ListToArray(&heights,heightlist);
    ListToArray(&pseudos,pseudolist);

    int out; // output file handle
    int errc = nc_create(args.output, NC_CLOBBER, &out);
    assert(errc == NC_NOERR);

    // Declare dimensions
    int dimtime,dimheight,dimbin,dimlat,dimlon;
    errc |= nc_def_dim(out,"time",ListCount(timelist),&dimtime);
    errc |= nc_def_dim(out,"height",ListCount(heightlist),&dimheight);
    errc |= nc_def_dim(out,"bin",ListCount(pseudolist),&dimbin);
    errc |= nc_def_dim(out,"grid_latitude",size[0],&dimlat);
    errc |= nc_def_dim(out,"grid_longitude",size[1],&dimlon);
    assert(errc == NC_NOERR);

    // Declare dimension arrays
    int vartime,varheight,varbin,varlat,varlon;
    errc |= nc_def_var(out,"time",NC_DOUBLE,1,&dimtime,&vartime);
    errc |= nc_def_var(out,"height",NC_DOUBLE,1,&dimheight,&varheight);
    errc |= nc_def_var(out,"bin",NC_DOUBLE,1,&dimbin,&varbin);
    errc |= nc_def_var(out,"grid_latitude",NC_DOUBLE,1,&dimlat,&varlat);
    errc |= nc_def_var(out,"grid_longitude",NC_DOUBLE,1,&dimlon,&varlon);
    assert(errc == NC_NOERR);

    // Declare data array
    char * stashname = NULL;
    asprintf(&stashname,"stash.%d",args.stash);
    int dims[] = {dimtime,dimheight,dimbin,dimlat,dimlon};
    int varstash;
    errc = nc_def_var(out,stashname,NC_DOUBLE,5,dims,&varstash);
    if (errc != NC_NOERR){
        fprintf(stderr,"%s\n",nc_strerror(errc));
        exit(-1);
    }
    free(stashname);

    nc_enddef(out);

    // Write dimension values
    errc |= nc_put_var_double(out,vartime,times);
    errc |= nc_put_var_double(out,varheight,heights);
    errc |= nc_put_var_double(out,varbin,pseudos);
    errc |= nc_put_var_double(out,varlat,lats);
    errc |= nc_put_var_double(out,varlon,lons);
    assert(errc == NC_NOERR);

    // Write data values layer by layer
    double * data = NULL;
    for (size_t i=0;i<ff->header->field_count;++i){
        if (ff->lookup[i].stash_code == args.stash){
            // Get the index of this slice in time & vertical level
            int timei = ListIndex(timelist,FFDateToUnixTime(ff->lookup[i].valid_time)); 
            int heighti = ListIndex(heightlist,ff->lookup[i].heightlevel);
            int bini = ListIndex(pseudolist,ff->lookup[i].pseudo_dimension);

            // Hyperslice of the field at a single horizontal level
            size_t start[] = { timei, heighti, bini, 0, 0 };
            size_t count[] = { 1, 1, 1, size[0], size[1] };

            // Read the layer from the fieldsfile, write to netcdf
            ReadFieldsFileData(&data,ff,i);
            errc = nc_put_vara_double(out,varstash,start,count,data);
            if (errc != NC_NOERR){
                fprintf(stderr,"%s\n",nc_strerror(errc));
                exit(-1);
            }
        }
    }

    nc_close(out);

    CloseFieldsFile(ff);
}
