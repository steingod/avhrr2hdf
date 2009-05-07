/*
 * NAME: 
 * avhrr2hdf.h
 *
 * PURPOSE: 
 * Header file for conversion of AVHRR METSAT files to HDF5.
 *
 * AUTHOR: 
 * Steinar Eastwood, met.no/FoU, 14.01.2003
 * 
 * MODIFIED: 
 * SE, met.no, 12.01.2004  Added snow field values for short integer.
 * Øystein Godøy, METNO/FOU, 05.05.2009: Adaptions to new libraries, no
 * fancy changes as the format is only used in support of NVE and is
 * likely to be removed in the future...
 *
 * CVS_ID:
 * $Id: avhrr2hdf.h,v 1.1 2009-05-07 11:13:32 steingod Exp $
 */

#ifndef _AVHRR2HDF_H
#define _AVHRR2HDF_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <tiffio.h>
#include <fmutil.h>
#include <fmio.h>
#include <safhdf.h>

typedef struct {
  int stnr;
  int snr;
  int yy;
  int mm;
  int dd;
  float height;
  float lat;
  float lon;
  float ss;
  float sd;
  int xpos;
  int ypos;
} OBSklimdb;

/* Parameter definition */

#define AVHRRH5P_LEVS 8

#define ANGPIXDIST 10           /* Distance between each pixel to calc angles */

#define DEBUG 1             /* Set to 1 if debug messages are to be printed, 0 if not */

#define BASENMSAT  "snowavhrr"  /* Basename for snow product from AVHRR data */
#define BASENMOBS  "snowobs"    /* Basename for snow product from ground obs */
#define BASENMANAM "snow_mon"   /* Basename for monthly analysed snow product */
#define BASENMANAS "snow_seas"   /* Basename for seasonal analysed snow product */
#define BASENMANAY "snow_year"   /* Basename for yearly analysed snow product */
#define BASENMANAN "snow_nsd"    /* Basename for yearly DCCI NSD snow product */

/* Parameters relatet to observations */
#define OBSINFRAD 75        /* Influence radius of observations (in km) */
#define PARSD 1             /* Code to identify that parameter is snow cover */
#define PARSS 2             /* Code to identify that parameter is snow depth */


/* Parameters related to interpolation */
#define XDIRSBOX 17      /* Half width in x dir of sliding box for interpolation */
#define YDIRSBOX 17      /* Half width in y dir of sliding box for interpolation */
#define XDIRSBOXOUT 50   /* Half width in x dir of outer sliding box for interpolation */
#define YDIRSBOXOUT 50   /* Half width in y dir of outer sliding box for interpolation */

#define HIGH_THR_SNOW  9000.0   /* Initialization value for heighest and lowest */
#define LOW_THR_NOSNOW   -9.9   /* altitude with/without snow */
#define THR_EXT_ELEV     50.0   /* Height a threshold can be extended when only snow or */
                                /* no snow has been observed within box. */


/* Fixed values used */
#define ECSN_MINSCOV   0.0        /* Minimum snow cover value */
#define ECSN_MAXSCOV   1.0        /* Maximum snow cover value */
#define ECSN_MINSDEP   0.0        /* Minimum snow depth value */
#define ECSN_MAXSDEP 900.0        /* Maximum snow depth value */

/* EuroClim SNOW field values */
#define ECSN_SNOW      1.0
#define ECSN_NOSNOW    0.0
#define ECSN_LAND    -99.0
#define ECSN_CLOUD  -199.0
#define ECSN_SEA    -399.0
#define ECSN_UNCL   -599.0
#define ECSN_UNDEF  -999.0

#define ECSNI_LAND   32700
#define ECSNI_CLOUD  32701
#define ECSNI_SEA    32703
#define ECSNI_UNCL   32705
#define ECSNI_UNDEF  32709

/* EuroClim QFLAG field values */
#define ECQF_UNDEF 0;
#define ECQF_QF1 1
#define ECQF_QF2 2
#define ECQF_QF3 3
#define ECQF_QF4 4
#define ECQF_QF5 5
#define ECQF_QF6 6
#define ECQF_QF7 7
#define ECQF_QF8 8
#define ECQF_QF9 9

/* Cloud mask pixel values */
#define MASK_UNPROC   0 
#define MASK_CLFREE   1
#define MASK_CLOUD    2
#define MASK_LAND     3
#define MASK_SEA      4
#define MASK_SNOWICE  5
#define MASK_UNCLASS  6


