
/*
 * NAME: 
 * func_avhrr2hdf
 * 
 * PURPOSE: 
 * Functions used by avhrr2hdf
 *
 * AUTHOR: Steinar Eastwood, met.no, 06.04.2006
 *
 * MODIFIED: 
 * Øystein Godøy, METNO/FOU, 05.05.2009: Adaptions to new libraries, no
 * fancy changes as the format is only used in support of NVE and is
 * likely to be removed in the future...
 *
 * CVS_ID:
 * $Id: func_avhrr2hdf.c,v 1.3 2009-05-14 12:06:33 steingod Exp $
 */ 

#include <avhrr2hdf.h>
/*
#include <avhrr2hdf_extra.h>
*/
#include <time.h>

/*
 * Convert METSAT values to HDF storage values.
 */
int calc_avhrr_channels(fmio_img img, float *ch1, float *ch2, float *ch3b,
	float *ch4, float *ch5, float *ch3a, float *soz, float *saz) {

    char *where="calc_avhrr_channels";
    long i, j;
    int xc, yc, ret, angarrX, angarrY;
    float A1, A2, A3, T3, T4, T5;
    float sozval, sazval;
    fmscale cal;
    fmtime date;
    fmsec1970 utctime, tst;
    fmucsref ucsref;
    fmucspos upos;
    fmgeopos gpos;
    fmindex xypos;
    fmio_subtrack subt;
    fmangreq  angreq;
    fmangles *angarr;

    fm_img2slopes(img, &cal);
    fm_img2fmucsref(img, &ucsref);
    upos.eastings = img.Bx;
    upos.northings = img.By;
    fm_img2fmtime(img, &date);
    angreq.ref = ucsref;
    angreq.date = date;
    angreq.myproj = MI;
    angreq.subtrack.gpos = (fmgeopos *) malloc(img.numtrack*sizeof(fmgeopos));
    for (i=0;i<img.numtrack;i++) {
	angreq.subtrack.gpos[i].lat = img.track[i].latitude;
	angreq.subtrack.gpos[i].lon = img.track[i].longitude;
    }
    angreq.subtrack.npoints = img.numtrack;

    fmlogmsg(where,"Unpacking AVHRR channels and estimating observation geometry.");

    /* Calculate angles for each ANGPIXDIST'th column and row */
    angarrX = img.iw/ANGPIXDIST;
    angarrY = img.ih/ANGPIXDIST;
    angarr = (fmangles *) malloc(angarrX*angarrY*sizeof(fmangles));
    for (yc=0;yc<img.ih;yc++) {
	for (xc=0;xc<img.iw;xc++) {
	    if (xc%ANGPIXDIST == 0 && yc%ANGPIXDIST == 0) {
		xypos.col = xc;
		xypos.row = yc;
		gpos=fmucs2geo(upos,MI);
		j = fmivec((int)(xc/ANGPIXDIST),(int)(yc/ANGPIXDIST),(int)(img.iw/ANGPIXDIST));
		angreq.ll = gpos;
		angreq.ind = xypos;
		if (! fmangest(angreq,&angarr[j])) {
		    fmerrmsg(where,
		    "Observation geometry estimation failed for pixel %d, %d",
		    xc, yc);
		}
	    }
	}
    }

    for (yc=0;yc<img.ih;yc++) {
	for (xc=0;xc<img.iw;xc++) {

	    i = fmivec(xc,yc,img.iw);

	    if (img.image[4][i] > 0 || img.image[0][i] > 0) {
		j = fmivec(mini(xc/ANGPIXDIST,angarrX),mini(yc/ANGPIXDIST,angarrY),angarrX);
		sozval = (angarr[j]).soz;
		sazval = (angarr[j]).saz;
	    } else {
		sozval = -999.9;
		sazval = -999.9;
	    }

	    /*if (img.image[0][i] > 0) {  A1 = calib(img.image[0][i], 1, cal);
	     * }*/
	    if (img.image[0][i] > 0) {  
		A1 = unpack_fmimage_ushort(img.image[0][i], cal, 1);  
	    }
	    else {  
		A1 = -999.9;  
	    }

	    /*if (img.image[1][i] > 0) {  A2 = calib(img.image[1][i], 2, cal);
	     * }*/
	    if (img.image[1][i] > 0) {  
		A2 = unpack_fmimage_ushort(img.image[1][i], cal, 1);  
	    } else {  
		A2 = -999.9;  
	    }

	    /*if (img.image[2][i] > 0) {  T3 = calib(img.image[2][i], 3, cal);
	     * }*/
	    if (img.image[2][i] > 0) {  
		T3 = unpack_fmimage_ushort(img.image[2][i], cal, 2);  
	    } else {  
		T3 = -999.9; 
	    }

	    /*if (img.image[3][i] > 0) {  T4 = calib(img.image[3][i], 4, cal);
	     * }*/
	    if (img.image[3][i] > 0) {  
		T4 = unpack_fmimage_ushort(img.image[3][i], cal, 2);  
	    } else {  
		T4 = -999.9;  
	    }

	    /*if (img.image[4][i] > 0) {  T5 = calib(img.image[4][i], 5, cal);
	     * }*/
	    if (img.image[4][i] > 0) {  
		T5 = unpack_fmimage_ushort(img.image[4][i], cal, 2);  
	    } else {  
		T5 = -999.9;  
	    }

	    /*if (img.image[5][i] > 0) {  A3 = calib(img.image[5][i], 6, cal);
	     * }*/
	    if (img.image[5][i] > 0) {  
		A3 = unpack_fmimage_ushort(img.image[5][i], cal, 1);  
	    } else {  
		A3 = -999.9;  
	    }

	    ch1[i] = A1;
	    ch2[i] = A2;
	    ch3b[i] = T3;
	    ch4[i] = T4;
	    ch5[i] = T5;
	    ch3a[i] = A3;
	    soz[i] = sozval;
	    saz[i] = sazval;
	}
    }

    return(FM_OK);
}

