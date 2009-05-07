
/*
 * NAME: 
 * avhrr2hdf
 * 
 * PURPOSE: 
 * Convert AVHRR AHA files to HDF5.
 * 
 * INPUT: 
 *    <infile avhrr> : File with AVHRR data on METSAT format.
 *
 * OUTPUT: 
 *    <outf HDF> : Filename for result file on met.no HDF5 format.
 *
 * AUTHOR: 
 * Steinar Eastwood, met.no, 06.04.2006
 * 
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 05.05.2009: Adaptions to new libraries, no
 * fancy changes as the format is only used in support of NVE and is
 * likely to be removed in the future...
 *
 * CVS_ID:
 * $Id: avhrr2hdf.c,v 1.1 2009-05-07 11:13:32 steingod Exp $
 */ 

#include <avhrr2hdf.h>

int main(int argc, char *argv[]) {    

  char *where="avhrr_aha2hdf";
  extern char *optarg;
  int ret, i, status, numobs;
  int iflg, pflg, oflg, aflg;
  float *snow, *ch1, *ch2, *ch3b, *ch4, *ch5, *ch3a, *soz, *saz;
  unsigned short *landmask, *landuse, *elevation, *qflag;
  char *infileSat;
  char *outfHDFavhrr;
  PRODhead avhrrhead;
  fmio_img img;

  /* Interprete commandline arguments */
  iflg = pflg = oflg = aflg = 0;
  while ((ret = getopt(argc, argv, "i:o:")) != EOF) {
    switch (ret) {
    case 'i':
      infileSat = (char *) malloc(strlen(optarg)+1);
      if (!strcpy(infileSat, optarg)) exit(1);
      iflg++;
      break;
    case 'o':
      outfHDFavhrr = (char *) malloc(strlen(optarg)+1);
      if (!strcpy(outfHDFavhrr, optarg)) exit(1);
      oflg++;
      break;
    default:
      usage();
    }
  }
  if (!iflg || !oflg) usage();

  fmlogmsg(where,"Converting AVHRR from METSAT (%s) to HDF5 (%s).", 
  infileSat, outfHDFavhrr);

  /*
   * Read METSAT file
   */
  fprintf(stdout,"\n\tReading satellite data...\n");
  fm_init_fmio_img(&img);
  fm_readdata(infileSat, &img);

  avhrrhead.iw = img.iw;
  avhrrhead.ih = img.ih; 
  avhrrhead.Ax = img.Ax;
  avhrrhead.Ay = img.Ay;
  avhrrhead.Bx = img.Bx;
  avhrrhead.By = img.By;
  avhrrhead.year = img.yy;
  avhrrhead.month = img.mm;
  avhrrhead.day = img.dd;
  avhrrhead.hour = img.ho;
  avhrrhead.minute = img.mi;
  sprintf(avhrrhead.area, "%s", img.area);
  sprintf(avhrrhead.source, "%s", img.sa);
  sprintf(avhrrhead.product, "%s", "AVHRR");

  fprintf(stdout,"\t\tSatellite: %s\n", img.sa);
  fprintf(stdout,"\t\tTime: %02d/%02d/%4d %02d:%02d\n", img.dd, img.mm, img.yy,
	 img.ho, img.mi);

  ch1 = (float *) malloc(avhrrhead.iw*avhrrhead.ih*sizeof(float));
  ch2 = (float *) malloc(avhrrhead.iw*avhrrhead.ih*sizeof(float));
  ch3b = (float *) malloc(avhrrhead.iw*avhrrhead.ih*sizeof(float));
  ch4 = (float *) malloc(avhrrhead.iw*avhrrhead.ih*sizeof(float));
  ch5 = (float *) malloc(avhrrhead.iw*avhrrhead.ih*sizeof(float));
  ch3a = (float *) malloc(avhrrhead.iw*avhrrhead.ih*sizeof(float));
  soz = (float *) malloc(avhrrhead.iw*avhrrhead.ih*sizeof(float));
  saz = (float *) malloc(avhrrhead.iw*avhrrhead.ih*sizeof(float));
  if (!ch1 || !ch2 || !ch3b || !ch4 || !ch4 || 
      !ch5 || !ch3a || !soz || !saz) {
      fmerrmsg(where,"Could not allocate channel storage containers.");
      exit(FM_MEMALL_ERR);
  }
  for (i=0;i<avhrrhead.iw*avhrrhead.ih;i++) {
    ch1[i] = -999.9;
    ch2[i] = -999.9;
    ch3b[i] = -999.9;
    ch4[i] = -999.9;
    ch5[i] = -999.9;
    ch3a[i] = -999.9;
    soz[i] = -999.9;
    saz[i] = -999.9;
  }

  /* 
   * Extract AVHRR data 
   */
  status = calc_avhrr_channels(img, ch1, ch2, ch3b, ch4, ch5, ch3a, soz, saz);
  if (status != 0)  {
    fprintf(stderr,"\nERROR: while extracting AVHRR data.\n");
    exit(10);
  }

  /* 
   * Create HDF file with AVHRR fields 
   */
  status = create_HDF_avhrr_file(avhrrhead, outfHDFavhrr, ch1, ch2,
				 ch3b, ch4, ch5, ch3a, soz, saz);
  if (status != 0)  {
    fprintf(stderr,"\nERROR: while creating HDF AVHRR file.\n");
    exit(10);
  }

  free(ch1);
  free(ch2);
  free(ch3b);
  free(ch4);
  free(ch5);
  free(ch3a);
  free(soz);
  free(saz);

  return(0);
}

void usage() {
  printf(" SYNTAX: avhrr2hdf -i <infile> -o <outfile>\n");
  printf("\tinfile: AVHRR data in METSAT-format\n");
  printf("\toutfile: AVHRR data in HDF5-format\n");
  exit(FM_OK);
}