/* Paramters for result grid in 1.5km and 25km resolution */
#define ECSN1_AX 1.5
#define ECSN1_AY 1.5
#define ECSN1_BX -333.5
#define ECSN1_BY -1676.5
#define ECSN1_IW 1200
#define ECSN1_IH 1200

#define ECSN25_AX 25.0
#define ECSN25_AY 25.0
#define ECSN25_BX -333.5
#define ECSN25_BY -1676.5
#define ECSN25_IW 100
#define ECSN25_IH 100

/* Common HDF5 description parameters */
#define SNOWH5P_AREA "SCANDINAVIA"        /* Product area passage product, not used */

/* Parameters for daily HDF5 SNOW files */
#define SNOWH5P_LEVS_DAY 2                /* Number of levels of data on HDF5 file */
#define SNOWH5P_PROD_SDEP "SNOWDEPTH"     /* Product name snow depth product */
#define SNOWH5P_PROD_SCOV "SNOWCOVER"     /* Product name snow cover product */
#define SNOWH5P_SOUR_OBS  "OBS"	          /* Product source observation product */
#define SNOWH5P_SOUR_DSAT "NOAA"          /* Product source satellite product */
#define SNOWH5P_SOUR_ANA  "OBS+SAT"       /* Product source analysed product */
#define SNOWH5P_DESC_SCOV  "SNOWCOVER"    /* Product desc snow cover */
#define SNOWH5P_DESC_SDEP  "SNOWDEPTH"    /* Product desc snow depth */
#define SNOWH5P_DESC_QFLAG "SNOWQFLAG"    /* Product desc qual flag */

/* Parameters for analysed HDF5 SNOW files  */
#define SNOWH5P_LEVS_MON 6                 /* Number of levels of data on HDF5 file */
#define SNOWH5P_PROD_SNMO "SNOWCOV_MONTH"  /* Product name monthly snow cover product */
#define SNOWH5P_PROD_SNSE "SNOWCOV_SEASON" /* Product name seasonal snow cover product */
#define SNOWH5P_PROD_SNYY "SNOWCOV_YEAR"   /* Product name yearly snow cover product */
#define SNOWH5P_PROD_NSD "SNOW_DCCI_NSD"   /* Product name DCCI NSD snow cover product */
#define SNOWH5P_DESC_MEAN "SNOW_MEAN"      /* Product description of snow mean */
#define SNOWH5P_DESC_MIN "SNOW_MIN"        /* Product description of snow minimum */
#define SNOWH5P_DESC_MAX "SNOW_MAX"        /* Product description of snow maximum */
#define SNOWH5P_DESC_STD "SNOW_STD"        /* Product description of snow standard deviation */
#define SNOWH5P_DESC_NUM "SNOW_NUM"        /* Product description of number of snow obs */
#define SNOWH5P_DESC_REL "SNOW_REL"        /* Product description of snow obs reliability */
#define SNOWH5P_DESC_NSD "NUM_SNOWDAYS"    /* Product desc number of snow days */


/* 
 * Function prototypes. 
 */


int create_HDF_snow_file(PRODhead snowhead, char *outfile, float *snow, 
			 unsigned short *qflag);

int create_HDF_snow_analysed_file(PRODhead snowhead, char *outfile, float *snow,
				 float *minsnow, float *maxsnow, float *stdsnow,
				 unsigned short *numsnowobs, float *relsnow);

int create_HDF_avhrr_file(PRODhead avhrrhead, char *outfile, float *ch1, 
			  float *ch2, float *ch3b, float *ch4,
			  float *ch5, float *ch3a, float *soz, float *saz);

int read_HDF_snowdata(char *infile, float **snow, PRODhead *snowhead, 
		      unsigned short **qflag);

int read_HDF_analysed_snowdata(char *infile, float **snow, float **minsnow, 
			      float **maxsnow, float **stdsnow, 
			      unsigned short **numsnowobs, float **relsnow,
			      PRODhead *snowhead);

int mini(int i1, int i2);

int minf(float f1, float f2);

int maxi(int i1, int i2);

int maxf(float f1, float f2);

int merge_snowcov_fields(char **fnames, int numf, float *snow, PRODhead *snowhead,
			 fmucspos finalgrid);

int merge_snowdep_fields(char **fnames, int numf, float *snow, PRODhead *snowhead,
			 fmucspos finalgrid);

void usage();



/*
 * End function prototypes.
 */

#endif /* _AVHRR2HDF_H */