/*
 * Create HDF5 file with AVHRR data.
 */
int create_HDF_avhrr_file(PRODhead avhrrhead, char *outfile, float *ch1, 
	float *ch2, float *ch3b, float *ch4,
	float *ch5, float *ch3a, float *soz, float *saz) {

    char *where="create_HDF_avhrr_file";
    short status;
    int i, imgsize;
    osihdf avhrrh5p;
    osi_dtype avhrr_ft[AVHRRH5P_LEVS];
    char *avhrr_desc[AVHRRH5P_LEVS];

    fmlogmsg(where,"Creating HDF file with AVHRR fields.");

    /* Initiate HDF5 object for AVHRR product */

    init_osihdf(&avhrrh5p);
    sprintf(avhrrh5p.h.source, "%s", avhrrhead.source);
    sprintf(avhrrh5p.h.product, "%s", avhrrhead.product);
    sprintf(avhrrh5p.h.area, "%s", avhrrhead.area);
    avhrrh5p.h.iw = avhrrhead.iw;
    avhrrh5p.h.ih = avhrrhead.ih;
    avhrrh5p.h.z = AVHRRH5P_LEVS;
    avhrrh5p.h.Ax = avhrrhead.Ax;
    avhrrh5p.h.Ay = avhrrhead.Ay;
    avhrrh5p.h.Bx = avhrrhead.Bx;
    avhrrh5p.h.By = avhrrhead.By;
    avhrrh5p.h.year = avhrrhead.year;
    avhrrh5p.h.month = avhrrhead.month;
    avhrrh5p.h.day = avhrrhead.day;
    avhrrh5p.h.hour = avhrrhead.hour;
    avhrrh5p.h.minute = avhrrhead.minute;
    for (i=0;i<AVHRRH5P_LEVS;i++) {
	avhrr_ft[i] = OSI_FLOAT;
    }
    avhrr_desc[0] = (char *) malloc(8*sizeof(char));
    sprintf(avhrr_desc[0], "CH1");
    avhrr_desc[1] = (char *) malloc(8*sizeof(char));
    sprintf(avhrr_desc[1], "CH2");
    avhrr_desc[2] = (char *) malloc(8*sizeof(char));
    sprintf(avhrr_desc[2], "CH3B");
    avhrr_desc[3] = (char *) malloc(8*sizeof(char));
    sprintf(avhrr_desc[3], "CH4");
    avhrr_desc[4] = (char *) malloc(8*sizeof(char));
    sprintf(avhrr_desc[4], "CH5");
    avhrr_desc[5] = (char *) malloc(8*sizeof(char));
    sprintf(avhrr_desc[5], "CH3A");
    avhrr_desc[6] = (char *) malloc(8*sizeof(char));
    sprintf(avhrr_desc[6], "SOZ");
    avhrr_desc[7] = (char *) malloc(8*sizeof(char));
    sprintf(avhrr_desc[7], "SAZ");


    status = malloc_osihdf(&avhrrh5p,avhrr_ft,avhrr_desc);
    if (status != 0) {
	fprintf(stderr,"\n\tERROR! In allocating memory (malloc_osihdf)\n");
	exit(3);
    }

    /* Copy AVHRR fields to avhrrh5p object */
    imgsize = avhrrhead.iw*avhrrhead.ih;
    for (i=0;i<imgsize;i++) {
	((float *) avhrrh5p.d[0].data)[i] = ch1[i];
	((float *) avhrrh5p.d[1].data)[i] = ch2[i];
	((float *) avhrrh5p.d[2].data)[i] = ch3b[i];
	((float *) avhrrh5p.d[3].data)[i] = ch4[i];
	((float *) avhrrh5p.d[4].data)[i] = ch5[i];
	((float *) avhrrh5p.d[5].data)[i] = ch3a[i];
	((float *) avhrrh5p.d[6].data)[i] = soz[i];
	((float *) avhrrh5p.d[7].data)[i] = saz[i];
    }

    status = store_hdf5_product(outfile,avhrrh5p);
    if (status != 0) {
	fprintf(stderr,"\n\tERROR: store_hdf5_product encountered an error.\n");
	exit(2);
    }

    if (free_osihdf(&avhrrh5p) != 0) {
	fprintf(stderr,"\n\tERROR! Could not free avhrrh5p properly.");
	exit(3);
    }

    return(FM_OK);
}

/*
 * Functions to return min/max values of integers and floats.
 */

int mini(int i1, int i2) {
    if (i1 <= i2) { return(i1); }
    else { return(i2); }
}

int maxi(int i1, int i2) {
    if (i1 >= i2) { return(i1); }
    else { return(i2); }
}

int minf(float f1, float f2) {
    if (f1 <= f2) { return(f1); }
    else { return(f2); }
}

int maxf(float f1, float f2) {
    if (f1 >= f2) { return(f1); }
    else { return(f2); }
}

